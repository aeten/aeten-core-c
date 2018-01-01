#include <errno.h>
#include <sys/select.h>
#include <unistd.h>

#define implements
#include "SelectorProvider.h"
#define implements
#include "Selector.h"
#define implements
#include "SocketChannel.h"
#define implements
#include "ServerSocketChannel.h"

#define import
#include "Object.h"
#include "SelectKey.h"
#include "SelectKeySet.h"
#include "SelectKeySetIterator.h"
#include "InetSocketAddress.h"
#include "List.h"
#include "Set.h"
#include "ArrayList.h"
#include "aeten/io/SelectableChannel.h"
#include "aeten/io/SelectorService.h"
#include "aeten/io/SelectionKey.h"
#include "aeten/io/SocketAddress.h"
#include "aeten/io/InetAddress.h"
#include "aeten/Iterator.h"

/*
@startuml(id=SelectorProvider) SelectorProvider
!include Object.c
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

@startuml(id=Selector) Selector
!include Object.c
!include aeten/io/Selector.c
!include aeten/io/SelectableChannel.c
!include SelectorProvider.c!SelectorProvider
!include SelectorProvider.c!SelectKeySet
!ifndef aeten_io_posix_Selector
!define aeten_io_posix_Selector
namespace aeten.io.posix {
	class Selector implements aeten.io.Selector {
		+ {static} Selector(SelectorProvider *provider) <<constructor>>
		+ SelectionKey registerTo(SelectableChannel channel, int fd, int interest, void *attachment)

		- provider: SelectorProvider*
		- selection: SelectKeySet
		- nfds: int
		- size: size_t
		- readfds: fd_set
		- writefds: fd_set
		- exceptfds: fd_set
	}
}
!endif
@enduml

@startuml(id=SocketChannel) SocketChannel
!include Object.c
!include aeten/io/SocketAddress.c
!include aeten/io/SocketChannel.c
!include ArrayList.c
!include SelectorProvider.c
!ifndef aeten_io_posix_SocketChannel
!define aeten_io_posix_SocketChannel
namespace aeten.io.posix {
	class SocketChannel implements aeten.io.SocketChannel {
		+ {static} SocketChannel(SelectorProvider *provider, int fd, struct sockaddr_in* local, struct sockaddr_in* peer) <<constructor>>

		- provider: SelectorProvider*
		- fd: int
		- keys: ArrayList*
		- local: SocketAddress
		- peer: SocketAddress
	}
}
!endif
@enduml

@startuml(id=ServerSocketChannel) ServerSocketChannel
!include Object.c
!include aeten/io/ServerSocketChannel.c
!include ArrayList.c
!include SelectorProvider.c
!ifndef aeten_io_posix_ServerSocketChannel
!define aeten_io_posix_ServerSocketChannel
namespace aeten.io.posix {
	class ServerSocketChannel implements aeten.io.ServerSocketChannel {
		+ {static} ServerSocketChannel(SelectorProvider *provider) <<constructor>>

		- provider: SelectorProvider*
		- fd: int
		- keys: ArrayList*
		- local: SocketAddress
		- peer_address_size: socklen_t
	}
}
!endif
@enduml

@startuml(id=SelectKeySet) SelectKeySet
!ifndef aeten_io_posix_SelectKeySet
!define aeten_io_posix_SelectKeySet
!include Object.c
!include aeten/io/SelectionKey.c
!include Set.c
!include SelectorProvider.c!Selector
namespace aeten.io.posix {
	class SelectKeySet<T extends SelectionKey> implements aeten.Set {
		+ {static} SelectKeySet(aeten_io_posix_Selector* selector) <<constructor>>
		+ finalize() <<override>>

		- selector: aeten_io_posix_Selector*
	}
}
!endif
@enduml

@startuml(id=SelectKey) SelectKey
!include Object.c
!include aeten/io/SelectionKey.c
!include SelectorProvider.c!Selector
!include aeten/io/SelectableChannel.c
!ifndef aeten_io_posix_SelectKey
!define aeten_io_posix_SelectKey
namespace aeten.io.posix {
	class SelectKey implements aeten.io.SelectionKey {
		+ {static} SelectKey(aeten_io_posix_Selector* selector, SelectableChannel channel, int fd, int interest, void* attachment) <<constructor>>

		- selector: aeten_io_posix_Selector*
		- channel: SelectableChannel
		- fd: int
		- interest: int
		- attachment: void*
	}
}
!endif
@enduml

@startuml(id=SelectKeySetIterator) SelectKeySetIterator
!include Object.c
!ifndef aeten_io_posix_SelectKeySetIterator
!define aeten_io_posix_SelectKeySetIterator
!include aeten/io/SelectionKey.c
!include SelectorProvider.c!SelectKeySet
!include Iterator.c
namespace aeten.io.posix {
	class SelectKeySetIterator<T extends SelectionKey> implements aeten.Iterator {
		+ {static} SelectKeySetIterator(SelectKeySet* selection) <<constructor>>
		+ finalize() <<override>>

		- selection: SelectKeySet*
	}
}
!endif
@enduml
*/

