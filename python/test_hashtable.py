#!/usr/bin/env python3
"""
Test script for the hashtable Python C extension.
Tests various key types, operations, and error handling.
"""

import sys
import traceback
import gc

def test_basic_operations():
    """Test basic insert, search, remove operations."""
    print("=== Testing Basic Operations ===")
    
    try:
        from hashtable import HashTable
        
        # Create a hash table
        ht = HashTable()
        assert ht is not None, "Failed to create HashTable"
        
        # Test initial state
        assert len(ht) == 0, f"Expected length 0, got {len(ht)}"
        assert ht.size() > 0, "Expected size > 0"
        print(f"✓ Created HashTable with size {ht.size()}, count {len(ht)}")
        
        # Test insert and search
        ht.insert("hello", "world")
        result = ht.search("hello")
        assert result == "world", f"Expected 'world', got {result}"
        print("✓ Insert and search work")
        
        # Test dict-like access
        ht["key"] = "value"
        assert ht["key"] == "value", "Dict-like access failed"
        print("✓ Dict-like syntax works")
        
        # Test contains
        assert "hello" in ht, "Contains check failed for existing key"
        assert "nonexistent" not in ht, "Contains check failed for non-existing key"
        print("✓ 'in' operator works")
        
        # Test length
        assert len(ht) == 2, f"Expected length 2, got {len(ht)}"
        print("✓ Length tracking works")
        
        # Test remove
        ht.remove("hello")
        assert "hello" not in ht, "Remove failed"
        assert len(ht) == 1, f"Expected length 1 after remove, got {len(ht)}"
        print("✓ Remove works")
        
        # Test delete with del
        del ht["key"]
        assert len(ht) == 0, "Delete with 'del' failed"
        print("✓ 'del' operator works")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic operations test failed: {e}")
        traceback.print_exc()
        return False


def test_various_key_types():
    """Test with different key types: strings, integers, floats, tuples."""
    print("\n=== Testing Various Key Types ===")
    
    try:
        from hashtable import HashTable
        
        ht = HashTable()
        
        # String keys
        ht.insert("string_key", "string_value")
        assert ht["string_key"] == "string_value"
        print("✓ String keys work")
        
        # Integer keys
        ht.insert(42, "answer")
        assert ht[42] == "answer"
        print("✓ Integer keys work")
        
        # Float keys
        ht.insert(3.14, "pi")
        assert ht[3.14] == "pi"
        print("✓ Float keys work")
        
        # Tuple keys
        ht.insert((1, 2, 3), "tuple_value")
        assert ht[(1, 2, 3)] == "tuple_value"
        print("✓ Tuple keys work")
        
        # Verify all are present
        assert len(ht) == 4, f"Expected 4 items, got {len(ht)}"
        assert "string_key" in ht
        assert 42 in ht
        assert 3.14 in ht
        assert (1, 2, 3) in ht
        print("✓ All key types coexist correctly")
        
        return True
        
    except Exception as e:
        print(f"✗ Various key types test failed: {e}")
        traceback.print_exc()
        return False


def test_error_handling():
    """Test error handling for unhashable types and missing keys."""
    print("\n=== Testing Error Handling ===")
    
    try:
        from hashtable import HashTable
        
        ht = HashTable()
        
        # Test unhashable type - list
        try:
            ht.insert([1, 2, 3], "value")
            print("✗ Should have raised TypeError for list key")
            return False
        except TypeError as e:
            print(f"✓ Correctly raised TypeError for list key: {e}")
        
        # Test unhashable type - dict
        try:
            ht[["key"]] = "value"
            print("✗ Should have raised TypeError for dict key")
            return False
        except TypeError as e:
            print(f"✓ Correctly raised TypeError for dict key: {e}")
        
        # Test missing key with dict access
        try:
            value = ht["nonexistent"]
            print("✗ Should have raised KeyError for missing key")
            return False
        except KeyError as e:
            print(f"✓ Correctly raised KeyError for missing key: {e}")
        
        # Test missing key with search (should return None)
        result = ht.search("nonexistent")
        assert result is None, "Search for missing key should return None"
        print("✓ Search for missing key returns None")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False


