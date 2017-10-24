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

		{static} + ArrayList(size_t initial_capacity, size_t element_size) <<constructor>>
		# finalize() <<override>>
	}
	note right of ArrayList::ArrayList
		Constructor
	end note
}
@enduml
*/

void _new(ArrayList *self, size_t initial_capacity, size_t element_size) {
	if (initial_capacity>0) {
		self->_elements = malloc(initial_capacity * element_size);
		check(self->_elements != NULL, NoSuchMemoryError, "initial_capacity=%u", initial_capacity);
	} else {
		self->_elements = NULL;
	}
	self->_element_size = element_size;
	self->_capacity = initial_capacity;
	self->_size = 0;
}

void _finalize(ArrayList *self) {
	if (self && self->_elements) {
		free(self->_elements);
		self->_elements = NULL;
	}
}

void _set(ArrayList *self, size_t position, void *element) {
	check(position < self->_size, IndexOutOfBoundException, "position=%u; array.length=%u", position, self->_size);
	memcpy(((char*)self->_elements) + (position * self->_element_size), element, self->_element_size);
}

void _add(ArrayList *self, void *element) {
	if (self->_capacity == self->_size) {
		unsigned int capacity = ((self->_capacity * 3) / 2) + 1;
		self->_elements = realloc(self->_elements, capacity * self->_element_size);
		check(self->_elements != NULL, NoSuchMemoryError, "capacity=%u", capacity);
		self->_capacity = capacity;
	}
	memcpy(((char*)self->_elements) + (self->_size++ * self->_element_size), element, self->_element_size);
}

void *_get(ArrayList *self, size_t position) {
	check(position < self->_size, IndexOutOfBoundException, "position=%u; array.length=%u", position, self->_size);
	return (void*)(((char*)self->_elements) + (position * self->_element_size));
}

size_t _size(ArrayList *self) {
	return self->_size;
}

Iterator _iterator(ArrayList *self) {
	// TODO
	check(0, NotImplementedOperationException, "ArrayList.iterator()");
	Iterator iterator;
	return iterator;
}


