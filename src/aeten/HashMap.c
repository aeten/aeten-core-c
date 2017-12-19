#include "HashMap.h"

#define import
#include "Object.h"
#include "Iterator.h"
#include "MapEntry.h"
#include "Collection.h"
#include "Set.h"
#include "Map.h"
#include "List.h"
#include "Hash.h"
#include "ArrayList.h"
#include "HashMapEntry.h"
#include "HashMapKeySet.h"

/*! \file
@startuml HashMap
!include Object.c
!include Map.c
!include List.c
namespace aeten {
	class HashMap<K extends Object,V extends Object> implements Map {
		+ {static} HashMap() <<constructor>>
		# finalize() <<override>>
        + V put(K key, V value) <<override>>
        + V remove(K key) <<override>>
		- size : size_t
		- hash_min: uint64_t
		- hash_max: uint64_t
		- bits: uint32_t
		- key_size: size_t
		- value_size: size_t
        - hash_array: List*
	}
}
@enduml

@startuml HashMapEntry
!include Object.c
!include MapEntry.c
!include HashMap.c!HashMap
namespace aeten {
	class HashMapEntry<K extends Object,V extends Object> implements MapEntry {
		+ {static} HashMapEntry(Object key, Object value) <<constructor>>
		- key: Object
		- value: Object
	}
}
@enduml

@startuml HashMapKeySet
!include Object.c
!include Set.c
!include HashMap.c!HashMap
namespace aeten {
    class HashMapKeySet<T extends Object> implements Set {
		+ {static} HashMapKeySet(HashMap* map) <<constructor>>
		- map: HashMap*
	}
}
@enduml
*/

#define DEFAULT_HASH_BITS 7


void HashMap_new(HashMap* self) {
	self->_key_size = sizeof(Object);
	self->_value_size = sizeof(Object);
	self->_bits = DEFAULT_HASH_BITS;
	self->_hash_min = UINT64_MAX;
	self->_hash_max = 0;
	self->_size = 0;
	self->_hash_array = NULL;
}

void HashMap_finalize(HashMap *self) {
	int size = self->_hash_max-self->_hash_min;
	for(int idx=0; idx<size; ++idx) {
        if (self->_hash_array[idx]._object_ != NULL) {
			List_delete(self->_hash_array[idx]);
		}
	}
	free(self->_hash_array);
}

static Object get_hashed(HashMap *self, Object *key, uint64_t hash, int *index) {
	int idx = hash - self->_hash_min;
	Object entry;

    if ((self->_hash_array == NULL) || (idx > (self->_hash_max - self->_hash_min)) || (Object_isNull(Object_cast(&self->_hash_array[idx])))) {
		return (Object){0};
	}
	for(int i=0; i<List_size(self->_hash_array[idx]); ++i) {
		entry = List_get(self->_hash_array[idx], i);
		if(!isNull(&entry)) {
			HashMapEntry *hm_entry = (HashMapEntry*)entry._object_;
			if (Object_equals(*key, &hm_entry->_key)) {
				if (index != NULL) *index = i;
				return hm_entry->_value;
			}
		}
	}
	return (Object){0};
}

Object HashMap_get(HashMap *self, Object key) {
	return get_hashed(self, &key, Object_hashCode(key), NULL);
}

