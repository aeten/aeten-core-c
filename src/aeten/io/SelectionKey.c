#include "SelectionKey.h"

/*
@startuml
!include Channel.c
namespace aeten.io {
	interface SelectionKey {
		+ {static} OP_ACCEPT: int <<final>>
		+ {static} OP_CONNECT: int <<final>>
		+ {static} OP_READ: int <<final>>
		+ {static} OP_WRITE: int <<final>>

		+ int readyOps() 
		+ Channel* channel()
		+ bool isAcceptable() <<default>>
		+ bool isReadable() <<default>>
		+ bool isWritable() <<default>>
		+ bool isConnectable() <<default>>
	}
}
@enduml
*/

__attribute__((constructor))
static void init(void) {
	READONLY_INIT(aeten_io_SelectionKey_class.OP_READ, 1 << 0);
	READONLY_INIT(aeten_io_SelectionKey_class.OP_WRITE, 1 << 2);
	READONLY_INIT(aeten_io_SelectionKey_class.OP_CONNECT, 1 << 3);
	READONLY_INIT(aeten_io_SelectionKey_class.OP_ACCEPT, 1 << 4);
}

bool SelectionKey_isReadable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}

bool SelectionKey_isWritable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}

bool SelectionKey_isConnectable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}

bool SelectionKey_isAcceptable(SelectionKey *self) {
	return ((aeten_io_SelectionKey_readyOps(self) & aeten_io_SelectionKey_class.OP_ACCEPT) != 0);
}
