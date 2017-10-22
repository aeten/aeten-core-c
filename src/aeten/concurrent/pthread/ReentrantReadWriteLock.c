#include "ReentrantReadWriteLock.h"

#include <pthread.h>

#define import
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/Lock.h"
#include "ReentrantReadLock.h"
#include "ReentrantWriteLock.h"

/*
@startuml ReentrantReadWriteLock
!include Object.c
!include aeten/concurrent/Condition.c
!include aeten/concurrent/Lock.c
!include aeten/concurrent/ReadWriteLock.c
!include ReentrantReadLock.c
!include ReentrantWriteLock.c
namespace aeten.concurrent.pthread {
	class ReentrantReadWriteLock implements aeten.concurrent.ReadWriteLock {
		+ {static} ReentrantReadWriteLock() <<constructor>>
		- rw_lock: pthread_rwlock_t
		- read_lock: Lock*
		- write_lock: Lock*
	}
}
@enduml
*/

void ReentrantReadWriteLock_new(ReentrantReadWriteLock* self) {
	pthread_rwlock_t pthread_rwlock = PTHREAD_RWLOCK_INITIALIZER;
	self->_rw_lock = pthread_rwlock;
	self->_read_lock = new_ReentrantReadLock(&self->_rw_lock);
	self->_write_lock = new_ReentrantWriteLock(&self->_rw_lock);
}

void finalize(ReentrantReadWriteLock* self) {
	Lock_delete(self->_read_lock);
	Lock_delete(self->_write_lock);
	pthread_rwlock_destroy(&self->_rw_lock);
}

Lock* readLock(ReentrantReadWriteLock* self) {
	return self->_read_lock;
}

Lock* writeLock(ReentrantReadWriteLock* self) {
	return self->_write_lock;
}

