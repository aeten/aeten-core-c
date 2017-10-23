#include "Collection.h"

/*
@startuml
!include Iterable.c
namespace aeten {
	interface Collection<T> extends Iterable {
		add(T* element)
		size_t size()
	}
	note right of Collection::add
		Ensures that this collection contains the specified element (optional operation).
	end note
	note right of Collection::size
		Return the size of the 
	end note
}
@enduml
*/