static inline SelectKey *findKey(ArrayList *keys, Selector *selector);
static inline SelectionKey registerKey(ArrayList **keys, Selector *selector, SelectableChannel channel, int fd, int interest, void *attachment);

/* SelectorProvider */
void SelectorProvider_new(SelectorProvider *self) {
}

aeten_io_SocketChannel SelectorProvider_openSocketChannel(SelectorProvider *self) {
	return aeten_io_SocketChannel_cast(new_SocketChannel(self, -1, NULL, NULL));
}

aeten_io_ServerSocketChannel SelectorProvider_openServerSocketChannel(SelectorProvider *self) {
	return aeten_io_ServerSocketChannel_cast(new_ServerSocketChannel(self));
}

aeten_io_Selector SelectorProvider_openSelector(SelectorProvider *self) {
	return aeten_io_Selector_cast(new_aeten_io_posix_Selector(self));
}


/* Selector */
void Selector_new(Selector *self, SelectorProvider *provider) {
	self->_provider = provider;
	self->_nfds = 0;
	self->_size = 0;
	init_SelectKeySet(&self->_selection, self);
	FD_ZERO(&self->_readfds);
	FD_ZERO(&self->_writefds);
	FD_ZERO(&self->_exceptfds);
}

SelectorService Selector_provider(Selector *self) {
   return SelectorService_cast(self->_provider);
}

aeten_io_SelectionKey Selector_registerTo(Selector *self, SelectableChannel channel, int fd, int interest, void *attachment) {
	return aeten_io_SelectionKey_cast(new_SelectKey(self, channel, fd, interest, attachment));
}

int Selector_select(Selector *self) {
	int retval = select(self->_nfds, &self->_readfds, &self->_writefds, &self->_exceptfds, NULL);
	if (retval < 0) {
		switch (errno) {
			case EBADF:
				check(retval > -1, IOException, "%m (errno=%u)", errno);
				break;
			case EINVAL:
				check(retval > -1, InvalidArgumentException, "%m (errno=%u)", errno);
				break;
			case ENOMEM:
				check(retval > -1, HeapAllocationIOException, "%m (errno=%u)", errno);
				break;
			case EINTR:
				check(retval > -1, InterruptException, "%m (errno=%u)", errno);
				break;
		}
	}
	return retval;
}

int Selector_selectNanos(Selector *self, uint64_t nanosTimeout) {
	int retval;
	const struct timespec ts = {
		nanosTimeout / 1e9,
		nanosTimeout - (( nanosTimeout / 1e9) * 1e9)
	};
	retval = pselect(self->_nfds, &self->_readfds, &self->_writefds, &self->_exceptfds, &ts, NULL);
	if (retval < 0) {
		switch (errno) {
			case EBADF:
				check(retval > -1, IOException, "%m (errno=%u)", errno);
				break;
			case EINVAL:
				check(retval > -1, InvalidArgumentException, "%m (errno=%u)", errno);
				break;
			case ENOMEM:
				check(retval > -1, HeapAllocationIOException, "%m (errno=%u)", errno);
				break;
			case EINTR:
				check(retval > -1, InterruptException, "%m (errno=%u)", errno);
				break;
		}
	}
	return retval;
}

void Selector_close(Selector* self) {
	// TODO
}

Set Selector_keys(Selector* self) {
	// TODO
	return (Set){0};
}

