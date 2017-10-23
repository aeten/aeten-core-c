#include "HashMap.h"

#define import
#include "Iterator.h"
#include "MapEntry.h"
#include "Collection.h"
#include "Set.h"

/*
@startuml
!include Object.c
!include Map.c
!include Set.c
!include MapEntry.c
namespace aeten {
	class HashMap<K,V> implements Map {
		- size : size_t

		{static} + HashMap() <<constructor>>
		# finalize() <<override>>
	}
}
@enduml
*/


void _new(HashMap* self) {
	// TODO: Implement
}

void _finalize(HashMap* self) {
	// TODO: Implement
}

void* _get(HashMap* self, void* key) {
	// TODO: Implement
}

void* _put(HashMap* self, void* key, void* value) {
	// TODO: Implement
}

void* _remove(HashMap* self, void* key) {
	// TODO: Implement
}

size_t _size(HashMap* self) {
	// TODO: Implement
}

Collection* _values(HashMap* self) {
	// TODO: Implement
}

Set _entrySet(HashMap* self) {
	// TODO: Implement
}

Set _keySet(HashMap* self) {
	// TODO: Implement
}


