#include "ReadWriteLock.h"

/*
@startuml
!include Object.c
!include Lock.c
interface aeten.concurrent.ReadWriteLock {
	+ Lock* readLock()
	+ Lock* writeLock()
}
@enduml
*/
