#include "Iterator.h"

/*!
@startuml
!include Object.c
namespace aeten {
	interface Iterator<T extends Object> {
		+ bool hasNext()
		+ T next()
		+ remove(T element) <<default>>
	}
}
@enduml
*/

void Iterator_remove(Iterator *iterator, aeten_Object element) {
	check(0, UnsupportedOperationException, "Iterator.remove()");
}


