#include <stdio.h>

#include "BlockingQueueTest.h"

#define import
#include "ArrayBlockingQueue.h"
#include "BlockingQueue.h"
#include "test/Testable.h"
#include "Integer.h"
#include "Number.h"

/*
@startuml
!include Object.c
!include Integer.c
!include concurrent/Condition.c
!include collection/ArrayBlockingQueue.c
!include test/Testable.c
namespace aeten.collection {
	class BlockingQueueTest implements aeten.test.Testable {
		+ {static} BlockingQueueTest() <<constructor>>
	}
}
@enduml
*/

#define queue_check_offer(success, value) { \
	Number *_value = new_Integer(value); \
	if (queue->offer(queue, _value) != success) { \
		printf("%3d: [FAILED] Try to offer %d: success != " #success " (length=%u, valu=%u)\n", __LINE__, value, queue->size(queue), _value); \
		\
		_value->delete(_value); \
		queue->finalize(queue); \
		return false; \
	} \
	printf("%3d: [SUCCESS] Try to offer %d: success == " #success " (length=%u)\n", __LINE__, _value->signedValue(_value), queue->size(queue)); \
}

#define queue_check_poll(success, value) { \
	Number *_value = (Number*)queue->poll(queue); \
	if ((_value != NULL) != success) { \
		printf("%3d: [FAILED] Try to poll %d: success != " #success " (length=%u)\n", __LINE__, value, queue->size(queue)); \
		if (_value) _value->delete(_value); \
		queue->finalize(queue); \
		return false; \
	} \
	printf("%3d: [SUCCESS] Try to poll %d: success == " #success " (length=%u)\n", __LINE__, _value? _value->signedValue(_value): -1, queue->size(queue)); \
	if (_value) _value->delete(_value); \
}


void BlockingQueueTest_new(BlockingQueueTest *self) {
}

bool test(BlockingQueueTest *self) {
	int i, length=10;
	BlockingQueue *queue = new_ArrayBlockingQueue(length);
	queue_check_poll(false, -1);
	for(i=0; i<length; ++i) {
		queue_check_offer(true, i);
	}
	queue_check_offer(false, ++i);
	queue_check_poll(true, i);
	queue_check_offer(true, ++i);
	queue_check_offer(false, ++i);

	for(i=0; i<length; ++i) {
		queue_check_poll(true, i);
	}

	queue_check_offer(true, ++i);
	queue_check_offer(true, ++i);
	queue_check_poll(true, i);
	queue_check_poll(true, i);
	queue_check_poll(false, i);


	for(i=0; i<length; ++i) {
		queue_check_offer(true, i);
	}
	queue->delete(queue);
	return true;
}

int main(int argc, char** argv) {
	Testable test = new_BlockingQueueTest_fromStack();
	bool success = test.test(&test);
	test.finalize(&test);
	return (success? 0: -1);
}
