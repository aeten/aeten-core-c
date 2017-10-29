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
	class ArrayList<T> implements List {
		- capacity : size_t
		- size : size_t
		- element_size : size_t
		- elements : void*

		+ {static} ArrayList(size_t initial_capacity, size_t element_size) <<constructor>>
		+ bool add(T* element) <<override>>
		+ removeAt(size_t position) <<override>>
		+ set(size_t position, T* element) <<override>>
		# finalize() <<override>>
	}
	note right of ArrayList::ArrayList
		Constructor
	end note
}
@enduml
*/

void ArrayList_new(ArrayList *self, size_t initial_capacity, size_t element_size) {
	if (initial_capacity>0) {
		self->_elements = malloc(initial_capacity * element_size);
		check(self->_elements != NULL, NoSuchMemoryError, "initial_capacity=%lu", initial_capacity);
	} else {
		self->_elements = NULL;
	}
	self->_element_size = element_size;
	self->_capacity = initial_capacity;
	self->_size = 0;
}

void ArrayList_finalize(ArrayList *self) {
	if (self && self->_elements) {
		free(self->_elements);
		self->_elements = NULL;
	}
}

void ArrayList_set(ArrayList *self, size_t position, void *element) {
	check(position < self->_size, IndexOutOfBoundException, "position=%lu; array.length=%lu", position, self->_size);
	memcpy(((char*)self->_elements) + (position * self->_element_size), element, self->_element_size);
}

void ArrayList_removeAt(ArrayList *self, size_t position) {
	check(position < self->_size, IndexOutOfBoundException, "position=%lu; array.length=%lu", position, self->_size);
	void *dst = ((char*)self->_elements) + (position * self->_element_size);
	if (position < (self->_size-1)) {
		void *src = ((char*)self->_elements) + ((1 + position) * self->_element_size);
		size_t size = (self->_size - position) * self->_element_size;
		memmove(dst, src, size);
		memset(((char*)self->_elements) + ((self->_size - 2) * self->_element_size), 0x0, self->_element_size);
	} else {
		memset(dst, 0x0, self->_element_size);
	}
	--self->_size;
}


bool ArrayList_add(ArrayList *self, void *element) {
	if (self->_capacity == self->_size) {
		unsigned int capacity = ((self->_capacity * 3) / 2) + 1;
		self->_elements = realloc(self->_elements, capacity * self->_element_size);
		check(self->_elements != NULL, NoSuchMemoryError, "capacity=%u", capacity);
		self->_capacity = capacity;
	}
	memcpy(((char*)self->_elements) + (self->_size++ * self->_element_size), element, self->_element_size);
	return true;
}

void *ArrayList_get(ArrayList *self, size_t position) {
	check(position < self->_size, IndexOutOfBoundException, "position=%lu; array.length=%lu", position, self->_size);
	return (void*)(((char*)self->_elements) + (position * self->_element_size));
}

size_t ArrayList_size(ArrayList *self) {
	return self->_size;
}

Iterator ArrayList_iterator(ArrayList *self) {
	// TODO
	check(0, NotImplementedOperationException, "ArrayList.iterator()");
	Iterator iterator;
	return iterator;
}

