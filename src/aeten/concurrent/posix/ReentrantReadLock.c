#include <pthread.h>

#include "ReentrantReadLock.h"

#define import
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/Lock.h"

/*
@startuml
!include concurrent/Lock.c
namespace aeten.concurrent.posix {
	class ReentrantReadLock implements aeten.concurrent.Lock {
		+ {static} ReentrantReadLock(pthread_rwlock_t* rw_lock) <<constructor>>
		- rw_lock: pthread_rwlock_t*
	}
}
@enduml
*/

void _new(ReentrantReadLock* self, pthread_rwlock_t *rw_lock) {
	self->_rw_lock = rw_lock;
}

void _lock(ReentrantReadLock* self) {
	check(pthread_rwlock_rdlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_rdlock");
}

bool _tryLock(ReentrantReadLock* self) {
	return (0 == pthread_rwlock_tryrdlock(self->_rw_lock));
}

void _unlock(ReentrantReadLock* self) {
	check(pthread_rwlock_unlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_unlock");
}

Condition* _newCondition(ReentrantReadLock* self) {
	check(0, UnsupportedOperationException, "ReentrantReadLock.newCondition()");
	return NULL;
}

