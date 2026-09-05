# AGENTS.md - Hash Table Implementation in C

## Project Overview

This is a **hobby project** implementing a **hash table data structure from scratch in C**, based on James Routley's [write-a-hash-table](https://github.com/jamesroutley/write-a-hash-table) tutorial. The focus is on **low-level details** of basic data structure implementation, memory management, and algorithmic efficiency.

- **Language**: C (C99 standard)
- **Compiler**: `gcc` (via Makefile)
- **Build System**: Makefile with targets: `build`, `build-debug`, `run`, `debugger`, `clean`
- **Entry Point**: `src/main.c` (demonstrates core operations)

## Version Control

- **Strategy**: Direct commits to `main` branch (no feature branches)
- **Commit Style**: Descriptive messages (e.g., "implemented resizing", "added size param to ht constructor")
- **No PR process**: This is a personal learning project

---

## Codebase Structure

```
.
├── include/
│   ├── hash_table.h    # Hash table type definitions and public API
│   └── utils.h         # Hash function and prime number utilities
├── src/
│   ├── hash_table.c    # Core hash table implementation
│   ├── main.c          # Demo program exercising all operations
│   └── utils.c         # Hashing and prime number implementations
├── Makefile            # Build configuration
└── README.md           # Project description
```

---

## Data Structures

### `KVPair` (Key-Value Pair)
- **Location**: `include/hash_table.h`
- **Fields**:
  - `char *key` - Null-terminated string key
  - `char *value` - Null-terminated string value
- **Memory**: Dynamically allocated with `malloc` and `strdup`

### `HashTable`
- **Location**: `include/hash_table.h`
- **Fields**:
  - `int size` - Number of buckets (always a prime number)
  - `int count` - Number of active (non-deleted) items
  - `kv_pair_t **items` - Array of pointers to KVPair objects (or NULL, or DELETED_ITEM sentinel)

### Sentinel Values
- **`DELETED_ITEM`**: A static `KVPair` with `NULL` key/value used to mark deleted slots in open addressing

---

## Core Algorithms

### Hashing
- **Location**: `src/utils.c`
- **Method**: **Double hashing** for collision resolution
  - Primary hash: Uses prime base `151`
  - Secondary hash: Uses prime base `163`
  - Formula: `(hashed_a + (retries * (hashed_b + 1))) % num_buckets`
- **Hash Function**: Polynomial rolling hash: `Σ(prime^(length-i-1) * char)`

### Collision Resolution
- **Strategy**: **Open addressing with linear probing** (via retries in `get_hash`)
- **Probing**: Incremental retries using the double hash to find next slot

### Resizing
- **Location**: `src/hash_table.c`
- **Triggers**:
  - **Upsize**: When load factor > `0.7` (70%)
  - **Downsize**: When load factor < `0.1` (10%)
- **Process**:
  1. Create a new table with new size (2x for upsize, 1/2 for downsize)
  2. Rehash all existing items into the new table
  3. Swap internal pointers to avoid full copy
  4. Free the old table
- **Minimum Size**: `HT_BASE_SIZE = 47` (a prime number)

### Prime Numbers
- **Location**: `src/utils.c`
- **Functions**:
  - `is_prime(int x)`: Checks if `x` is prime (optimized: only checks odd divisors up to √x)
  - `next_prime(int x)`: Finds the next prime number ≥ x
- **Purpose**: Table sizes are always prime to reduce clustering

---

## Memory Management

- **Allocation**: Uses `malloc` and `calloc` for structs, `strdup` for strings
- **Deallocation**: `free` for all allocated memory
- **Ownership**: Hash table owns all KVPairs it contains
- **Cleanup**: `delete_hash_table` recursively frees all items and the table itself
- **Resizing Memory**: During resize, old items are copied to new table, then old table is freed

---

## Key Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `HT_BASE_SIZE` | 47 | Default/initial table size (prime) |
| `UPSIZE_TRIGGER` | 0.7 | Load factor threshold for upsizing |
| `DOWNSIZE_TRIGGER` | 0.1 | Load factor threshold for downsizing |
| Prime bases for hashing | 151, 163 | Used in double hashing |

---

## Public API

### Functions in `hash_table.h`

| Function | Description |
|----------|-------------|
| `hash_table_t *create_hash_table(const int size)` | Creates a new hash table with size rounded up to next prime |
| `void delete_hash_table(hash_table_t *ht)` | Frees all memory associated with the hash table |
| `void hash_table_insert(hash_table_t *ht, const char *key, const char *value)` | Inserts or updates a key-value pair; triggers upsize if needed |
| `char *hash_table_search(hash_table_t *ht, const char *key)` | Returns value for key, or NULL if not found |
| `void hash_table_remove(hash_table_t *ht, const char *key)` | Removes a key-value pair; triggers downsize if needed |

### Functions in `utils.h`

