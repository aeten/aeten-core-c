#include <stdio.h>

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

