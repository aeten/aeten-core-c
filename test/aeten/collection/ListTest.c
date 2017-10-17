#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#include "ListTest.h"

#define import
#include "ArrayList.h"
#include "List.h"
#include "Integer.h"
#include "Number.h"
#include "Object.h"
#include "test/Testable.h"

/*!
@startuml
!include Object.c
!include Integer.c
!include aeten/test/Testable.c
!include aeten/collection/List.c
!include aeten/collection/ArrayList.c
namespace aeten.collection {
	class ListTest implements aeten.test.Testable {
		{static} + ListTest(List* list) <<constructor>>

		{static} - counter: unsigned
		- list: List *
	}
} 
@enduml
*/

#define LIST(list, args) {#list, new_##list(args)}
struct test_list {
	char *name;
	List *list;
};

int main(int argc, char** argv) {
	_counter=0;
	bool result;

	struct test_list list[] = {LIST(ArrayList, 0)};

	for (int i=0; i<(sizeof(list)/sizeof(struct test_list)); ++i) {
		Testable* test = new_ListTest(list[i].list);
		result = test->test(test);
		if (!result) {
			printf("[ FAIL ]", argv[0], list[i].name);
			++_counter;
		} else {
			printf("[SUCCES]", argv[0], list[i].name);
		}
		printf(" Test %s of %s\n", argv[0], list[i].name);
		list[i].list->finalize(list[i].list);
	}
	return _counter;
}

void ListTest_new(ListTest* self, List* list) {
	self->_list = list;
}

static jmp_buf  context;

static void sig_handler(int signo) {
	longjmp(context, 1);
}

static void catch_segv(bool iscatch) {
	struct sigaction sa;

	if (iscatch) {
		memset(&sa, 0, sizeof(struct sigaction));
		sa.sa_handler = sig_handler;
		sa.sa_flags = SA_RESTART;
		sigaction(SIGSEGV, &sa, NULL);
		sigaction(SIGABRT, &sa, NULL);
	} else {
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGSEGV);
		sigaddset(&sa.sa_mask, SIGABRT);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}
}

static bool test(ListTest* self) {
	List *list = self->_list;
	Number *number;
	int i;
	for (i=0; i<10; ++i) {
		number = new_Integer(i);
		list->add(list, (Object*)number);
		number = (Number*)list->get(list, i);
		if (number->signedValue(number) != i) {
			fprintf(stderr, "list->get(list, %d) != %d\n", i, i);
			return false;
		}
	}

	catch_segv(1);
	if (setjmp(context)) {
		catch_segv(0);
		return (i != number->signedValue(number));
	}
	number = (Number*)list->get(list, i);
	catch_segv(0);
	return false;
}
