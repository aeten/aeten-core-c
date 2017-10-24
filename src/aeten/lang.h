#ifndef AETEN_LANG_H
#define AETEN_LANG_H

#include <stdio.h>


//! @startuml Array
//! class aeten.Array {
//! @pause
typedef struct {
	//! @append length: size_t
	size_t length;
	//! @append size: size_t
	size_t size;
	//! @append values: void*
	void *values;
} aeten_Array;
//! @unpause
//! }
//! @enduml

/** Check expression or throws exception (TODO) */
#define check(expression, exception, message_format, ...) { \
		int _ret = 0; \
		if (!(expression)) { \
			fprintf(stderr, #exception ": " message_format "\n", ##__VA_ARGS__); \
			assert(expression); \
			_ret = 1; \
		} \
		_ret; \
	}

#define _AETEN_MARK 0x01C3860A /* SOH Æ 10 */
#define isObject(pointer) ( \
	(pointer != NULL) && ((Object*)pointer)->_mark == _AETEN_MARK \
)

#endif // AETEN_LANG_H
