#include "Condition.h"

/*
@startuml
interface aeten.concurrent.Condition {
	+ await()
	+ uint64_t awaitNanos(uint64_t nanosTimeout)
	+ bool awaitUntil(struct timespec deadline)
	+ signal()
	+ signalAll()
}
@enduml
*/
