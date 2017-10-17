#include "ReentrantWriteLock.h"

#include <pthread.h>

#define import
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/Lock.h"

/*
@startuml
!include concurrent/Lock.c
namespace aeten.concurrent.pthread {
	class ReentrantWriteLock implements aeten.concurrent.Lock {
		+ {static} ReentrantWriteLock(pthread_rwlock_t* rw_lock) <<constructor>>
		- rw_lock: pthread_rwlock_t
	}
}
@enduml
*/

void new_ReentrantWriteLock(ReentrantWriteLock* self, pthread_rwlock_t* rw_lock) {
	self->_rw_lock = pthread_rwlock;
}

void lock(ReentrantWriteLock* self) {
	check(pthread_rwlock_wrlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_wrlock");
}

bool tryLock(ReentrantLock* self) {
	pthread_mutex_trywrlock(&self->_rw_lock) == 0;
}

Lock* unlock(ReentrantWriteLock* self) {
	check(pthread_rwlock_unlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_unlock");
}

Condition newCondition(ReentrantWriteLock* self) {
	check(0, UnsupportedOperationException, "ReentrantWriteLock.newCondition()");
}

