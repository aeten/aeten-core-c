#include <stdio.h>

#define import
#include "RunnableTest.h"

/*
@startuml RunnableTest
!include Runnable.puml
namespace aeten {
	class RunnableTest implements Runnable {
		+ {static} RunnableTest() <<constructor>>
		+ result: int
		- {static} count: int
	}
}
@enduml
*/

static inline void RunnableTest(struct _aeten__RunnableTest* self) {
	self->result = (10 * ++_count);
}

static inline void run(struct _aeten__RunnableTest* self) {
	printf("%d\n", self->result);
}

int main(int argc, char** argv) {
	// Heap allocation
	Runnable* heap_test = new__aeten__RunnableTest();
	heap_test->run(heap_test);
	heap_test->finalize(heap_test);
	// Stack allocation
	Runnable stack_test = aeten__RunnableTest();
	stack_test.run(&stack_test);
	stack_test.finalize(&stack_test);
	return 0;
}

