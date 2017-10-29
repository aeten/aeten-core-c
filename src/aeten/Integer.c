#include "Integer.h"

#define import
#include "Number.h"
#include "Hash.h"

/*
@startuml
!include Number.c
namespace aeten {
	class Integer implements Number {
		+ {static} Integer(int value) <<constructor>>
		+ {static} fromNumber(Number* value) <<constructor>>
		+ uint64_t hashCode() <<override>>
		- value: int
	}
}
@enduml
*/

#define DEFAULT_HASH_BITS 7

void Integer_new(Integer* self, int value) {
	self->_value = value;
}

void Integer_new_fromNumber(Integer* self, aeten_Number *value) {
	self->_value = Number_signedValue(value);
}

long Integer_signedLongValue(Integer* self) {
	return (long) self->_value;
}

int Integer_signedValue(Integer* self) {
	return self->_value;
}

unsigned Integer_unsignedValue(Integer* self) {
	return (unsigned) self->_value;
}

uint64_t Integer_unsignedLongValue(Integer* self) {
	return (unsigned long) self->_value;
}

float Integer_floatValue(Integer* self) {
	return (float) self->_value;
}

double Integer_doubleValue(Integer* self) {
	return (double) self->_value;
}

uint64_t Integer_hashCode(Integer* self) {
	return Hash_hash64(self->_value, DEFAULT_HASH_BITS);
}
