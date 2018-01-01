#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "InetAddress.h"

// TODO Support IPv6
/*
@startuml
!include Object.c
!include aeten/io/InetAddress.c
namespace aeten.io.posix {
	class InetAddress implements aeten.io.InetAddress {
		+ {static} InetAddress(char* address) <<constructor>>
		+ {static} fromInAddr(struct in_addr* address) <<constructor>>
		- addr: struct in_addr
		- valid: bool
		- name: char*
	}
}
@enduml
*/

void InetAddress_new(InetAddress *self, char *address) {
	self->_valid = false;
	self->_name = NULL;
	struct hostent* he;
	if(inet_aton(address, &self->_addr) == 0) {
		he = gethostbyname(address);
		if ((he != NULL) && (he->h_length > 0)) {
			check(he->h_addrtype == AF_INET, UnsuportedProtocolException, "Only IPv4 is supported")
			self->_addr = *(struct in_addr*)he->h_addr_list[0];
			self->_name = strdup(address);
			self->_valid = true;
		}
	} else {
		InetAddress_new_fromInAddr(self, &self->_addr);
	}
}

void InetAddress_new_fromInAddr(InetAddress *self, struct in_addr *address) {
	struct hostent* he;
	self->_addr = *address;
	he = gethostbyaddr(&self->_addr, sizeof(self->_addr), AF_INET);
	if ((he != NULL) && (he->h_length > 0)) {
		self->_name = strdup(he->h_name);
	}
	self->_valid = true;
}

char* InetAddress_hostname(InetAddress *self) {
	return self->_name;
}

size_t InetAddress_size(InetAddress *self) {
	return self->_valid? sizeof(self->_addr): 0;
}

uint8_t *InetAddress_address(InetAddress *self) {
	return self->_valid? (uint8_t *)&self->_addr: NULL;
}
