#include "Set.h"

/*
@startuml
!include Object.c
namespace aeten.collection {
	interface Set<T> extends Collection {
		+ bool contains(T* element)
		+ bool add(T* element) <<default>> <<optional>>
	}
	note right of Set::contains
		Returns true if this set contains the specified element.
	end note
	note right of Set::add
		Adds the specified element to this set if it is not already present (optional operation).
	end note
}
@enduml
*/

bool add(Set* self, void *element) {
	check(0, UnsupportedOperationException, "Set.add()");
	return false;
}
