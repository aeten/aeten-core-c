#include <stdio.h>

#define implements
#include "BlockingQueueTest.h"

#define import
#include "concurrent/ArrayBlockingQueue.h"
#include "concurrent/BlockingQueue.h"
#include "test/Testable.h"
#include "Integer.h"
#include "Number.h"
#include "Object.h"

/*!
@startuml
!include Integer.c
!include concurrent/Condition.c
!include concurrent/ArrayBlockingQueue.c
!include test/Testable.c
namespace aeten {
	class BlockingQueueTest implements aeten.test.Testable {
		+ {static} BlockingQueueTest() <<constructor>>
	}
}
@enduml
*/

#define queue_check_offer(success, value) { \
	Number _value = Number_cast(new_Integer(value)); \
	if (BlockingQueue_offer(queue, Object_cast(&_value)) != success) { \
		printf("[FAILED] %3d: Try to offer %d: success != " #success " (length=%lu, value=%u)\n", __LINE__, value, BlockingQueue_size(queue), Number_unsignedValue(_value)); \
		\
		Number_delete(_value); \
		BlockingQueue_delete(queue); \
		return false; \
	} \
	printf("[SUCCESS] %3d: Try to offer %d: success == " #success " (length=%lu)\n", __LINE__, Number_signedValue(_value), BlockingQueue_size(queue)); \
}

#define queue_check_poll(success, value) { \
	Number _value = Number_staticCast(BlockingQueue_poll(queue)); \
	if ((!isNull(&_value)) != success) { \
		printf("[FAILED] %3d: Try to poll %d: success != " #success " (length=%lu)\n", __LINE__, value, BlockingQueue_size(queue)); \
	    if (!isNull(&_value)) Number_delete(_value); \
		BlockingQueue_delete(queue); \
		return false; \
	} \
	printf("[SUCCESS] %3d: Try to poll %d: success == " #success " (length=%lu)\n", __LINE__, isNull(&_value)? -1: Number_signedValue(_value), BlockingQueue_size(queue)); \
	if (!isNull(&_value)) Number_delete(_value); \
}


void BlockingQueueTest_new(BlockingQueueTest *self) {
}


bool BlockingQueueTest_test(BlockingQueueTest *self) {
	int i, length=10;
	BlockingQueue queue = BlockingQueue_cast(new_ArrayBlockingQueue(length));
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
	BlockingQueue_delete(queue);
	return true;
}

int main(int argc, char** argv) {
	Testable test = Testable_cast(new_BlockingQueueTest());
	bool success = Testable_test(test);
	Testable_test(test);
	Testable_delete(test);
	return (success? 0: -1);
}
