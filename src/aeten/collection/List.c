#include "List.h"

/*
@startuml
!include Object.c
!include Collection.c
namespace aeten.collection {
	interface List<T> extends Collection {
		+ T* get(unsigned position)
		+ set(unsigned position, T* element)
		+ add(T* element)
	}
	note right of List::get
		Return the element at the mentionned position.
	end note
	note right of List::set
		Set the element at the given position.
	end note
	note right of List::add
		Add element to the list. It will be placed at the end.
	end note
}
@enduml
*/
