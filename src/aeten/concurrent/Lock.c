#include "Lock.h"

/*
@startuml
!include Object.c
!include Condition.c
interface aeten.concurrent.Lock {
	void lock()
	Condition* newCondition()
	bool tryLock()
	void unlock()
}
@enduml
*/

