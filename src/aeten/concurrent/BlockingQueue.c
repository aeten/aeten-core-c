#include "BlockingQueue.h"

/*
@startuml
!include aeten/Queue.c
namespace aeten.concurrent {
	interface BlockingQueue<T> extends aeten.Queue {
		+ void put(T* element)
		+ T* take()
	}
}
@enduml
*/
