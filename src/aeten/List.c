#include "List.h"

/*
@startuml
!include Object.c
!include Collection.c
namespace aeten {
	interface List<T> extends Collection {
		+ T* get(size_t position)
		+ removeAt(size_t position) <<default>> <<optional>>
		+ set(size_t position, T* element) <<default>> <<optional>>
	}
	note right of List::get
		Return the element at the mentioned position.
	end note
	note right of List::set
		Set the element at the given position.
	end note
}
@enduml
*/

void _set(List* list, size_t position, void *element) {
	check(0, UnsupportedOperationException, "Last.set()");
}

void _removeAt(List* list, size_t position) {
	check(0, UnsupportedOperationException, "List.removeAt()");
}
