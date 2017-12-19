#include "Queue.h"

#define import
#include "Object.h"

/*
@startuml
!include Collection.c
namespace aeten {
    interface Queue<T extends Object> extends Collection {
        + bool add(T element) <<default>> <<override>>
        + bool offer(T element)
        + T poll()
        + T peek()
	}
}
@enduml
*/

bool Queue_add(Queue *queue, Object element) {
	return aeten_Queue_offer(*queue, element);
}

