#include "Iterator.h"

/*!
@startuml
!include Object.c
namespace aeten {
	interface Iterator<T> {
		+bool has_next()
		+T* next()
		+set(T* element)
		+add(T* element)
		+remove(T* element)
	}
}
@enduml
*/

