#include <stdio.h>

#include "ListTest.h"
#include "ArrayList.h"
#include "Integer.h"
#include "Number.h"

#define import
#include "List.h"
#include "aeten/test/Test.h"
#undef import

/*!
@startuml
!include Object.c
!include aeten/test/Test.c
!include List.c
namespace aeten.collection {
	class ListTest extends Object implements aeten.test.Test {
		{static} + ListTest(List* list) <<constructor>>

		{static} - counter: unsigned
		- list: List*
	}
} 
@enduml
*/


#define LIST(list, args) {#list, list(args)}
struct test_list {
	char *name;
	List *list;
};

int main(int argc, char** argv) {
	_counter=0;
	bool result, is_nominal = true;

	struct test_list list[] = {LIST(new__aeten_collection__ArrayList, 0)};

	for (int i=0; i<sizeof(struct test_list); ++i) {
		aeten_test__Test* test = new__aeten_collection__ListTest(list[i].list);
		for (int j=0; j<2; ++j) {
			result = (is_nominal? test->nominal(test): test->error(test));
			if (!result) {
				printf("[ FAIL ]", argv[0], list[i].name);
				++_counter;
			} else {
				printf("[SUCCES]", argv[0], list[i].name);
			}
			printf(" Test %s of %s\n", argv[0], list[i].name);
			list[i].list->finalize(list[i].list);
			is_nominal = false;
		};
	}
	return _counter;
}

void ListTest(struct _aeten_collection__ListTest* self, List* list) {
	self->_list = list;
}

static bool nominal(struct _aeten_collection__ListTest* self) {
	List *list = self->_list;
	aeten__Number *number;
	for (int i=0; i<10; ++i) {
		number = new__aeten__Integer(i);
		list->add(list, (aeten__Object*)number);
		number = (aeten__Number*)list->get(list, i);
		if (number->signedValue(number) != i) {
			fprintf(stderr, "list->get(list, %d) != %d\n", i, i);
			return false;
		}
	}
	return true;
}
/*
#ifndef NDEBUG
#define _NDEBUG
#define NDEBUG
#endif
*/
static bool error(struct _aeten_collection__ListTest* self) {
	List *list = self->_list;
	int i;
	aeten__Number *number;
	for (i=0; i<10; ++i) {
		number = new__aeten__Integer(i);
		list->add(list, (aeten__Object*)number);
	}
	number = (aeten__Number*)list->get(list, i);
	return number->signedValue(number) != i;
}
/*
#ifdef _NDEBUG
#undef _NDEBUG
#undef NDEBUG
#endif
*/