Set Selector_selectedKeys(Selector* self) {
	return Set_cast(&self->_selection);
}

/* SocketChannel */
void SocketChannel_new(SocketChannel *self, SelectorProvider *provider, int fd, struct sockaddr_in *local, struct sockaddr_in *peer) {
	self->_provider = provider;
	self->_keys = NULL;
	self->_fd = fd;
	self->_local = SocketAddress_cast(new_InetSocketAddress_fromSockAddr(local));
	self->_peer = SocketAddress_cast(new_InetSocketAddress_fromSockAddr(peer));
}

SelectorService SocketChannel_provider(SocketChannel *self) {
	return SelectorService_cast(self->_provider);
}

aeten_io_SelectionKey SocketChannel_registerTo(SocketChannel *self, aeten_io_Selector sel, int interest, void *attachment) {
	return registerKey(&self->_keys, Selector_dynamicCast(sel),  SelectableChannel_cast(self), self->_fd, interest, attachment);
}

aeten_io_SocketChannel SocketChannel_bind(SocketChannel *self, SocketAddress local) {
	self->_local = local;
	InetAddress local_addr = InetSocketAddress_address(InetSocketAddress_dynamicCast(local));
	check(0 == bind(self->_fd, (const struct sockaddr *)InetAddress_address(local_addr), InetAddress_size(local_addr)), IOException, "%m (errno=%u)", errno);
	return aeten_io_SocketChannel_cast(self);
}

SocketAddress SocketChannel_getRemoteAddress(SocketChannel *self) {
	return self->_peer;
}

bool SocketChannel_connect(SocketChannel *self, SocketAddress remote) {
	InetAddress remote_addr = InetSocketAddress_address(InetSocketAddress_dynamicCast(remote));
	int ret = connect(self->_fd, (struct sockaddr *)InetAddress_address(remote_addr), InetAddress_size(remote_addr));
	if(ret == 0) {
		return true;
	}
	if (errno == EINPROGRESS) {
		return false;
	}
	// TODO: throw different exception regard to errno
	check(0, ConnectionException, "Unable to connect to %s (%m)", InetAddress_hostname(remote_addr));
	return false;
}

SocketAddress SocketChannel_getLocalAddress(SocketChannel *self) {
	return self->_local;
}

int SocketChannel_read(SocketChannel *self, aeten_io_ByteBuffer dst) {
	// TODO
	return -1;
}

int SocketChannel_write(SocketChannel *self, aeten_io_ByteBuffer src) {
	// TODO
	return -1;
}

