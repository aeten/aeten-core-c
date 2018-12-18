#include "SelectionKey.h"

/*
@startuml
!include SelectableChannel.c
!ifndef SelectableChannel
!define SelectableChannel
namespace aeten.io {
	interface SelectionKey {
		+ {static} OP_ACCEPT: uint8_t <<final>>
		+ {static} OP_CONNECT: uint8_t <<final>>
		+ {static} OP_READ: uint8_t <<final>>
		+ {static} OP_WRITE: uint8_t <<final>>

		+ uint8_t interestOps()
		+ SelectionKey setInterestOps(uint8_t interest)
		+ uint8_t readyOps()
		+ SelectableChannel channel()
		+ void* attachment()
		+ void* attach(void* attachment)
		+ bool isAcceptable() <<default>>
		+ bool isReadable() <<default>>
		+ bool isWritable() <<default>>
		+ bool isConnectable() <<default>>
	}
}
!endif
@enduml
*/

__attribute__((constructor))
static void init(void) {
	READONLY_INIT(aeten_io_SelectionKey_class.OP_READ, 1 << 0);
	READONLY_INIT(aeten_io_SelectionKey_class.OP_WRITE, 1 << 1);
	READONLY_INIT(aeten_io_SelectionKey_class.OP_CONNECT, 1 << 2);
	READONLY_INIT(aeten_io_SelectionKey_class.OP_ACCEPT, 1 << 3);
}

bool SelectionKey_isReadable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(*self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}

bool SelectionKey_isWritable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(*self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}

bool SelectionKey_isConnectable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(*self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}

bool SelectionKey_isAcceptable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(*self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}
