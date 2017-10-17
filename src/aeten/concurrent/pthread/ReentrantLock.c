#include "ReentrantLock.h"

#include <pthread.h>

#define import
#include "concurrent/Condition.h"
#include "concurrent/pthread/ReentrantLockCondition.h"

/*
@startuml
!include concurrent/Lock.c
!include concurrent/Condition.c
!include concurrent/pthread/ReentrantLockCondition.c
namespace aeten.concurrent.pthread {
	class ReentrantLock implements aeten.concurrent.Lock {
		+ {static} ReentrantLock() <<constructor>>
		# void finalize() <<override>>
		- mutex: pthread_mutex_t
	}
}
@enduml
*/

void ReentrantLock_new(ReentrantLock* self) {
	pthread_mutex_t pthread_mutex = PTHREAD_MUTEX_INITIALIZER;
	self->_mutex = pthread_mutex;
}

void finalize(ReentrantLock* self) {
	pthread_mutex_destroy(&self->_mutex);
}

void lock(ReentrantLock* self) {
	pthread_mutex_lock(&self->_mutex);
}

bool tryLock(ReentrantLock* self) {
	pthread_mutex_trylock(&self->_mutex) == 0;
}

void unlock(ReentrantLock* self) {
	pthread_mutex_unlock(&self->_mutex);
}

Condition *newCondition(ReentrantLock* self) {
	return new_ReentrantLockCondition(&self->_mutex);
}

