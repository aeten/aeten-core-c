#include <stdio.h>

#include "RunnableTest.h"

#define import
#include "Runnable.h"

/*
@startuml RunnableTest
!include Object.c
!include Runnable.c
namespace aeten {
	class RunnableTest implements Runnable {
		+ {static} RunnableTest() <<constructor>>
		+ result: int
		- {static} count: int
	}
}
@enduml
*/

inline void RunnableTest_new(RunnableTest* self) {
	self->result = (10 * ++_count);
}

void RunnableTest_run(RunnableTest* self) {
	self->result++;
}

int main(int argc, char** argv) {
	// Heap allocation
	RunnableTest *test = (RunnableTest*)new_RunnableTest();
	if (test->result != 10) {
		return 1;
	}
	RunnableTest_run(test);
	if (test->result != 11) {
		return 2;
	}
	RunnableTest_delete(test);
	// Stack allocation
	Runnable stack_test = new_RunnableTest_fromStack();
	test = (RunnableTest*) stack_test._object_;
	if (test->result != 20) {
		return 4;
	}
	Runnable_run(&stack_test);
	if (test->result != 21) {
		return 8;
	}
	Runnable_delete(&stack_test);
	return 0;
}

