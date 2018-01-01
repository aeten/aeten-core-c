#include "InetSocketAddress.h"

#define import
#include "InetAddress.h"

/*!
@startuml
!include Object.c
!include InetAddress.c
!include aeten/io/InetSocketAddress.c
namespace aeten.io.posix {
	class InetSocketAddress implements aeten.io.InetSocketAddress {
		{static} InetSocketAddress(char* address, int port) <<constructor>>
		{static} fromSockAddr(const struct sockaddr_in* addr) <<constructor>>
		# finalize() <<override>>
		- addr: aeten_io_posix_InetAddress
		- port: int
	}
}
@enduml
*/

void InetSocketAddress_new(InetSocketAddress *self, char *address, int port) {
	init_InetAddress(&self->_addr, address);
	self->_port = port;
}

void InetSocketAddress_new_fromSockAddr(InetSocketAddress *self, const struct sockaddr_in* addr) {
	check(addr != NULL, NullPointerException);
	struct in_addr address = addr->sin_addr;
	init_InetAddress_fromInAddr(&self->_addr, &address);
	self->_port = addr->sin_port;
}

void InetSocketAddress_finalize(InetSocketAddress *self) {
	InetAddress_delete(&self->_addr);
}

aeten_io_InetAddress InetSocketAddress_address(InetSocketAddress *self) {
	return aeten_io_InetAddress_cast(&self->_addr);
}

int InetSocketAddress_port(InetSocketAddress *self) {
	return self->_port;
}
