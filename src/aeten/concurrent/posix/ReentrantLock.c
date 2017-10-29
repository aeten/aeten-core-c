#include "ReentrantLock.h"

#include <pthread.h>

#define import
#include "concurrent/Condition.h"
#include "ReentrantLockCondition.h"

/*
@startuml ReentrantLock
!include concurrent/Lock.c
!include concurrent/Condition.c
namespace aeten.concurrent.posix {
	class ReentrantLock implements aeten.concurrent.Lock {
		+ {static} ReentrantLock() <<constructor>>
		# void finalize() <<override>>
		- mutex: pthread_mutex_t
	}
}
@enduml

@startuml ReentrantLockCondition
!include Object.c
!include concurrent/Condition.c
namespace aeten.concurrent.posix {
	class ReentrantLockCondition implements aeten.concurrent.Condition {
		+ {static} ReentrantLockCondition(pthread_mutex_t *mutex) <<constructor>>
		- mutex: pthread_mutex_t*
		- cond: pthread_cond_t
	}
}
@enduml
*/

void ReentrantLock_new(ReentrantLock* self) {
	pthread_mutex_t pthread_mutex = PTHREAD_MUTEX_INITIALIZER;
	self->_mutex = pthread_mutex;
}

void ReentrantLock_finalize(ReentrantLock* self) {
	pthread_mutex_destroy(&self->_mutex);
}

void ReentrantLock_lock(ReentrantLock* self) {
	pthread_mutex_lock(&self->_mutex);
}

bool ReentrantLock_tryLock(ReentrantLock* self) {
	return (pthread_mutex_trylock(&self->_mutex) == 0);
}

void ReentrantLock_unlock(ReentrantLock* self) {
	pthread_mutex_unlock(&self->_mutex);
}

Condition *ReentrantLock_newCondition(ReentrantLock* self) {
	return new_ReentrantLockCondition(&self->_mutex);
}

void ReentrantLockCondition_new(ReentrantLockCondition* self, pthread_mutex_t *mutex) {
	self->_mutex = mutex;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	self->_cond = cond;
}

void ReentrantLockCondition_await(ReentrantLockCondition* self) {
	pthread_cond_wait(&self->_cond, self->_mutex);
}

void ReentrantLockCondition_signal(ReentrantLockCondition* self) {
	pthread_cond_signal(&self->_cond);
}

void ReentrantLockCondition_signalAll(ReentrantLockCondition* self) {
	pthread_cond_broadcast(&self->_cond);
}

