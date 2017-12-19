#include <errno.h>

#include "ByteBuffer.h"
#include "DirectByteBuffer.h"

/*
@startuml ByteBuffer
namespace aeten.io {
	interface ByteBuffer extends Buffer {
		{static} ByteBuffer allocateDirect(size_t capacity)
		{static} ByteBuffer wrap(uint8_t* src, size_t offset, size_t length)
		get(uint8_t* dst, size_t offset, size_t length)
		uint8_t getByte(size_t index)
		put(uint8_t* src, size_t offset, size_t length)
		putByte(uint8_t b)
	}
}
@enduml

@startuml DirectByteBuffer
!include Object.c
namespace aeten.io {
	class DirectByteBuffer implements ByteBuffer {
		{static} DirectByteBuffer(size_t capacity) <<constructor>>
		{static} from(uint8_t* src, size_t offset, size_t length) <<constructor>>

		- buffer: uint8_t*
		- capacity: size_t
		- position: size_t
		- mark: size_t
	}
}
@enduml
*/

ByteBuffer ByteBuffer_allocateDirect(size_t capacity) {
	return ByteBuffer_cast(new_aeten_io_DirectByteBuffer(capacity));
}

ByteBuffer ByteBuffer_wrap(uint8_t* src, size_t offset, size_t length) {
	return ByteBuffer_cast(new_aeten_io_DirectByteBuffer_from(src, offset, length));
}

void DirectByteBuffer_new(DirectByteBuffer *self, size_t capacity) {
	self->_capacity = capacity;
	self->_buffer = (uint8_t*)calloc(capacity, sizeof(uint8_t));
	check(self->_buffer, HeapAllocationException, "%m (%u)", errno);
	self->_position = 0;
}

void DirectByteBuffer_new_from(DirectByteBuffer *self, uint8_t* src, size_t offset, size_t length) {
	self->_buffer = src;
	check(self->_buffer, NullPointerException);
	self->_capacity = length;
	self->_position = offset;
}

size_t DirectByteBuffer_capacity(DirectByteBuffer *self) {
	return self->_capacity;
}

size_t DirectByteBuffer_remaining(DirectByteBuffer *self) {
	return self->_capacity - self->_position;
}

void DirectByteBuffer_clear(DirectByteBuffer *self) {
	self->_mark = 0;
}

bool DirectByteBuffer_isDirect(DirectByteBuffer *self) {
	return true;
}


void DirectByteBuffer_mark(DirectByteBuffer *self) {
	self->_mark = self->_position;
}

size_t DirectByteBuffer_position(DirectByteBuffer *self) {
	return self->_position;
}

void DirectByteBuffer_setPosition(DirectByteBuffer *self, size_t newPosition) {
	self->_position = newPosition;
	if (self->_mark > newPosition) {
		self->_mark = 0;
	}
}

void DirectByteBuffer_reset(DirectByteBuffer *self) {
	self->_position = self->_mark;
}

void DirectByteBuffer_rewind(DirectByteBuffer *self) {
	self->_position = 0;
	self->_mark = 0;
}

uint8_t DirectByteBuffer_getByte(DirectByteBuffer *self, size_t index) {
	return self->_buffer[index];
}

void DirectByteBuffer_putByte(DirectByteBuffer *self, uint8_t b) {
	*(self->_buffer + self->_position++) = b;
}

void DirectByteBuffer_get(DirectByteBuffer *self, uint8_t* dst, size_t offset, size_t length) {
	memcpy(dst + offset, self->_buffer + self->_position, length);
}

void DirectByteBuffer_put(DirectByteBuffer *self, uint8_t* src, size_t offset, size_t length) {
	memcpy(self->_buffer + self->_position, src + offset, length);
	self->_position += length;
}

