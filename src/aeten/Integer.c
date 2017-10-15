#include "Integer.h"
#include "Number.h"

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

void Integer(struct _aeten__Integer* self, int value) {
	self->_value = value;
}

void from_number(struct _aeten__Integer* self, aeten__Number *value) {
	self->_value = value->signedValue(value);
}

long signedLongValue(struct _aeten__Integer* self) {
	return (long) self->_value;
}

int signedValue(struct _aeten__Integer* self) {
	return self->_value;
}

unsigned unsignedValue(struct _aeten__Integer* self) {
	return (unsigned) self->_value;
}

uint64_t unsignedLongValue(struct _aeten__Integer* self) {
	return (unsigned long) self->_value;
}

float floatValue(struct _aeten__Integer* self) {
	return (float) self->_value;
}

double doubleValue(struct _aeten__Integer* self) {
	return (double) self->_value;
}

