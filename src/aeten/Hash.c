#include "Hash.h"
#include <limits.h>

/*!
@startuml
!include Object.c
namespace aeten {
	interface Hash<T> {
		+ uint32_t hash(T* value, unsigned bits) <<default>>
		+ {static} uint32_t hash32(uint32_t value, unsigned bits)
		+ {static} uint32_t hash64(uint64_t value, unsigned bits)
		+ {static} uint32_t hashBuffer(char* ptr, size_t size, unsigned bits)
		+ {static} uint32_t hashPointer(void* ptr, unsigned bits)
	}
}
@enduml
*/

/*! © Implementation from Linux kernel */

/*
 * This hash multiplies the input by a large odd number and takes the
 * high bits.  Since multiplication propagates changes to the most
 * significant end only, it is essential that the high bits of the
 * product be used for the hash value.
 *
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * Although a random odd number will do, it turns out that the golden
 * ratio phi = (sqrt(5)-1)/2, or its negative, has particularly nice
 * properties.  (See Knuth vol 3, section 6.4, exercise 9.)
 *
 * These are the negative, (1 - phi) = phi**2 = (3 - sqrt(5))/2,
 * which is very slightly easier to multiply by and makes no
 * difference to the hash distribution.
 */
#define GOLDEN_RATIO_PRIME_32 0x61C88647
#define GOLDEN_RATIO_PRIME_64 0x61C8864680B583EBull

#if __WORDSIZE == 32
#define hash_long(val, bits) Hash_hash32(val, bits)
#elif __WORDSIZE == 64
#define hash_long(val, bits) Hash_hash64(val, bits)
#else
#error Wordsize not 32 or 64
#endif

uint32_t Hash_hash(Hash* hash, void *value, unsigned bits) {
	return Hash_hashPointer(value, bits);
}

uint32_t Hash_hash32(uint32_t value, unsigned bits) {
	/* On some cpus multiply is faster, on others gcc will do shifts */
	uint32_t hash = value * GOLDEN_RATIO_PRIME_32;
	/* High bits are more random, so use them. */
	return hash >> (32 - bits);
}

uint32_t Hash_hash64(uint64_t value, unsigned bits) {
#if _WORDSIZE == 64
	/* 64x64-bit multiply is efficient on all 64-bit processors */
	return value * GOLDEN_RATIO_PRIME_64 >> (64 - bits);
#else
	/* Hash 64 bits using only 32x32-bit multiply. */
	return Hash_hash32((uint32_t)value ^ ((value >> 32) * GOLDEN_RATIO_PRIME_32), bits);
#endif
}

uint32_t Hash_hashBuffer(char *buffer, size_t size, unsigned bits) {
	uint32_t hash;
	if (size <= 32) {
		hash = Hash_hash32(*((uint32_t*)buffer), bits);
	} else if (size <= 64) {
		hash = Hash_hash64(*((uint64_t*)buffer), bits);
	} else {
		hash = 0;
		for(long i=0; i<size; ++i) {
			hash = (hash^buffer[i])*GOLDEN_RATIO_PRIME_64;
		}
	}
	return hash;
}

uint32_t Hash_hashPointer(void *ptr, unsigned bits) {
	return hash_long((unsigned long)ptr, bits);
}

