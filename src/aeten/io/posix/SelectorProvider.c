#include "SelectorProvider.h"
#include "Selector.h"
#include "SocketChannel.h"
#include "ServerSocketChannel.h"

#define import
#include "aeten/io/SelectorService.h"
#include "aeten/io/SelectionKey.h"
#include "aeten/io/SocketAddress.h"

/*
@startuml SelectorProvider
!include aeten/io/SelectorService.c
!ifndef aeten_io_posix_SelectorService
!define aeten_io_posix_SelectorService
namespace aeten.io.posix {
	class SelectorProvider implements aeten.io.SelectorService {
		{static} SelectorProvider() <<constructor>>
	}
}
!endif
@enduml

@startuml Selector
!include aeten/io/Selector.c
!include SelectorProvider.c!SelectorProvider
!ifndef aeten_io_posix_Selector
!define aeten_io_posix_Selector
namespace aeten.io.posix {
	class Selector implements aeten.io.Selector {
		{static} Selector(SelectorProvider *provider) <<constructor>>

		- provider: SelectorProvider*
	}
}
@enduml

@startuml SocketChannel
!include aeten/io/SocketChannel.c
!include SelectorProvider.c!SelectorProvider
!ifndef SocketChannel
!define SocketChannel
namespace aeten.io.posix {
	class SocketChannel implements aeten.io.SocketChannel {
		{static} SocketChannel(SelectorProvider *provider) <<constructor>>

		- provider: SelectorProvider*
	}
}
!endif
@enduml

@startuml ServerSocketChannel
!include aeten/io/ServerSocketChannel.c
!include SelectorProvider.c!SelectorProvider
!ifndef ServerSocketChannel
!define ServerSocketChannel
namespace aeten.io.posix {
	class ServerSocketChannel implements aeten.io.ServerSocketChannel {
		{static} ServerSocketChannel(SelectorProvider *provider) <<constructor>>

		- provider: SelectorProvider*
	}
}
!endif
@enduml
*/

/* SelectorProvider */
void SelectorProvider_new(SelectorProvider *self) {
}

aeten_io_SocketChannel *SelectorProvider_openSocketChannel(SelectorProvider *self) {
	return new_SocketChannel(self);
}

aeten_io_ServerSocketChannel *SelectorProvider_openServerSocketChannel(SelectorProvider *self) {
	return new_ServerSocketChannel(self);
}

aeten_io_Selector *SelectorProvider_openSelector(SelectorProvider *self) {
	return new_aeten_io_posix_Selector(self);
}


/* Selector */
void Selector_new(Selector *self, SelectorProvider *provider) {
	self->_provider = provider;
}

SelectorService *Selector_provider(Selector *self) {
   return (SelectorService*)self->_provider;
}

int Selector_select(Selector *self) {
	// TODO
	return -1;
}

int Selector_selectNanos(Selector *self, uint64_t nanosTimeout) {
	// TODO
	return -1;
}

void Selector_close(aeten_io_posix_Selector* self) {
	// TODO
}

/* SocketChannel */
void SocketChannel_new(SocketChannel *self, SelectorProvider *provider) {
	self->_provider = provider;
}

SelectorService *SocketChannel_provider(SocketChannel *self) {
	return (SelectorService*) self->_provider;
}

SelectionKey *SocketChannel_registerTo(SocketChannel *self, aeten_io_Selector* sel, int ops) {
	// TODO
	return NULL;
}

aeten_io_SocketChannel *SocketChannel_bind(SocketChannel *self, SocketAddress *local) {
	// TODO
	return NULL;
}

bool SocketChannel_connect(SocketChannel *self, SocketAddress *remote) {
	// TODO
	return false;
}

SocketAddress* SocketChannel_getLocalAddress(SocketChannel* self) {
	// TODO
	return NULL;
}

aeten_io_SocketChannel* SocketChannel_open(SocketChannel* self) {
	// TODO
	return NULL;
}

int SocketChannel_read(SocketChannel* self, aeten_io_ByteBuffer * dst) {
	// TODO
	return -1;
}

int SocketChannel_write(SocketChannel* self, aeten_io_ByteBuffer * src) {
	// TODO
	return -1;
}

bool SocketChannel_isOpen(SocketChannel* self) {
	// TODO
	return false;
}

void SocketChannel_close(SocketChannel* self) {
	// TODO
}


/* ServerSocketChannel */
void ServerSocketChannel_new(ServerSocketChannel *self, SelectorProvider *provider) {
	self->_provider = provider;
}

SelectorService *ServerSocketChannel_provider(ServerSocketChannel *self) {
   return (SelectorService*)self->_provider;
}

SelectionKey *ServerSocketChannel_registerTo(ServerSocketChannel *self, aeten_io_Selector* sel, int ops) {
	// TODO
	return NULL;
}

aeten_io_SocketChannel* ServerSocketChannel_bind(ServerSocketChannel* self, aeten_io_SocketAddress*  local, int  backlog) {
	// TODO
	return NULL;
}

aeten_io_ServerSocketChannel* ServerSocketChannel_open(ServerSocketChannel* self) {
	// TODO
	return NULL;
}

aeten_io_SocketChannel* ServerSocketChannel_accept(ServerSocketChannel* self) {
	// TODO
	return NULL;
}

bool ServerSocketChannel_isOpen(ServerSocketChannel* self) {
	// TODO
	return false;
}

void ServerSocketChannel_close(ServerSocketChannel* self) {
	// TODO
}
