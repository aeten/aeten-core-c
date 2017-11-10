#include "Condition.h"

/*
@startuml
!include Object.c
interface aeten.concurrent.Condition {
	+ await()
	+ awaitNanos(uint64_t nanosTimeout)
	+ awaitUntil(struct timespec deadline)
	+ signal()
	+ signalAll()
}
@enduml
*/
