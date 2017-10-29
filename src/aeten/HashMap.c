#include "HashMap.h"

#define import
#include "Iterator.h"
#include "MapEntry.h"
#include "Collection.h"
#include "Set.h"
#include "List.h"
#include "Hash.h"
#include "ArrayList.h"
#include "HashMapEntry.h"

/*! \file
@startuml
!include Object.c
!include Map.c
!include List.c
namespace aeten {
	class HashMap<K,V> implements Map {
		- size : size_t
		- hash_min: uint64_t
		- hash_max: uint64_t
		- bits: uint32_t
		- key_size: size_t
		- value_size: size_t
		- hash_array: List**

		{static} + HashMap(size_t key_size, size_t value_size) <<constructor>>
		# finalize() <<override>>
		V* put(K* key, V* value) <<override>>
		V* remove(K* key) <<override>>
	}
}
@enduml

@startuml HashMapEntry
!include Object.c
namespace aeten {
	class HashMapEntry implements MapEntry
}
@enduml


*/

#define DEFAULT_HASH_BITS 7


void HashMap_new(HashMap* self, size_t key_size, size_t value_size) {
	self->_key_size = key_size;
	self->_value_size = value_size;
	self->_bits = DEFAULT_HASH_BITS;
	self->_hash_min = 0;
	self->_hash_max = 0;
	self->_size = 0;
	self->_hash_array = NULL;
}

void HashMap_finalize(HashMap *self) {
	int size = self->_hash_max-self->_hash_min;
	for(int idx=0; idx<size; ++idx) {
		if (self->_hash_array[idx] != NULL) {
			List_delete(self->_hash_array[idx]);
		}
	}
	free(self->_hash_array);
}

static void *get_hashed(HashMap *self, void *key, uint64_t hash, int *index) {
	int idx = hash - self->_hash_min;
	uint8_t *entry;
	bool is_object = isObject(key);

	if ((self->_hash_array == NULL) || (idx > (self->_hash_max - self->_hash_min)) || (self->_hash_array[idx] == NULL)) {
		return NULL;
	}
	for(int i=0; i<List_size(self->_hash_array[idx]); ++i) {
		if((entry = (uint8_t*)List_get(self->_hash_array[idx], i)) != NULL) {
			if (is_object? Object_equals((Object*)key, entry): (0 == memcmp(key, entry, self->_key_size))) {
				if (index != NULL) *index = i;
				return (entry + self->_key_size);
			}
		}
	}
	return NULL;
}

void *HashMap_get(HashMap *self, void *key) {
	uint64_t hash = isObject(key)? Object_hashCode((Object*)key): Hash_hashBuffer((char*)key, self->_key_size, self->_bits);
	return get_hashed(self, key, hash, NULL);
}

/// @startuml HashMap.put
/// start
/// @pause
void *HashMap_put(HashMap *self, void* key, void *value) {
	int idx, idx_subarray;
	uint64_t hash = isObject(key)? Object_hashCode((Object*)key): Hash_hashBuffer((char*)key, self->_key_size, self->_bits);
	void *old_value = get_hashed(self, key, hash, &idx_subarray);

	/// @append if(old value is null) then (yes)
	if (old_value == NULL) {
		/// @append if(hash>max) then (yes)
		if (hash > self->_hash_max) {
			uint64_t old_max = self->_hash_max;
			self->_hash_max = hash;
			// If is first allocation
			if (self->_hash_min == UINT64_MAX) {
				self->_hash_min = hash;
			}
			size_t size       = self->_hash_max - self->_hash_min + 1;
			/// @append increase hash array;
			self->_hash_array = realloc(self->_hash_array, size * sizeof(List*));
			check(self->_hash_array, HeadAllocationException, "realloc(%lu)", size * sizeof(List*));
			memset(self->_hash_array + (old_max - self->_hash_min + 1), 0x0, (self->_hash_max - old_max - 1) * sizeof(List*));
		}
		/// @append endif
		/// @append if(hash<min) then (yes)
		if (hash < self->_hash_min) {
			size_t    old_size  = (self->_hash_max - self->_hash_min);
			uint64_t  old_min   = self->_hash_min;
			self->_hash_min     = hash;
			size_t size         = self->_hash_max - self->_hash_min;
			/// @append increase hash array;
			self->_hash_array   = realloc(self->_hash_array, size * sizeof(List*));
			check(self->_hash_array, HeadAllocationException, "realloc(%lu)", size * sizeof(List*));
			/// @append Move...;
			memmove(self->_hash_array + (old_min - self->_hash_min), self->_hash_array, old_size * sizeof(List*));
			memset(self->_hash_array, 0x0, (old_min - self->_hash_min) * sizeof(List*));
		}
		/// @append endif
		idx = hash - self->_hash_min;
		if (self->_hash_array == NULL) {
			self->_hash_array = calloc(1, sizeof(List*));
			check(self->_hash_array, HeadAllocationException, "malloc(%lu)", sizeof(List*));
		}
		if (self->_hash_array[idx] == NULL) {
			self->_hash_array[idx] = new_ArrayList(1, (self->_key_size + self->_value_size));
		}
		++self->_size;
	}
		/// @append endif

	uint8_t entry[self->_key_size + self->_value_size];
	memcpy(entry, key, self->_key_size);
	memcpy(entry + self->_key_size, value, self->_value_size);
	if (old_value == NULL) {
		List_add(self->_hash_array[idx], entry);
	} else {
		List_set(self->_hash_array[idx], idx_subarray, entry);
	}
	return old_value;
}
///! @append @enduml

void* HashMap_remove(HashMap* self, void* key) {
	uint64_t hash = isObject(key)? Object_hashCode((Object*)key): Hash_hashBuffer((char*)key, self->_key_size, self->_bits);
	int idx_subarray;
	int idx = hash - self->_hash_min;
	void *value = get_hashed(self, key, hash, &idx_subarray);
	if (value != NULL) {
		List_removeAt(self->_hash_array[idx], idx_subarray);
	}
	return value;
}

size_t HashMap_size(HashMap* self) {
	return self->_size;
}

Collection* HashMap_values(HashMap* self) {
	// TODO: Implement
	return NULL;
}

Set* HashMap_entrySet(HashMap* self) {
	// TODO: Implement
	return NULL;
}

Set* HashMap_keySet(HashMap* self) {
	// TODO: Implement
	return NULL;
}

void HashMapEntry_new(HashMapEntry *self) {
}
