#include <pthread.h>

#include "ReentrantReadLock.h"

#define import
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/Lock.h"

/*
@startuml
!include concurrent/Lock.c
namespace aeten.concurrent.pthread {
	class ReentrantReadLock implements aeten.concurrent.Lock {
		+ {static} ReentrantReadLock(pthread_rwlock_t* rw_lock) <<constructor>>
		- rw_lock: pthread_rwlock_t*
	}
}
@enduml
*/

void ReentrantReadLock_new(ReentrantReadLock* self, pthread_rwlock_t *rw_lock) {
	self->_rw_lock = rw_lock;
}

void lock(ReentrantReadLock* self) {
	check(pthread_rwlock_rdlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_rdlock");
}

bool tryLock(ReentrantReadLock* self) {
	return (0 == pthread_rwlock_tryrdlock(self->_rw_lock));
}

void unlock(ReentrantReadLock* self) {
	check(pthread_rwlock_unlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_unlock");
}

Condition* newCondition(ReentrantReadLock* self) {
	check(0, UnsupportedOperationException, "ReentrantReadLock.newCondition()");
	return NULL;
}

