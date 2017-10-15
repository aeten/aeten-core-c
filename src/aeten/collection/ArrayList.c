#include "lang.h"

#include "ArrayList.h"

#define import
#include "Object.h"
#undef import

/*
@startuml
!include Object.c
!include List.puml
namespace aeten.collection {
	class ArrayList<T> implements List {
		- capacity : unsigned
		- size : unsigned
		- elements : T[]

		{static} + ArrayList(unsigned initial_capacity) <<constructor>>
		+ finalize() <<override>>
	}
	note right of ArrayList::ArrayList
		Constructor
	end note
}
@enduml
*/

void ArrayList(struct _aeten_collection__ArrayList *self, unsigned initial_capacity) {
	if (initial_capacity>0) {
		self->_elements = malloc(initial_capacity * sizeof(self->_elements));
		check(self->_elements != NULL, NoSuchMemoryError, "initial_capacity=%u", initial_capacity);
	} else {
		self->_elements = NULL;
	}
	self->_capacity = initial_capacity;
	self->_size = 0;
}

void finalize(struct _aeten_collection__ArrayList *self) {
	if (self && self->_elements) {
		free(self->_elements);
		self->_elements = NULL;
	}
}

void set(struct _aeten_collection__ArrayList *self, unsigned int position, Object *element) {
	check(position < self->_size, IndexOutOfBoundException, "position=%u; array.length=%u", position, self->_size);
	self->_elements[position] = element;
}

void add(struct _aeten_collection__ArrayList *self, Object *element) {
	if (self->_capacity == self->_size) {
		unsigned int capacity = ((self->_capacity * 3) / 2) + 1;
		self->_elements = realloc(self->_elements, capacity * sizeof(self->_elements));
		check(self->_elements != NULL, NoSuchMemoryError, "capacity=%u", capacity);
		self->_capacity = capacity;
	}
	self->_elements[self->_size++] = element;
}

Object *get(struct _aeten_collection__ArrayList *self, unsigned int position) {
	check(position < self->_size, IndexOutOfBoundException, "position=%u; array.length=%u", position, self->_size);
	return self->_elements[position];
}

unsigned size(struct _aeten_collection__ArrayList *self) {
	return self->_size;
}

