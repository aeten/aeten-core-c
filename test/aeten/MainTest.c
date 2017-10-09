#include <string.h>
#include <stdio.h>

#include "aeten/MainTest.h"

static unsigned getValue(aeten__Test* _self);
static void setValue(aeten__Test* _self, unsigned value);

#define _AETEN__MAINTEST__INIT \
	if(self == NULL) { \
		aeten__MainTest* self = malloc(sizeof(aeten__MainTest)); \
		if(self == NULL) { \
			return NULL; \
		} \
	} \
	memset(self, 0, sizeof(aeten__MainTest)); \
	self->delete = aeten__Object__delete; \
	self->clone = aeten__Object__clone; \
	self->copy = aeten__Object__copy; \
	self->getValue = getValue; \
	self->setValue = setValue;

int main(int argc, char** argv) {
	aeten__MainTest test;
	unsigned value = 10;
	unsigned test_value = 0;
	aeten__Test* test_ifc = aeten__MainTest__new(&test, value);

	test_value = test_ifc->getValue(test_ifc);
	if (test_value != value) {
		printf("value %u!=%u\n", test_value, value);
		return 1;
	}
	value = 5;
	test_ifc->setValue(test_ifc, value);
	test_value = test_ifc->getValue(test_ifc);
	if (test_value != value) {
		printf("value %u!=%u\n", test_value, value);
		return 2;
	}
	return 0;
}

//! \memberof aeten__MainTest
aeten__Test* aeten__MainTest__new(aeten__MainTest* self, unsigned value) {
	_AETEN__MAINTEST__INIT
	self->value=value;
	return (aeten__Test*) self;
}

//! \memberof aeten__Test
static unsigned getValue(aeten__Test* _self) {
	aeten__MainTest* self = (aeten__MainTest*) _self;
	return self->value;
}

//! \memberof aeten__Test
static void setValue(aeten__Test* _self, unsigned value) {
	aeten__MainTest* self = (aeten__MainTest*) _self;
	self->value = value;
}
