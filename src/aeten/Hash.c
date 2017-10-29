#include "Hash.h"

/*!
@startuml
namespace aeten {
	interface Hash<T> {
		uint64_t hash(T* value, unsigned bits) <<default>>
		{static} uint32_t hash32(uint32_t value, unsigned bits)
		{static} uint64_t hash64(uint64_t value, unsigned bits)
		{static} uint64_t hashBuffer(char* ptr, size_t size, unsigned bits)
		{static} uint64_t hashPointer(void* ptr, unsigned bits)
	}
}
@enduml
*/

/*! © Implementation from Linux kernel */

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
/*  2^63 + 2^61 - 2^57 + 2^54 - 2^51 - 2^18 + 1 */
#define GOLDEN_RATIO_PRIME_64 0x9e37fffffffc0001UL

#if __SIZEOF_POINTER__ == 8
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_64
#define hash_long(val, bits) Hash_hash32(val, bits)
#else
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_32                                                                                       
#define hash_long(val, bits) Hash_hash64(val, bits)
#endif

uint64_t Hash_hash(Hash* hash, void *value, unsigned bits) {
	return Hash_hashPointer(value, bits);
}

uint32_t Hash_hash32(uint32_t value, unsigned bits) {
	/* On some cpus multiply is faster, on others gcc will do shifts */
	uint32_t hash = value * GOLDEN_RATIO_PRIME_32;
	/* High bits are more random, so use them. */
	return hash >> (32 - bits);
}

uint64_t Hash_hash64(uint64_t value, unsigned bits) {
	uint64_t hash = value;

	/*  Sigh, gcc can't optimise this alone like it does for 32 bits. */
	uint64_t n = hash;
	n <<= 18;
	hash -= n;
	n <<= 33;
	hash -= n;
	n <<= 3;
	hash += n;
	n <<= 3;
	hash -= n;
	n <<= 4;
	hash += n;
	n <<= 2;
	hash += n;

	/* High bits are more random, so use them. */
	return hash >> (64 - bits);
}

uint64_t Hash_hashBuffer(char *buffer, size_t size, unsigned bits) {
	uint64_t hash;
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

uint64_t Hash_hashPointer(void *ptr, unsigned bits) {
	return hash_long((uint64_t)ptr, bits);
}

