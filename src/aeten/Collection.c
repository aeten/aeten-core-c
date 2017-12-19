#include "Collection.h"

#define import
#include "Object.h"

/*
@startuml
!include Iterable.c
namespace aeten {
	interface Collection<T extends Object> extends Iterable {
		+ bool add(T element) <<default>> <<optional>>
		+ bool remove(T element) <<default>> <<optional>>
		+ size_t size()
	}
	note right of Collection::add
		Ensures that this collection contains the specified element (optional operation).
	end note
	note right of Collection::remove
		Removes a single instance of the specified element from this collection, if it is present (optional operation)
	end note
	note right of Collection::size
		Return the size of the 
	end note
}
@enduml
*/

bool Collection_add(Collection *collection, Object element) {
	check(0, UnsupportedOperationException, "Collection.add(Object element)");
	return false;
}

bool Collection_remove(Collection *collection, Object element) {
	check(0, UnsupportedOperationException, "Collection.remove(Object element)");
	return false;
}
