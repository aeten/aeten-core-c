#include "Long.h"

#define import
#include "Object.h"
#include "Number.h"
#include "Hash.h"

/*!
@startuml
!include Number.c
namespace aeten {
	class Long implements Number {
		+ {static} Long(int value) <<constructor>>
		+ {static} fromNumber(Number value) <<constructor>>
		+ bool equals(Object other) <<override>>
		+ uint64_t hashCode() <<override>>
		- value: int64_t
	}
}
@enduml
*/

#define DEFAULT_HASH_BITS 7

void Long_new(Long* self, int value) {
	self->_value = value;
}

void Long_new_fromNumber(Long* self, aeten_Number value) {
	self->_value = Number_signedLongValue(value);
}

long Long_signedLongValue(Long* self) {
	return self->_value;
}

int Long_signedValue(Long* self) {
	return self->_value;
}

unsigned Long_unsignedValue(Long* self) {
	return (unsigned) self->_value;
}

uint64_t Long_unsignedLongValue(Long* self) {
	return (unsigned long) self->_value;
}

float Long_floatValue(Long* self) {
	return (float) self->_value;
}

double Long_doubleValue(Long* self) {
	return (double) self->_value;
}

uint64_t Long_hashCode(Long* self) {
	return Hash_hash64(self->_value, DEFAULT_HASH_BITS);
}

bool Long_equals(Long* self, Object other) {
	return Number_signedLongValue(Number_dynamicCast(other)) == self->_value;
}
