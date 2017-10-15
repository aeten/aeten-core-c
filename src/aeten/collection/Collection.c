#include "Collection.h"

/*
@startuml
!include Object.c
namespace aeten.collection {
	interface Collection<T> {
		+ unsigned size()
	}
	note right of Collection::size
		Return the size of the collection.
	end note
}
@enduml
*/