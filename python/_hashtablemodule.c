#include <Python.h>
#include "../include/hash_table.h"

// Callback functions for Python integration

// Hash callback - uses PyObject_Hash and double hashing
static unsigned long pyobject_hash(void *key, int num_buckets, int retries)
{
    PyObject *obj = (PyObject *)key;
    Py_ssize_t py_hash = PyObject_Hash(obj);
    if (py_hash == -1) {
        // PyObject_Hash returns -1 and sets an exception for unhashable types
        // Return 0 to signal error, but this should never be called with unhashable types
        // if the Python code checks hashability first (which it does)
        return 0;
    }
    
    // Use absolute value to handle negative hashes
    unsigned long abs_hash = py_hash < 0 ? (unsigned long)(-py_hash) : (unsigned long)py_hash;
    unsigned long h1 = abs_hash % num_buckets;
    // Ensure h2 is never 0 to avoid infinite loops in probing
    unsigned long h2 = (abs_hash >> 16);
    if (h2 == 0) {
        h2 = 1;
    }
    return (h1 + (retries * (h2 + 1))) % num_buckets;
}

// Compare callback - uses PyObject_RichCompareBool
static int pyobject_compare(void *a, void *b)
{
    PyObject *obj_a = (PyObject *)a;
    PyObject *obj_b = (PyObject *)b;
    int result = PyObject_RichCompareBool(obj_a, obj_b, Py_EQ);
    if (result == -1 && PyErr_Occurred()) {
        return -1;
    }
    return result ? 0 : 1;
}

// Free callback - uses Py_DECREF
static void pyobject_decref(void *obj)
{
    Py_DECREF((PyObject *)obj);
}

// Python object wrapper for HashTable
typedef struct {
    PyObject_HEAD
    hash_table_t *c_table;  // Pointer to C hash table
} PyHashTableObject;

// Method declarations
static PyObject *HashTable_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);
static int HashTable_init(PyHashTableObject *self, PyObject *args, PyObject *kwargs);
static void HashTable_dealloc(PyHashTableObject *self);
static PyObject *HashTable_insert(PyHashTableObject *self, PyObject *args);
static PyObject *HashTable_search(PyHashTableObject *self, PyObject *args);
static PyObject *HashTable_remove(PyHashTableObject *self, PyObject *args);
static PyObject *HashTable_size(PyHashTableObject *self, PyObject *args);
static Py_ssize_t HashTable_len(PyHashTableObject *self);
static int HashTable_contains(PyHashTableObject *self, PyObject *key);
static PyObject *HashTable_getitem(PyHashTableObject *self, PyObject *key);
static int HashTable_setitem(PyHashTableObject *self, PyObject *key, PyObject *value);
static int HashTable_delitem(PyHashTableObject *self, PyObject *key);
// static PyObject *HashTable_delitem_wrapper(PyHashTableObject *self, PyObject *args);

// Method definitions
static PyMethodDef HashTable_methods[] = {
    {"insert", (PyCFunction)HashTable_insert, METH_VARARGS, "Insert a key-value pair"},
    {"search", (PyCFunction)HashTable_search, METH_VARARGS, "Search for a key and return its value"},
    {"remove", (PyCFunction)HashTable_remove, METH_VARARGS, "Remove a key-value pair"},
    {"size", (PyCFunction)HashTable_size, METH_NOARGS, "Return the number of buckets"},
    // {"__delitem__", (PyCFunction)HashTable_delitem_wrapper, METH_VARARGS, "Delete a key-value pair"},
    {NULL, NULL, 0, NULL}
};

// Type definition
static PyTypeObject HashTableType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "hashtable.HashTable",
    .tp_basicsize = sizeof(PyHashTableObject),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)HashTable_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "Hash table implementation in C",
    .tp_methods = HashTable_methods,
    .tp_new = HashTable_new,
    .tp_init = (initproc)HashTable_init,
    .tp_as_mapping = &(PyMappingMethods){
        .mp_length = (lenfunc)HashTable_len,
        .mp_subscript = (binaryfunc)HashTable_getitem,
        .mp_ass_subscript = (objobjargproc)HashTable_setitem,
    },
    .tp_as_sequence = &(PySequenceMethods){
        .sq_contains = (objobjproc)HashTable_contains,
        .sq_length = (lenfunc)HashTable_len,
    },
};

// Module definition
static PyModuleDef hashtablemodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_hashtable",
    .m_doc = "Python C extension for hash table",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit__hashtable(void)
{
    PyObject *m;
    
    if (PyType_Ready(&HashTableType) < 0) {
        return NULL;
    }
    
    m = PyModule_Create(&hashtablemodule);
    if (m == NULL) {
        return NULL;
    }
    
    Py_INCREF(&HashTableType);
    if (PyModule_AddObject(m, "HashTable", (PyObject *)&HashTableType) < 0) {
        Py_DECREF(&HashTableType);
        Py_DECREF(m);
        return NULL;
    }
    
    return m;
}

// Implementation of methods

static PyObject *HashTable_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    PyHashTableObject *self = (PyHashTableObject *)type->tp_alloc(type, 0);
    if (self) {
        self->c_table = NULL;
    }
    return (PyObject *)self;
}