| Function | Description |
|----------|-------------|
| `int get_hash(const char *key, const int num_buckets, const int retries)` | Computes hash index with probing support |
| `int next_prime(int x)` | Returns the next prime number ≥ x |

---

## Implementation Details

### Insert Logic
1. Creates a new KVPair with `strdup` for key and value
2. Uses `get_hash` with incremental retries to find an empty slot (NULL or DELETED_ITEM)
3. If the key already exists, deletes the old KVPair before inserting
4. Increments `count`
5. Checks load factor and triggers `resize_hash_table` if > 0.7

### Search Logic
1. Computes initial hash with `retries = 0`
2. Probes sequentially using `get_hash` with increasing retries
3. Returns the value if key matches, NULL if empty slot encountered

### Remove Logic
1. Finds the key using probing (same as search)
2. Frees the KVPair and replaces the slot with `&DELETED_ITEM` sentinel
3. Decrements `count`
4. Checks load factor and triggers downsize if < 0.1

### Resize Logic
1. Validates new size is ≥ `HT_BASE_SIZE`
2. Creates a new table with the new size
3. Rehashes all non-NULL, non-DELETED_ITEM entries into the new table
4. Swaps `size`, `count`, and `items` pointers between old and new tables
5. Deletes the new table (which now holds the old data)

---

## Build & Run

### Compilation
```bash
# Regular build
make build

# Debug build (with -g flag)
make build-debug

# Clean build artifacts
make clean
```

### Execution
```bash
make run      # Runs the demo program
make debugger # Starts lldb debugger
```

### Manual Compilation
```bash
gcc -Iinclude -o build/main.o src/main.c src/hash_table.c src/utils.c
./build/main.o
```

---

## Demo Program Flow (main.c)

1. Creates an empty hash table with `HT_BASE_SIZE`
2. Prints initial size and count
3. Inserts `{"hello": "world"}` and verifies search
4. Updates `"hello"` to `"bye"` (replaces existing value)
5. Deletes `"hello"` and verifies count
6. Inserts 33 elements to trigger **upsizing**
7. Removes 24 elements to trigger **downsizing**
8. Cleans up with `delete_hash_table`

---

## Coding Style & Conventions

### Formatting
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style (opening brace on same line)
- **Naming**:
  - `snake_case` for functions and variables
  - `PascalCase` for types (e.g., `hash_table_t`, `kv_pair_t`)
  - `SCREAMING_SNAKE_CASE` for constants
- **Comments**: Single-line (`//`) for explanations, multi-line (`/* */`) for docstrings

### Memory Safety
- **Always check for NULL** before dereferencing
- **Free all allocated memory** in symmetric order
- **Use sentinels** (`DELETED_ITEM`) to mark special states
- **Copy strings** with `strdup` (not raw pointers) to avoid ownership issues

### Error Handling
- **No explicit error handling** in current implementation (assumes malloc always succeeds)
- **Future improvement**: Add NULL checks for malloc/calloc/strdup

---

## Low-Level Focus Areas

When working on this project, pay special attention to:

1. **Hash Function Quality**: The polynomial hash can cause overflow with long strings. Consider modulo operations during computation.
2. **Prime Number Distribution**: The `is_prime` function has a quirk: returns `true` for x < 4 (commented as "untrue mathematically but it works for our purpose").
3. **Resizing Efficiency**: The current resize swaps pointers to avoid reallocating the entire items array. This is clever but subtle.
4. **Open Addressing**: Using `DELETED_ITEM` sentinel allows probing to continue past deleted items while still marking them as available for insertion.
5. **Load Factor Calculation**: Uses floating-point division `(float)ht->count / ht->size` for accuracy.

---

## Common Tasks

| Task | Command/Location |
|------|-----------------|
| Build | `make build` |
| Run demo | `make run` |
| Debug | `make build-debug` then `make debugger` |
| Add new feature | Edit `src/hash_table.c` and `include/hash_table.h` |
| Test manually | Run `make run` and verify output |
| Clean up | `make clean` |

---

## Known Limitations / Future Improvements

- **No error handling** for malloc failures
- **String-only keys/values** (could be generalized to void* with size)
- **No iterator** for traversing all items
- **Fixed hash function** (could support custom hash functions)
- **No thread safety** (not designed for concurrent access)
- **Basic prime checking** (could use a sieve for repeated calls)

---

## Git History Context

Recent commits indicate active development on:
- **Resizing**: Added case to main, implemented full resize logic
- **Table sizing**: Added `size` parameter to constructor
- **Prime utilities**: Implemented `is_prime` and `next_prime`
- **Code style**: Changed function naming convention

---

## How to Contribute (for self)

1. **Make small, focused commits** to `main`
2. **Test each change** by running `make run`
3. **Verify memory safety** - no leaks or double-frees
4. **Keep the demo in `main.c`** updated to showcase new features
5. **Add comments** explaining non-obvious algorithmic choices
