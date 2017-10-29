#include "Queue.h"

/*
@startuml
!include Collection.c
namespace aeten {
	interface Queue<T> extends Collection {
		bool add(T* element) <<default>> <<override>>
		bool offer(T* element)
		T* poll()
		T* peek()
	}
}
@enduml
*/

bool Queue_add(Queue* queue, void* element) {
	return aeten_Queue_offer(queue, element);
}

