#include <pthread.h>

#include "ReentrantWriteLock.h"

#define import
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/Lock.h"

/*
@startuml
!include concurrent/Lock.c
namespace aeten.concurrent.pthread {
	class ReentrantWriteLock implements aeten.concurrent.Lock {
		+ {static} ReentrantWriteLock(pthread_rwlock_t* rw_lock) <<constructor>>
		- rw_lock: pthread_rwlock_t*
	}
}
@enduml
*/

void ReentrantWriteLock_new(ReentrantWriteLock* self, pthread_rwlock_t* rw_lock) {
	self->_rw_lock = rw_lock;
}

void lock(ReentrantWriteLock* self) {
	check(pthread_rwlock_wrlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_wrlock");
}

bool tryLock(ReentrantWriteLock* self) {
	return (0 == pthread_rwlock_trywrlock(self->_rw_lock));
}

void unlock(ReentrantWriteLock* self) {
	check(pthread_rwlock_unlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_unlock");
}

Condition* newCondition(ReentrantWriteLock* self) {
	check(0, UnsupportedOperationException, "ReentrantWriteLock.newCondition()");
	return NULL;
}

