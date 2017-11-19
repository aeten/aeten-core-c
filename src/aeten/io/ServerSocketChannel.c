#include "ServerSocketChannel.h"

#define import
#include "SelectorService.h"

/*
@startuml
!define T SocketChannel
!include Server.c
!include SocketChannel.c
!ifndef aeten_io_ServerSocketChannel
!define aeten_io_ServerSocketChannel
namespace aeten.io {
	interface ServerSocketChannel {
		+ SocketChannel* bind(SocketAddress* local, int backlog)
		+ {static} ServerSocketChannel* open()
	}
	ServerSocketChannel -|> Server
	ServerSocketChannel -|> SelectableChannel
}
!endif
@enduml
*/

ServerSocketChannel *ServerSocketChannel_open() {
	return SelectorService_openServerSocketChannel(SelectorService_provider());
}
