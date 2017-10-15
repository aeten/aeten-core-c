#include "Integer.h"

/*
@startuml
!include Number.c
namespace aeten {
	class Integer implements Number {
		{static} + Integer(int value) <<constructor>>
		{static} + from_number(Number* value) <<constructor>>
		- value: int
	}
}
@enduml
*/

void _Integer(Integer* self, int value) {
	self->_value = value;
}

void _from_number(Integer* self, aeten_Number *value) {
	self->_value = value->signedValue(value);
}

long signedLongValue(Integer* self) {
	return (long) self->_value;
}

int signedValue(Integer* self) {
	return self->_value;
}

unsigned unsignedValue(Integer* self) {
	return (unsigned) self->_value;
}

uint64_t unsignedLongValue(Integer* self) {
	return (unsigned long) self->_value;
}

float floatValue(Integer* self) {
	return (float) self->_value;
}

double doubleValue(Integer* self) {
	return (double) self->_value;
}

