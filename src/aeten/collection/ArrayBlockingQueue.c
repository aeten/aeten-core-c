#include "ArrayBlockingQueue.h"

#define import
#include "concurrent/Lock.h"
#include "concurrent/pthread/ReentrantLock.h"
#include "concurrent/Condition.h"
#include "Object.h"
#include "List.h"
#include "ArrayList.h"

/*
@startuml
!include Object.c
!include Collection.c
!include ArrayList.c
!include List.c
!include concurrent/Condition.c
!include concurrent/Lock.c
!include concurrent/pthread/ReentrantLock.c
!include Queue.c
!include BlockingQueue.c
namespace aeten.collection {
	class ArrayBlockingQueue implements BlockingQueue {
		+ {static} ArrayBlockingQueue(unsigned capacity)
		- lock: Lock*
		- full: Condition*
		- empty: Condition*
		- not_full: Condition*
		- not_empty: Condition*
		- capacity: size_t
		- size: size_t
		- array: List*
		- head: unsigned
		- tail: unsigned
	}
}
@enduml
*/

void ArrayBlockingQueue_new(ArrayBlockingQueue* self, unsigned capacity) {
	self->_lock = new_ReentrantLock();
	self->_full = self->_lock->newCondition(self->_lock);
	self->_empty = self->_lock->newCondition(self->_lock);
	self->_not_full = self->_lock->newCondition(self->_lock);
	self->_not_empty = self->_lock->newCondition(self->_lock);
	self->_array = new_ArrayList(capacity);
}

static void finalize(ArrayBlockingQueue* self) {
	self->_lock->delete(self->_lock);
	self->_full->delete(self->_full);
	self->_empty->delete(self->_empty);
	self->_not_full->delete(self->_not_full);
	self->_not_empty->delete(self->_not_empty);
	self->_array->delete(self->_array);
}

static unsigned size(ArrayBlockingQueue* self) {
	unsigned size;
	self->_lock->lock(self->_lock);
	size = self->_size;
	self->_lock->unlock(self->_lock);
	return size;
}


static bool _queue_push(ArrayBlockingQueue* self, void *element, bool try_) {
	self->_lock->lock(self->_lock);
	int backup = self->_head++;
	if (self->_head == self->_capacity) {
		self->_head=0;
	}
	while (self->_size == self->_capacity) {
		if (try_)  {
			self->_head = backup;
			self->_lock->unlock(self->_lock);
			return false;
		}
		self->_not_full->await(self->_not_full);
	}
	self->_array->set(self->_array, self->_head, element);

	self->_size++;

	self->_not_empty->signalAll(self->_not_empty);
	self->_lock->unlock(self->_lock);
	return true;
}

static void *_queue_pop(ArrayBlockingQueue* self, bool try_) {
	void *element = NULL;
	self->_lock->lock(self->_lock);
	int backup = self->_tail++;
	if (self->_tail == self->_capacity) {
		self->_tail=0;
	}
	while (self->_size == 0) {
		if (try_) {
			self->_tail = backup;
			self->_lock->unlock(self->_lock);
			return NULL;
		}
		self->_not_empty->await(self->_not_empty);
	}
	element = self->_array->get(self->_array, self->_tail);

	self->_size--;

	self->_not_empty->signalAll(self->_not_full);
	self->_lock->unlock(self->_lock);
	return element;
}

static bool offer(ArrayBlockingQueue* self, void *element) {
	return _queue_push(self, element, true);
}

static void *poll(ArrayBlockingQueue* self) {
	return _queue_pop(self, true);
}

static void *peek(ArrayBlockingQueue* self) {
	void *element = NULL;
	self->_lock->lock(self->_lock);
	if (self->_size != 0) {
		element = self->_array->get(self->_array, self->_tail);
	}
	self->_lock->unlock(self->_lock);
	return element;
}

static void put(ArrayBlockingQueue* self, void *element) {
	_queue_push(self, element, false);
}

static void *take(ArrayBlockingQueue* self) {
	return _queue_pop(self, false);
}

