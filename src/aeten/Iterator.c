#include "Iterator.h"

/*!
@startuml
!include Object.c
namespace aeten {
	interface Iterator<T> {
		+ bool hasNext()
		+ T* next()
		+ remove(T* element) <<default>>
	}
}
@enduml
*/

void Iterator_remove(Iterator *iterator, void *element) {
	check(0, UnsupportedOperationException, "Iterator.remove()");
}


