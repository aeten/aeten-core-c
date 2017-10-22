#include "List.h"

/*
@startuml
!include Object.c
!include Collection.c
namespace aeten.collection {
	interface List<T> extends Collection {
		+ T* get(size_t position)
		+ set(size_t position, T* element)
	}
	note right of List::get
		Return the element at the mentionned position.
	end note
	note right of List::set
		Set the element at the given position.
	end note
}
@enduml
*/