/// @startuml HashMap.put
/// start
/// @pause
Object HashMap_put(HashMap *self, Object key, Object value) {
	int idx, idx_subarray;
    uint64_t hash = Object_hashCode(key);
	Object old_value = get_hashed(self, &key, hash, &idx_subarray);

    /// @append if(have no old value) then (yes)
	if (Object_isNull(old_value)) {
		/// @append if(hash>max) then (yes)
		if (hash > self->_hash_max) {
			uint64_t old_max = self->_hash_max;
			self->_hash_max = hash;
			// If is first allocation
			if (self->_hash_min == UINT64_MAX) {
				self->_hash_min = hash;
				old_max = hash;
			}
			size_t size	    = self->_hash_max - self->_hash_min + 1;
			/// @append increase hash array;
            self->_hash_array = realloc(self->_hash_array, size * sizeof(List));
            check(self->_hash_array, HeadAllocationException, "realloc(%zu)", size * sizeof(List));
			// If is first allocation
			if ((old_max - self->_hash_min) == 0) {
                memset(self->_hash_array, 0x0, sizeof(List));
			} else {
                memset(self->_hash_array + (old_max - self->_hash_min + 1), 0x0, (self->_hash_max - old_max) * sizeof(List));
			}
		}
		/// @append endif
		/// @append if(hash<min) then (yes)
		if (hash < self->_hash_min) {
            size_t old_size   = (self->_hash_max - self->_hash_min + 1);
            uint64_t old_min  = self->_hash_min;
			self->_hash_min	  = hash;
			size_t size	      = self->_hash_max - self->_hash_min + 1;
			/// @append increase hash array;
            self->_hash_array = realloc(self->_hash_array, size * sizeof(List));
            check(self->_hash_array, HeadAllocationException, "realloc(%zu)", size * sizeof(List));
			/// @append Move...;
			memmove(self->_hash_array + (old_min - self->_hash_min), self->_hash_array, old_size * sizeof(List));
			memset(self->_hash_array, 0x0, (old_min - self->_hash_min) * sizeof(List));
		}
		/// @append endif
		idx = hash - self->_hash_min;
		if (self->_hash_array == NULL) {
			self->_hash_array = calloc(1, sizeof(List));
			check(self->_hash_array, HeadAllocationException, "malloc(%zu)", sizeof(List));
		}
		if (self->_hash_array[idx]._object_ == NULL) {
			self->_hash_array[idx] = List_cast(new_ArrayList(1));
		}
		++self->_size;
	}
    /// @append endif

	HashMapEntry *entry = new_HashMapEntry(key, value);
	if (isNull(&old_value)) {
		List_add(self->_hash_array[idx], Object_cast(entry));
	} else {
		List_set(self->_hash_array[idx], idx_subarray, Object_cast(entry));
	}
	return old_value;
}
///! @append @enduml

Object HashMap_remove(HashMap* self, Object key) {
	uint64_t hash = Object_hashCode(key);
	int idx_subarray;
	int idx = hash - self->_hash_min;
	Object value = get_hashed(self, &key, hash, &idx_subarray);
	if (!isNull(&value)) {
		List_removeAt(self->_hash_array[idx], idx_subarray);
	}
	return value;
}

size_t HashMap_size(HashMap* self) {
	return self->_size;
}

Collection HashMap_values(HashMap* self) {
	// TODO: Implement
	return (Collection){0};
}

Set HashMap_entrySet(HashMap* self) {
	// TODO: Implement
	return (Set){0};
}

Set HashMap_keySet(HashMap* self) {
    return Set_cast(new_HashMapKeySet(self));
}

void HashMapEntry_new(HashMapEntry *self, Object key, Object value) {
	self->_key = key;
	self->_value = value;
}

Object HashMapEntry_getKey(HashMapEntry *self) {
	return self->_key;
}

Object HashMapEntry_getValue(HashMapEntry *self) {
	return self->_value;
}

void HashMapEntry_setValue(HashMapEntry *self, Object value) {
	self->_value = value;
}

/* HashMapKeySet*/
void HashMapKeySet_new(HashMapKeySet* self, HashMap *map) {
	self->_map = map;
}

bool HashMapKeySet_contains(HashMapKeySet* self, Object element) {
	return (!Object_isNull(HashMap_get(self->_map, element)));
}

size_t HashMapKeySet_size(HashMapKeySet* self) {
	return HashMap_size(self->_map);
}

Iterator HashMapKeySet_iterator(HashMapKeySet* self) {
	// TODO
	return (Iterator){0};
}
