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

inline void _RunnableTest(RunnableTest* self) {
	self->result = (10 * ++_count);
}

static inline void run(RunnableTest* self) {
	self->result++;
}

int main(int argc, char** argv) {
	// Heap allocation
	RunnableTest *test = (RunnableTest*)RunnableTest_new();
	if (test->result != 10) {
		return 1;
	}
	test->run(test);
	if (test->result != 11) {
		return 2;
	}
	test->finalize(test);
	free(test);
	// Stack allocation
	Runnable stack_test = RunnableTest_newInStack();
	test = (RunnableTest*) &stack_test;
	if (test->_self->result != 20) {
		return 4;
	}
	stack_test.run(&stack_test);
	if (test->_self->result != 21) {
		return 8;
	}
	stack_test.finalize(&stack_test);
	return 0;
}

