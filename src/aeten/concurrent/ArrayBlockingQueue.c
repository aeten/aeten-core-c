#include "ArrayBlockingQueue.h"

#define import
#include "concurrent/Lock.h"
#include "concurrent/Condition.h"
#include "concurrent/posix/ReentrantLock.h"
#include "Object.h"
#include "Iterator.h"

/*
@startuml
!include Object.c
!include Collection.c
!include Queue.c
!include concurrent/Condition.c
!include concurrent/Lock.c
!include concurrent/BlockingQueue.c
!include concurrent/posix/ReentrantLock.c
namespace aeten.concurrent {
	class ArrayBlockingQueue<T extends Object> implements BlockingQueue {
		+ {static} ArrayBlockingQueue(unsigned capacity)
		# void finalize() <<override>>
		- lock: Lock
		- not_full: Condition
		- not_empty: Condition
		- capacity: size_t
		- size: size_t
		- array: T[]
		- head: int
		- tail: int
	}
}
@enduml
*/

inline static bool _queue_push(ArrayBlockingQueue* self, Object element, bool try_);
inline static Object _queue_pop(ArrayBlockingQueue* self, bool try_);

void ArrayBlockingQueue_new(ArrayBlockingQueue* self, unsigned capacity) {
	self->_array = malloc(capacity * sizeof(Object));
	check(self->_array, HeapAllocationException, "Array(capacity=%u * size=%zu)", capacity, sizeof(void*));
	self->_lock = Lock_cast(new_ReentrantLock());
	self->_not_full = Lock_newCondition(self->_lock);
	self->_not_empty = Lock_newCondition(self->_lock);
	self->_capacity = capacity;
	self->_size = 0;
	self->_head = -1;
	self->_tail = -1;
}

void ArrayBlockingQueue_finalize(ArrayBlockingQueue* self) {
	Lock_delete(self->_lock);
	Condition_delete(self->_not_full);
	Condition_delete(self->_not_empty);
	free(self->_array);
}

size_t ArrayBlockingQueue_size(ArrayBlockingQueue* self) {
	size_t size;
	Lock_lock(self->_lock);
	size = self->_size;
	Lock_unlock(self->_lock);
	return size;
}


bool ArrayBlockingQueue_offer(ArrayBlockingQueue* self, Object element) {
	return _queue_push(self, element, true);
}

Object ArrayBlockingQueue_poll(ArrayBlockingQueue* self) {
	return _queue_pop(self, true);
}

Object ArrayBlockingQueue_peek(ArrayBlockingQueue* self) {
	Object element;
	Lock_lock(self->_lock);
	if (self->_size != 0) {
		element = self->_array[self->_tail];
	}
	Lock_unlock(self->_lock);
	return element;
}

void ArrayBlockingQueue_put(ArrayBlockingQueue* self, Object element) {
	_queue_push(self, element, false);
}

Object ArrayBlockingQueue_take(ArrayBlockingQueue* self) {
	return _queue_pop(self, false);
}

Iterator ArrayBlockingQueue_iterator(ArrayBlockingQueue *self) {
	// TODO
	check(0, NotImplementedOperationException, "ArrayBlockingQueue.iterator()");
	return (Iterator){NULL,NULL};
}

bool _queue_push(ArrayBlockingQueue* self, Object element, bool try_) {
	Lock_lock(self->_lock);
	int backup = self->_head++;
	if (self->_head == self->_capacity) {
		self->_head=0;
	}
	while (self->_size == self->_capacity) {
		if (try_)  {
			self->_head = backup;
			Lock_unlock(self->_lock);
			return false;
		}
		Condition_await(self->_not_full);
	}
	self->_array[self->_head] = element;

	self->_size++;

	Condition_signalAll(self->_not_empty);
	Lock_unlock(self->_lock);
	return true;
}

Object _queue_pop(ArrayBlockingQueue* self, bool try_) {
	Object element = aeten_Object_class.null;
	Lock_lock(self->_lock);
	int backup = self->_tail++;
	if (self->_tail == self->_capacity) {
		self->_tail=0;
	}
	while (self->_size == 0) {
		if (try_) {
			self->_tail = backup;
			Lock_unlock(self->_lock);
			return element; // null
		}
		Condition_await(self->_not_empty);
	}
	element = self->_array[self->_tail];

	self->_size--;

	Condition_signalAll(self->_not_full);
	Lock_unlock(self->_lock);
	return element;
}


