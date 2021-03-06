#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#include "ListTest.h"

#define import
#include "Object.h"
#include "Integer.h"
#include "Number.h"
#include "ArrayList.h"
#include "List.h"
#include "test/Testable.h"

/*!
@startuml
!include aeten/test/Testable.c
!include aeten/List.c
!include aeten/ArrayList.c
namespace aeten {
	class ListTest implements aeten.test.Testable {
		{static} + ListTest(List list) <<constructor>>

		{static} - counter: unsigned
		- list: List
	}
} 
@enduml
*/

#define LIST(list, ...) { #list, List_cast(new_##list( __VA_ARGS__ )) }
struct test_list {
	char *name;
    List list;
};

int main(int argc, char** argv) {
	_counter=0;
	bool result;

	struct test_list list[] = { LIST(ArrayList, 0) };

	for (int i=0; i<(sizeof(list)/sizeof(struct test_list)); ++i) {
		Testable test = Testable_cast(new_ListTest(list[i].list));
		result = Testable_test(test);
		if (!result) {
			printf("[ FAIL ]");
			++_counter;
		} else {
			printf("[SUCCES]");
		}
		printf(" Test %s of %s\n", argv[0], list[i].name);
		List_delete(list[i].list);
		Testable_delete(test);
    }
	return _counter;
}

void ListTest_new(ListTest* self, List list) {
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

bool ListTest_test(ListTest* self) {
	List list = self->_list;
	int i;
	Integer number[10];
	for (i=0; i<10; ++i) {
		init_Integer(&number[i], i);
		Integer *integer = &number[i];
		Integer_signedValue(integer);
		Object n = Object_cast(&number[i]);
		List_add(list, n);
		n = List_get(list, i);
		Number num = Number_staticCast(n);
		if (Number_signedValue(num) != i) {
			fprintf(stderr, "[ FAIL ] List_get(list, %d) != %d\n", Number_signedValue(num), i);
			return false;
		}
	}

	catch_segv(1);
	if (setjmp(context)) {
		catch_segv(0);
		return (i != Integer_signedValue(&number[i]));
	}
	List_get(list, i);
	catch_segv(0);
	return false;
}
