#include "HashSet.h"

#define import
#include "Object.h"
#include "Iterator.h"
#include "Map.h"
#include "Set.h"
#include "HashMap.h"

/*!
@startuml
!include Object.c
!include Set.c
!include Map.c
!include HashMap.c
namespace aeten {
    class HashSet<T extends Object> implements Set {
		+ {static} HashSet() <<constructor>>
		+ bool add(T element) <<override>>
		+ bool remove(T element) <<override>>
		+ finalize() <<override>>

		- map: HashMap
		- key_set: Set
	}
}
@enduml
*/

void HashSet_new(HashSet *self) {
	init_HashMap(&self->_map);
	self->_key_set = Map_keySet(Map_cast(&self->_map));
}

void HashSet_finalize(HashSet *self) {
	Set_delete(self->_key_set);
	Map_delete(Map_cast(&self->_map));
}

bool HashSet_contains(HashSet* self, Object element) {
	return Set_contains(self->_key_set, element);
}

bool HashSet_add(HashSet *self, Object element) {
	return Set_add(self->_key_set, element);
}

size_t HashSet_size(HashSet* self) {
	return Set_size(self->_key_set);
}

bool HashSet_remove(HashSet *self, Object element) {
	return Set_remove(self->_key_set, element);
}

Iterator HashSet_iterator(HashSet* self) {
	return Set_iterator(self->_key_set);
}

