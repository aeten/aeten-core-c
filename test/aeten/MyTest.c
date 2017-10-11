#include <stdio.h>

#include "MyTest.h"

/*
@startuml MyTest
!include Test.c
namespace aeten {
	class MyTest implements Test {
		+ {static} MyTest() <<constructor>>
		+ result: int
		- {static} count: int
	}
}
@enduml
*/

static inline void MyTest(struct _aeten__MyTest* self) {
	self->result = (10 * ++count);
}

static inline void run(struct _aeten__MyTest* self) {
	printf("%d\n", self->result);
}

int main(int argc, char** argv) {
	// Heap allocation
	aeten__Test* heap_test = new__aeten__MyTest();
	heap_test->run(heap_test);
	heap_test->finalize(heap_test);
	// Stack allocation
	aeten__Test stack_test = aeten__MyTest();
	stack_test.run(&stack_test);
	stack_test.finalize(&stack_test);
	return 0;
}

