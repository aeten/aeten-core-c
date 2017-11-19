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

#define _HAS_FMT_(...) _HAS_FMT1_(X, ##__VA_ARGS__)
#define _HAS_FMT1_(...) _HAS_FMT2_(__VA_ARGS__, 1, 0)
#define _HAS_FMT2_(a0, a1, N, ...) N
#define _SEMICOLON__0
#define _SEMICOLON__1 ": "
#define _SEMICOLON__x_(x) _SEMICOLON__##x
#define _SEMICOLON__x(x) _SEMICOLON__x_(x)
#define _SEMICOLON_(...) _SEMICOLON__x(_HAS_FMT_(__VA_ARGS__))
#define _FMT_(fmt, ...) _SEMICOLON_(fmt) fmt
#define _MSG_ATTR_(msg, ...) , ##__VA_ARGS__
#define _MSG_FORMAT_(...) _FMT_(__VA_ARGS__) "\n" _MSG_ATTR_(__VA_ARGS__)

/** Check expression or throws exception (TODO) */
#define check(expression, exception, ...) do { \
		if (!(expression)) { \
			fprintf(stderr, #exception _MSG_FORMAT_(__VA_ARGS__)); \
			assert(expression); \
		} \
	} while (0);

/** Throws exception (TODO) */
#define throw(exception, ...) do { \
		fprintf(stderr, #exception _MSG_FORMAT_(__VA_ARGS__)); \
	} while (0);

#define _AETEN_MARK 0x01C3860A /* SOH Æ 10 */
#define isObject(pointer) ( \
	(pointer != NULL) && ((Object*)pointer)->_mark_ == _AETEN_MARK \
)

#define READONLY_INIT(attribute, value) do { \
		typeof(attribute) _val_ = value; \
		uint8_t* _ptr_ = (uint8_t*)&attribute; \
		memcpy(_ptr_, &_val_, sizeof(attribute)); \
	} while (0)


#endif // AETEN_LANG_H
