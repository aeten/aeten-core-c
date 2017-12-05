#include "ReentrantLock.h"

#include <pthread.h>
#include <errno.h>

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

static struct timespec time_diff(struct timespec start, struct timespec end);


void ReentrantLock_new(ReentrantLock* self) {
	pthread_mutex_t pthread_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutexattr_t mutex_init;
	bzero(&mutex_init, sizeof(pthread_mutexattr_t));
	pthread_mutexattr_init(&mutex_init);
	pthread_mutexattr_settype(&mutex_init, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&pthread_mutex, &mutex_init);
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

uint64_t ReentrantLockCondition_awaitNanos(ReentrantLockCondition* self, uint64_t nanosTimeout) {
	struct timespec from;
	check(clock_gettime(CLOCK_REALTIME, &from) == 0, Error, "%m (errno: %u)", errno);
	struct timespec end = {
		from.tv_sec,
		from.tv_nsec + nanosTimeout
	};
	while (end.tv_nsec >= 1000000000) {
		end.tv_nsec -= 1000000000;
		++end.tv_sec;
	}

	if (ReentrantLockCondition_awaitUntil(self, end)) {
		struct timespec now, rest;
		check(clock_gettime(CLOCK_REALTIME, &now) == 0, Error, "%m (errno: %u)", errno);
		rest = time_diff(from, now);
		return (rest.tv_sec * 1E9) + rest.tv_nsec;
	}
	return 0;
}

bool ReentrantLockCondition_awaitUntil(ReentrantLockCondition* self, struct timespec deadline) {
	switch (pthread_cond_timedwait(&self->_cond, self->_mutex, &deadline)) {
		case ETIMEDOUT:
			return false;
		case EINTR:
			throw(InterruptException);
			return true;
		default:
			return true;
	}
}

void ReentrantLockCondition_signal(ReentrantLockCondition* self) {
	pthread_cond_signal(&self->_cond);
}

void ReentrantLockCondition_signalAll(ReentrantLockCondition* self) {
	pthread_cond_broadcast(&self->_cond);
}

struct timespec time_diff(struct timespec start, struct timespec end) {
	struct timespec diff;
	diff.tv_sec = end.tv_sec - start.tv_sec;
	diff.tv_nsec = end.tv_nsec - start.tv_nsec;
	while (diff.tv_nsec < 0 ) {
		diff.tv_nsec += 1E9;
		--diff.tv_sec;
	}
	return diff;
}