static int HashTable_init(PyHashTableObject *self, PyObject *args, PyObject *kwargs)
{
    int size = HT_BASE_SIZE;
    
    // Parse optional size argument
    static char *kwlist[] = {"size", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", kwlist, &size)) {
        return -1;
    }
    
    // Create the hash table with Python-specific callbacks
    self->c_table = create_generic_hash_table(
        size,
        pyobject_hash,
        pyobject_compare,
        pyobject_decref,
        pyobject_decref
    );
    
    if (self->c_table == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create hash table");
        return -1;
    }
    
    return 0;
}

static void HashTable_dealloc(PyHashTableObject *self)
{
    if (self->c_table) {
        delete_hash_table(self->c_table);
        self->c_table = NULL;
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *HashTable_insert(PyHashTableObject *self, PyObject *args)
{
    PyObject *key, *value;
    
    if (!PyArg_ParseTuple(args, "OO", &key, &value)) {
        return NULL;
    }
    
    // Check if key is hashable
    if (PyObject_Hash(key) == -1) {
        // PyObject_Hash already sets an exception if it fails
        return NULL;
    }
    
    // Check if value is hashable (for consistency)
    if (PyObject_Hash(value) == -1) {
        PyErr_SetString(PyExc_TypeError, "unhashable type: 'value'");
        return NULL;
    }
    
    // Increment reference counts before storing
    Py_INCREF(key);
    Py_INCREF(value);
    
    // Insert into the hash table
    hash_table_insert(self->c_table, (void *)key, (void *)value);
    
    Py_RETURN_NONE;
}

static PyObject *HashTable_search(PyHashTableObject *self, PyObject *args)
{
    PyObject *key;
    
    if (!PyArg_ParseTuple(args, "O", &key)) {
        return NULL;
    }
    
    // Check if key is hashable
    if (PyObject_Hash(key) == -1) {
        // PyObject_Hash already sets an exception if it fails
        return NULL;
    }
    
    // Search for the key
    void *found = hash_table_search(self->c_table, (void *)key);
    
    if (found == NULL) {
        Py_RETURN_NONE;
    }
    
    // Return the found object (already a PyObject*)
    PyObject *result = (PyObject *)found;
    Py_INCREF(result);
    return result;
}

static PyObject *HashTable_remove(PyHashTableObject *self, PyObject *args)
{
    PyObject *key;
    
    if (!PyArg_ParseTuple(args, "O", &key)) {
        return NULL;
    }
    
    // Check if key is hashable
    if (PyObject_Hash(key) == -1) {
        // PyObject_Hash already sets an exception if it fails
        return NULL;
    }
    
    // Remove the key
    hash_table_remove(self->c_table, (void *)key);
    
    Py_RETURN_NONE;
}

static PyObject *HashTable_size(PyHashTableObject *self, PyObject *args)
{
    return PyLong_FromLong(self->c_table->size);
}

static Py_ssize_t HashTable_len(PyHashTableObject *self)
{
    return (Py_ssize_t)self->c_table->count;
}

static int HashTable_contains(PyHashTableObject *self, PyObject *key)
{
    // Check if key is hashable by attempting to hash it
    Py_ssize_t hash_result = PyObject_Hash(key);
    if (hash_result == -1) {
        // PyObject_Hash sets PyErr_Occurred() if it fails
        return -1;
    }
    
    void *found = hash_table_search(self->c_table, (void *)key);
    
    if (found == NULL) {
        return 0;
    }
    
    return 1;
}

static PyObject *HashTable_getitem(PyHashTableObject *self, PyObject *key)
{
    // Check if key is hashable
    if (PyObject_Hash(key) == -1) {
        // PyObject_Hash already sets an exception if it fails
        return NULL;
    }
    
    void *found = hash_table_search(self->c_table, (void *)key);
    
    if (found == NULL) {
        PyErr_SetString(PyExc_KeyError, "key not found");
        return NULL;
    }
    
    PyObject *result = (PyObject *)found;
    Py_INCREF(result);
    return result;
}

static int HashTable_setitem(PyHashTableObject *self, PyObject *key, PyObject *value)
{
    // Check if key is hashable
    if (PyObject_Hash(key) == -1) {
        // PyObject_Hash already sets an exception if it fails
        return -1;
    }
    
    // Check if value is hashable (for consistency)
    if (PyObject_Hash(value) == -1) {
        PyErr_SetString(PyExc_TypeError, "unhashable type: 'value'");
        return -1;
    }
    
    // Increment reference counts before storing
    Py_INCREF(key);
    Py_INCREF(value);
    
    // Insert into the hash table
    hash_table_insert(self->c_table, (void *)key, (void *)value);
    
    return 0;
}

static int HashTable_delitem(PyHashTableObject *self, PyObject *key)
{
    // Check if key is hashable
    if (PyObject_Hash(key) == -1) {
        // PyObject_Hash already sets an exception if it fails
        return -1;
    }
    
    // Remove the key
    hash_table_remove(self->c_table, (void *)key);
    
    return 0;
}

// Wrapper for __delitem__ method (temporarily disabled)
// static PyObject *HashTable_delitem_wrapper(PyHashTableObject *self, PyObject *args)
// {
//     PyObject *key;
//     
//     if (!PyArg_ParseTuple(args, "O", &key)) {
//         return NULL;
//     }
//     
//     // Increment refcount to keep key alive during the operation
//     Py_INCREF(key);
//     if (HashTable_delitem(self, key) < 0) {
//         Py_DECREF(key);
//         return NULL;
//     }
//     Py_DECREF(key);
//     
//     Py_RETURN_NONE;
// }