#include "SocketChannel.h"

#define import
#include "SelectorService.h"

/*
@startuml
!include SelectableChannel.c
!include SocketAddress.c
!include Readable.c
!include Writable.c
!ifndef aeten_io_SocketChannel
!define aeten_io_SocketChannel
namespace aeten.io {
	interface SocketChannel {
		+ SocketChannel* bind(SocketAddress* local)
		+ bool connect(SocketAddress* remote)
		+ SocketAddress* getLocalAddress()

		+ {static} SocketChannel* open()
	}
	SocketChannel -|> Readable
	SocketChannel -|> Writable
	SocketChannel -|> SelectableChannel
}
!endif
@enduml
*/

SocketChannel *SocketChannel_open() {
	return SelectorService_openSocketChannel(SelectorService_provider());
}
