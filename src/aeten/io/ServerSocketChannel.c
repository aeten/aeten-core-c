#include "ServerSocketChannel.h"

#define import
#include "SelectorService.h"

/*
@startuml
!define T SocketChannel
!include Server.c
!include SocketChannel.c
namespace aeten.io {
	interface ServerSocketChannel {
		+ ServerSocketChannel bind(SocketAddress local, int backlog)
		+ SocketAddress getLocalAddress()

		+ {static} ServerSocketChannel open()
	}
	ServerSocketChannel -|> Server
	ServerSocketChannel -|> SelectableChannel
}
@enduml
*/

ServerSocketChannel ServerSocketChannel_open() {
	return SelectorService_openServerSocketChannel(SelectorService_provider());
}
