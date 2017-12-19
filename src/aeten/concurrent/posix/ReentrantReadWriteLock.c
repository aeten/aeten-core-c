#include "ReentrantReadWriteLock.h"
#include "ReentrantReadLock.h"
#include "ReentrantWriteLock.h"

#include <pthread.h>

#define import
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/Lock.h"
#include "ReentrantReadLock.h"
#include "ReentrantWriteLock.h"

/*!
@startuml ReentrantReadWriteLock
!include Object.c
!include aeten/concurrent/Condition.c
!include aeten/concurrent/Lock.c
!include aeten/concurrent/ReadWriteLock.c
!include ReentrantReadWriteLock.c!ReentrantReadLock
!include ReentrantReadWriteLock.c!ReentrantWriteLock
namespace aeten.concurrent.posix {
	class ReentrantReadWriteLock implements aeten.concurrent.ReadWriteLock {
		+ {static} ReentrantReadWriteLock() <<constructor>>
		# finalize() <<override>>
		- rw_lock: pthread_rwlock_t
        - read_lock: ReentrantReadLock
        - write_lock: ReentrantWriteLock
	}
}
@enduml

@startuml(id=ReentrantReadLock) ReentrantReadLock
!include concurrent/Lock.c
namespace aeten.concurrent.posix {
	class ReentrantReadLock implements aeten.concurrent.Lock {
		+ {static} ReentrantReadLock(pthread_rwlock_t* rw_lock) <<constructor>>
		- rw_lock: pthread_rwlock_t*
	}
}
@enduml

@startuml(id=ReentrantWriteLock) ReentrantWriteLock
!include concurrent/Lock.c
namespace aeten.concurrent.posix {
	class ReentrantWriteLock implements aeten.concurrent.Lock {
		+ {static} ReentrantWriteLock(pthread_rwlock_t* rw_lock) <<constructor>>
		- rw_lock: pthread_rwlock_t*
	}
}
@enduml
*/


void ReentrantReadWriteLock_new(ReentrantReadWriteLock* self) {
	pthread_rwlock_t pthread_rwlock = PTHREAD_RWLOCK_INITIALIZER;
	self->_rw_lock = pthread_rwlock;
	init_ReentrantReadLock(&self->_read_lock, &self->_rw_lock);
	init_ReentrantWriteLock(&self->_write_lock, &self->_rw_lock);
}

void ReentrantReadWriteLock_finalize(ReentrantReadWriteLock* self) {
	Lock_delete(Lock_cast(&self->_read_lock));
	Lock_delete(Lock_cast(&self->_write_lock));
	pthread_rwlock_destroy(&self->_rw_lock);
}

Lock ReentrantReadWriteLock_readLock(ReentrantReadWriteLock* self) {
    return Lock_cast(&self->_read_lock);
}

Lock ReentrantReadWriteLock_writeLock(ReentrantReadWriteLock* self) {
    return Lock_cast(&self->_write_lock);
}

void ReentrantReadLock_new(ReentrantReadLock* self, pthread_rwlock_t *rw_lock) {
	self->_rw_lock = rw_lock;
}

void ReentrantReadLock_lock(ReentrantReadLock* self) {
	check(pthread_rwlock_rdlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_rdlock");
}

bool ReentrantReadLock_tryLock(ReentrantReadLock* self) {
	return (0 == pthread_rwlock_tryrdlock(self->_rw_lock));
}

void ReentrantReadLock_unlock(ReentrantReadLock* self) {
	check(pthread_rwlock_unlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_unlock");
}

Condition ReentrantReadLock_newCondition(ReentrantReadLock* self) {
	check(0, UnsupportedOperationException, "ReentrantReadLock.newCondition()");
	return (Condition){0};
}

void ReentrantWriteLock_new(ReentrantWriteLock* self, pthread_rwlock_t* rw_lock) {
	self->_rw_lock = rw_lock;
}

void ReentrantWriteLock_lock(ReentrantWriteLock* self) {
	check(pthread_rwlock_wrlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_wrlock");
}

bool ReentrantWriteLock_tryLock(ReentrantWriteLock* self) {
	return (0 == pthread_rwlock_trywrlock(self->_rw_lock));
}

void ReentrantWriteLock_unlock(ReentrantWriteLock* self) {
	check(pthread_rwlock_unlock(self->_rw_lock) == 0, RuntimeError, "pthread_rwlock_unlock");
}

Condition ReentrantWriteLock_newCondition(ReentrantWriteLock* self) {
	check(0, UnsupportedOperationException, "ReentrantWriteLock.newCondition()");
	return (Condition){0};
}

