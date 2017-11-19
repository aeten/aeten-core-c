#include "HashSet.h"

#define import
#include "Iterator.h"
#include "Map.h"
#include "Set.h"
#include "HashMap.h"

/*
@startuml
!include Object.c
!include Set.c
!include Map.c
namespace aeten {
	class HashSet<T> implements Set {
		+ {static} HashSet(size_t element_size) <<constructor>>
		+ bool add(T* element) <<override>>
		+ bool remove(T* element) <<override>>
		+ finalize() <<override>>

		- map: Map*
		- key_set: Set*
	}
}
@enduml
*/

void HashSet_new(HashSet *self, size_t element_size) {
	self->_map = new_HashMap(element_size, 0);
	self->_key_set = Map_keySet(self->_map);
}

void HashSet_finalize(HashSet *self) {
	Set_delete(self->_key_set);
	Map_delete(self->_map);
}

bool HashSet_contains(HashSet* self, void *element) {
	return Set_contains(self->_key_set, element);
}

bool HashSet_add(HashSet *self, void *element) {
	return Set_add(self->_key_set, element);
}

size_t HashSet_size(HashSet* self) {
	return Set_size(self->_key_set);
}

bool HashSet_remove(HashSet *self, void *element) {
	return Set_remove(self->_key_set, element);
}

Iterator *HashSet_iterator(HashSet* self) {
	return Set_iterator(self->_key_set);
}