bool SocketChannel_isOpen(SocketChannel *self) {
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (self->_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	// TODO: check retval
	return ((retval == 0) && (error == 0));
}

void SocketChannel_close(SocketChannel *self) {
	close(self->_fd);
	// TODO: check error
}


/* ServerSocketChannel */
void ServerSocketChannel_new(ServerSocketChannel *self, SelectorProvider *provider) {
	self->_provider = provider;
	self->_keys = NULL;
	self->_fd = -1;
	self->_peer_address_size = 0;
}

SelectorService ServerSocketChannel_provider(ServerSocketChannel *self) {
   return SelectorService_cast(self->_provider);
}

SelectionKey ServerSocketChannel_registerTo(ServerSocketChannel *self, aeten_io_Selector sel, int interest, void *attachment) {
	return registerKey(&self->_keys, Selector_dynamicCast(sel), SelectableChannel_cast(self), self->_fd, interest, attachment);
}

aeten_io_ServerSocketChannel ServerSocketChannel_bind(ServerSocketChannel *self, aeten_io_SocketAddress local, int  backlog) {
	self->_local = local;
	InetAddress local_addr = InetSocketAddress_address(InetSocketAddress_dynamicCast(local));
	socklen_t addr_size = InetAddress_size(local_addr);
	check(0 == bind(self->_fd, (const struct sockaddr *)InetAddress_address(local_addr), addr_size), IOException, "%m (errno=%u)", errno);
	check(0 == listen(self->_fd, backlog), IOException, "%m (errno=%u)", errno) {
		self->_peer_address_size = addr_size;
	}
	return aeten_io_ServerSocketChannel_cast(self);
}

aeten_io_SocketChannel ServerSocketChannel_accept(ServerSocketChannel* self) {
	InetAddress local_addr = InetSocketAddress_address(InetSocketAddress_dynamicCast(self->_local));
	self->_peer_address_size = InetAddress_size(local_addr);
	uint8_t addr[self->_peer_address_size];
	int client_fd = accept(self->_fd, (struct sockaddr *)&addr, &self->_peer_address_size);
	check(client_fd != -1, IOException, "%m (errno=%u)", errno);
	// TODO: switch protocol (sockaddr_in, sockaddr_un...)
	return aeten_io_SocketChannel_cast(new_SocketChannel(self->_provider, client_fd, (struct sockaddr_in*)InetAddress_address(local_addr), (struct sockaddr_in*)addr));
}

SocketAddress ServerSocketChannel_getLocalAddress(ServerSocketChannel *self) {
	return self->_local;
}

bool ServerSocketChannel_isOpen(ServerSocketChannel *self) {
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (self->_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	// TODO: check retval
	return ((retval == 0) && (error == 0));
}

void ServerSocketChannel_close(ServerSocketChannel *self) {
	close(self->_fd);
	// TODO: check error
}

/* SelectKeySet */
void SelectKeySet_new(SelectKeySet *self, Selector *selector) {
	self->_selector = selector;
}

bool SelectKeySet_contains(SelectKeySet *self, SelectionKey key) {
	// TODO
	return false;
}

size_t SelectKeySet_size(SelectKeySet *self) {
	return self->_selector->_size;
}

Iterator SelectKeySet_iterator(SelectKeySet *self) {
	return Iterator_cast(new_SelectKeySetIterator(self));
}

void SelectKeySet_finalize(SelectKeySet *self) {
	// TODO
}

/* SelectKeySetIterator */
void SelectKeySetIterator_new(SelectKeySetIterator *self, SelectKeySet *selection) {
	self->_selection = selection;
}

void SelectKeySetIterator_finalize(SelectKeySetIterator *self) {
	// TODO
}

bool SelectKeySetIterator_hasNext(SelectKeySetIterator *self) {
	// TODO
	return false;
}

SelectionKey SelectKeySetIterator_next(SelectKeySetIterator *self) {
	// TODO
	return (SelectionKey){0};
}

/* SelectKey */
void SelectKey_new(SelectKey *self, Selector *selector, SelectableChannel channel, int fd, int interest, void *attachment) {
	self->_selector = selector;
	self->_channel = channel;
	self->_fd = fd;
	self->_interest = interest;
	self->_attachment = attachment;
}

int SelectKey_interestOps(SelectKey *self) {
	return self->_interest;
}

SelectionKey SelectKey_setInterestOps(SelectKey *self, int interest) {
	self->_interest = interest;
	return SelectionKey_cast(self);
}

int SelectKey_readyOps(SelectKey *self) {
	// TODO
	return 0;
}

SelectableChannel SelectKey_channel(SelectKey *self) {
	return self->_channel;
}

void *SelectKey_attachment(SelectKey *self) {
	return self->_attachment;
}

void *SelectKey_attach(SelectKey *self, void *attachment) {
	void *oldAttachment = self->_attachment;
	self->_attachment = attachment;
	return oldAttachment;
}

/* Privates functions */

static SelectKey *findKey(ArrayList *keys, Selector *selector) {
	SelectKey *key;
	if (keys == NULL) {
		return NULL;
	}

	int size = ArrayList_size(keys);
	for(int i=0; i<size; ++i) {
		key = SelectKey_staticCast(ArrayList_get(keys, i));
		if((key != NULL) && (key->_selector == selector)) {
			return key;
		}
	}
	return NULL;
}

static SelectionKey registerKey(ArrayList **keys, Selector *selector, SelectableChannel channel, int fd, int interest, void *attachment) {
	SelectionKey key = SelectionKey_cast(findKey(*keys, selector));
	if (*keys == NULL) {
		*keys = new_ArrayList(3);
	}
	if (isNull(&key)) {
		key = SelectionKey_cast(new_SelectKey(selector, channel, fd, interest, attachment));
		ArrayList_add(*keys, Object_cast(&key));
	} else {
		SelectionKey_setInterestOps(key, interest);
		SelectionKey_attach(key, attachment);
	}
	return key;
}


