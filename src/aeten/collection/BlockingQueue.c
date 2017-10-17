#include "BlockingQueue.h"

/*
@startuml
!include Queue.c
namespace aeten.collection {
	interface BlockingQueue<T> extends Queue {
		void put(T element)
		T take()
	}
}
@enduml
*/
