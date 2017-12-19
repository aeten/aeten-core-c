#include "lang.h"

#include "ArrayList.h"

#define import
#include "Object.h"
#include "Iterator.h"

/*
@startuml
!include Object.c
!include List.c
namespace aeten {
	class ArrayList<T extends Object> implements List {
		- capacity : size_t
		- size : size_t
		- elements : T[]

		+ {static} ArrayList(size_t initial_capacity) <<constructor>>
		+ bool add(T element) <<override>>
		+ removeAt(size_t position) <<override>>
		+ set(size_t position, T element) <<override>>
		# finalize() <<override>>
	}
	note right of ArrayList::ArrayList
		Constructor
	end note
}
@enduml
*/

void ArrayList_new(ArrayList *self, size_t initial_capacity) {
	if (initial_capacity>0) {
		self->_elements = malloc(initial_capacity);
		check(self->_elements != NULL, NoSuchMemoryError, "initial_capacity=%zu", initial_capacity);
	} else {
		self->_elements = NULL;
	}
	self->_capacity = initial_capacity;
	self->_size = 0;
}

void ArrayList_finalize(ArrayList *self) {
	if (self && self->_elements) {
		free(self->_elements);
		self->_elements = NULL;
	}
}

void ArrayList_set(ArrayList *self, size_t position, Object element) {
	check(position < self->_size, IndexOutOfBoundException, "position=%zu; array.length=%zu", position, self->_size);
	self->_elements[position] = element;
}

void ArrayList_removeAt(ArrayList *self, size_t position) {
	check(position < self->_size, IndexOutOfBoundException, "position=%zu; array.length=%zu", position, self->_size);
	void *dst = ((char*)self->_elements) + (position * sizeof(Object));
	if (position < (self->_size-1)) {
		void *src = ((uint8_t*)self->_elements) + ((1 + position) * sizeof(Object));
		size_t size = (self->_size - position) * sizeof(Object);
		memmove(dst, src, size);
		memset(((uint8_t*)self->_elements) + ((self->_size - 2) * sizeof(Object)), 0x0, sizeof(Object));
	} else {
		memset(dst, 0x0, sizeof(Object));
	}
	--self->_size;
}


bool ArrayList_add(ArrayList *self, Object element) {
	if (self->_capacity == self->_size) {
		unsigned int capacity = ((self->_capacity * 3) / 2) + 1;
		self->_elements = realloc(self->_elements, capacity * sizeof(Object));
		check(self->_elements != NULL, NoSuchMemoryError, "capacity=%u", capacity);
		self->_capacity = capacity;
	}
	self->_elements[self->_size++] = element;
	return true;
}

Object ArrayList_get(ArrayList *self, size_t position) {
	check(position < self->_size, IndexOutOfBoundException, "position=%zu; array.length=%zu", position, self->_size);
	return self->_elements[position];
}

size_t ArrayList_size(ArrayList *self) {
	return self->_size;
}

Iterator ArrayList_iterator(ArrayList *self) {
	// TODO
	check(0, NotImplementedOperationException, "ArrayList.iterator()");
	return (Iterator){0};
}

