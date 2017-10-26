#include "Queue.h"

/*
@startuml
!include Collection.c
namespace aeten.collection {
	interface Queue<T> extends Collection {
		void add(T* element) <<default>> <<override>>
		bool offer(T* element)
		T* poll()
		T* peek()
	}
}
@enduml
*/

void _add(Queue* queue, void* element) {
	Queue_offer(queue, element);
}

