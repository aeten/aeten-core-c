#include "ReentrantLockCondition.h"

#include <pthread.h>

/*
@startuml
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

void _new(ReentrantLockCondition* self, pthread_mutex_t *mutex) {
	self->_mutex = mutex;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	self->_cond = cond;
}

void _await(ReentrantLockCondition* self) {
	pthread_cond_wait(&self->_cond, self->_mutex);
}

void _signal(ReentrantLockCondition* self) {
	pthread_cond_signal(&self->_cond);
}

void _signalAll(ReentrantLockCondition* self) {
	pthread_cond_broadcast(&self->_cond);
}

