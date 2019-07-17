#include "Set.h"

#define import
#include "Object.h"

/*
@startuml
!include Object.c
!include Collection.c
namespace aeten {
    interface Set<T extends Object> extends Collection {
        + bool contains(T element)
        + bool add(T element) <<override>> <<default>> <<optional>>
	}
!ifdef aeten_Set
	note right of Set::contains
		Returns true if this set contains the specified element.
	end note
	note right of Set::add
		Adds the specified element to this set if it is not already present (optional operation).
	end note
!endif
}
@enduml
*/

bool Set_add(Set* self, Object element) {
	check(0, UnsupportedOperationException, "Set.add()");
	return false;
}
