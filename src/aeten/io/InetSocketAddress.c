#include "InetSocketAddress.h"
#include "aeten/io/posix/InetSocketAddress.h"

/*
@startuml
!include InetAddress.c
namespace aeten.io {
	interface InetSocketAddress extends SocketAddress {
		+ InetAddress address()
		+ int port()
		{static} InetSocketAddress create(char* address, int port)
	}
}
@enduml
*/

InetSocketAddress InetSocketAddress_create(char* address, int port) {
	return InetSocketAddress_cast(new_aeten_io_posix_InetSocketAddress(address, port));
}
