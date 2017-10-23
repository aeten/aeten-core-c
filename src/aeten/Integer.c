#include "Integer.h"

#define import
#include "Number.h"
#include "Hasher.h"

/*
@startuml
!include Number.c
namespace aeten {
	class Integer implements Number {
		{static} + Integer(int value) <<constructor>>
		{static} + fromNumber(Number* value) <<constructor>>
		- value: int
	}
}
@enduml
*/

#define DEFAULT_HASH_BITS 7

void _new(Integer* self, int value) {
	self->_value = value;
}

void _new_fromNumber(Integer* self, aeten_Number *value) {
	self->_value = Number_signedValue(value);
}

long _signedLongValue(Integer* self) {
	return (long) self->_value;
}

int _signedValue(Integer* self) {
	return self->_value;
}

unsigned _unsignedValue(Integer* self) {
	return (unsigned) self->_value;
}

uint64_t _unsignedLongValue(Integer* self) {
	return (unsigned long) self->_value;
}

float _floatValue(Integer* self) {
	return (float) self->_value;
}

double _doubleValue(Integer* self) {
	return (double) self->_value;
}

long hashCode(Integer* self) {
	return Hasher_hash64(self->_value, DEFAULT_HASH_BITS);
}