def test_update_existing():
    """Test updating existing keys."""
    print("\n=== Testing Update Existing Keys ===")
    
    try:
        from hashtable import HashTable
        
        ht = HashTable()
        
        # Insert initial value
        ht.insert("key", "value1")
        assert ht["key"] == "value1"
        
        # Update with insert
        ht.insert("key", "value2")
        assert ht["key"] == "value2"
        print("✓ Insert updates existing key")
        
        # Update with dict syntax
        ht["key"] = "value3"
        assert ht["key"] == "value3"
        print("✓ Dict syntax updates existing key")
        
        # Length should still be 1
        assert len(ht) == 1
        print("✓ Length unchanged after update")
        
        return True
        
    except Exception as e:
        print(f"✗ Update existing test failed: {e}")
        traceback.print_exc()
        return False


def test_resizing():
    """Test that resizing works correctly."""
    print("\n=== Testing Resizing ===")
    
    try:
        from hashtable import HashTable
        
        # Create a hash table with small initial size
        ht = HashTable(10)
        initial_size = ht.size()
        print(f"Initial size: {initial_size}")
        
        # Insert many items to trigger upsizing
        for i in range(50):
            ht.insert(f"key_{i}", f"value_{i}")
        
        new_size = ht.size()
        print(f"Size after inserting 50 items: {new_size}")
        assert new_size > initial_size, "Expected size to increase"
        assert len(ht) == 50, f"Expected 50 items, got {len(ht)}"
        print("✓ Upsizing works")
        
        # Remove many items to trigger downsizing
        for i in range(40):
            ht.remove(f"key_{i}")
        
        final_size = ht.size()
        print(f"Size after removing 40 items: {final_size}")
        assert len(ht) == 10, f"Expected 10 items, got {len(ht)}"
        print("✓ Downsizing works")
        
        return True
        
    except Exception as e:
        print(f"✗ Resizing test failed: {e}")
        traceback.print_exc()
        return False


def test_memory_management():
    """Test that memory management (reference counting) works correctly."""
    print("\n=== Testing Memory Management ===")
    
    try:
        from hashtable import HashTable
        import gc
        
        # Create hash table
        ht = HashTable()
        
        # Insert some items
        ht.insert("key1", "value1")
        ht.insert("key2", "value2")
        ht.insert("key3", "value3")
        
        # Get references to the values
        v1 = ht["key1"]
        v2 = ht["key2"]
        
        # Remove from hash table
        ht.remove("key1")
        ht.remove("key2")
        
        # Values should still exist because we have references
        assert v1 == "value1"
        assert v2 == "value2"
        print("✓ Values still accessible after removal from hash table")
        
        # Delete our references and force garbage collection
        del v1, v2
        gc.collect()
        
        # Hash table should still work
        assert ht["key3"] == "value3"
        print("✓ Hash table still works after gc")
        
        # Delete hash table
        del ht
        gc.collect()
        
        print("✓ Memory management test passed")
        return True
        
    except Exception as e:
        print(f"✗ Memory management test failed: {e}")
        traceback.print_exc()
        return False


def test_custom_initial_size():
    """Test creating hash table with custom initial size."""
    print("\n=== Testing Custom Initial Size ===")
    
    try:
        from hashtable import HashTable
        
        ht = HashTable(size=100)
        # Size should be rounded up to next prime
        assert ht.size() >= 100, f"Expected size >= 100, got {ht.size()}"
        print(f"✓ Created hash table with custom size: {ht.size()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Custom initial size test failed: {e}")
        traceback.print_exc()
        return False


def run_all_tests():
    """Run all tests and report results."""
    print("Running hashtable Python C extension tests...\n")
    
    tests = [
        test_basic_operations,
        test_various_key_types,
        test_error_handling,
        test_update_existing,
        test_resizing,
        test_memory_management,
        test_custom_initial_size,
    ]
    
    results = []
    for test in tests:
        try:
            result = test()
            results.append(result)
        except Exception as e:
            print(f"✗ Test {test.__name__} crashed: {e}")
            traceback.print_exc()
            results.append(False)
    
    print("\n" + "="*50)
    passed = sum(results)
    total = len(results)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("All tests passed!")
        return True
    else:
        print("Some tests failed")
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)