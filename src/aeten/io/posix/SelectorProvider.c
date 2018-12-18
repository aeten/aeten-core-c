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
#include "HashSet.h"
#include "ArrayList.h"
#include "aeten/io/ByteBuffer.h"
#include "aeten/io/SelectableChannel.h"
#include "aeten/io/SelectorService.h"
#include "aeten/io/SelectionKey.h"
#include "aeten/io/SocketAddress.h"
#include "aeten/io/InetAddress.h"
#include "aeten/Iterator.h"
#include "aeten/Hash.h"


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
!include HashSet.c
!include aeten/io/Selector.c
!include aeten/io/SelectableChannel.c
!include SelectorProvider.c!SelectorProvider
!include SelectorProvider.c!SelectKeySet
!ifndef aeten_io_posix_Selector
!define aeten_io_posix_Selector
namespace aeten.io.posix {
	class Selector implements aeten.io.Selector {
		+ {static} Selector(SelectorProvider *provider) <<constructor>>
		+ SelectionKey registerTo(SelectableChannel channel, int fd, uint8_t interest, void *attachment)

		- provider: SelectorProvider*
		- selection: SelectKeySet
		- nfds: int
		- size: size_t
		- keys: HashSet
		- acceptfds: fd_set
		- connectfds: fd_set
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
		+ {static} SelectKey(aeten_io_posix_Selector* selector, SelectableChannel channel, int fd, uint8_t interest, void* attachment) <<constructor>>
		+ bool equals(Object other) <<override>>
		+ uint64_t hashCode() <<override>>

		- selector: aeten_io_posix_Selector*
		- channel: SelectableChannel
		- fd: int
		- interest: uint8_t
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
		- current: SelectKey*
		- next: SelectKey*
	}
}
!endif
@enduml
*/

#define IS_INTEREST(interest_flags, interest) \
	((interest_flags & aeten_io_SelectionKey_class.OP_##interest) == aeten_io_SelectionKey_class.OP_##interest)

static inline SelectKey *findKey(ArrayList *keys, Selector *selector);
static inline SelectionKey registerKey(ArrayList **keys, Selector *selector, SelectableChannel channel, int fd, uint8_t interest, void *attachment);

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
	init_HashSet(&self->_keys);
	FD_ZERO(&self->_acceptfds);
	FD_ZERO(&self->_connectfds);
	FD_ZERO(&self->_readfds);
	FD_ZERO(&self->_writefds);
	FD_ZERO(&self->_exceptfds);
}

SelectorService Selector_provider(Selector *self) {
   return SelectorService_cast(self->_provider);
}

aeten_io_SelectionKey Selector_registerTo(Selector *self, SelectableChannel channel, int fd, uint8_t interest, void *attachment) {
	SelectKey *key = new_SelectKey(self, channel, fd, interest, attachment);
	HashSet_add(&self->_keys, Object_cast(key));
	return aeten_io_SelectionKey_cast(key);
}

int Selector_select(Selector *self) {
	Iterator i = HashSet_iterator(&self->_keys);
	while(Iterator_hasNext(i)) {
		SelectKey *key = SelectKey_cast(Iterator_next(i));
		SelectKey_setInterestOps(key, key->_interest);
	}
	Iterator_delete(i);
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
				check(retval > -1, HeapAllocationException, "%m (errno=%u)", errno);
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
				check(retval > -1, HeapAllocationException, "%m (errno=%u)", errno);
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
	check(0, UnsupportedOperationException, "Selector.close()");
}

Set Selector_keys(Selector* self) {
	// TODO
	check(0, UnsupportedOperationException, "Selector.keys()");
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

aeten_io_SelectionKey SocketChannel_registerTo(SocketChannel *self, aeten_io_Selector sel, uint8_t interest, void *attachment) {
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
	size_t position = ByteBuffer_position(dst);
	size_t size     = ByteBuffer_capacity(dst) - position;
	uint8_t *array;
	if (ByteBuffer_hasArray(dst)) {
		array = ByteBuffer_array(dst) + position;
	} else {
		check((array = malloc(size)) != NULL, HeapAllocationException, "array = malloc(%zu)", size);	
	}
	int n = read(self->_fd, array, ByteBuffer_capacity(dst) - position);
	if (n > 0) {
		ByteBuffer_setLimit(dst, position + n);
		ByteBuffer_setPosition(dst, position + n);
	} else if (n <  0) {
		switch(errno) {
			case EAGAIN: // or EWOULDBLOCK
				n = 0;
				break;
			case EBADF:
				throw(ClosedChannelException);
				break;
			case EFAULT:
				throw(InvalidAddressError);
				break;
			case EINTR:
				throw(InterruptException);
				break;
			case EINVAL:
				throw(InvalidArgumentException);
				break;
			case EIO:
				throw(IOException);
				break;
			case EISDIR:
				throw(IOException, "File %u is a directory", self->_fd);
				break;
			default:
				throw(IOException "Unknown error %m");
				break;
		}
	} else if (n == 0) {
		n = -1; // end of stream
	}
	return n;
}

int SocketChannel_write(SocketChannel *self, aeten_io_ByteBuffer src) {
	size_t position = ByteBuffer_position(src);
	uint8_t *array  = ByteBuffer_array(src) + position;
	int n           = write(self->_fd, array, ByteBuffer_limit(src) - position);
	if (n > 0) {
		ByteBuffer_setLimit(src, position + n);
		ByteBuffer_setPosition(src, position + n);
	} else if (n <  0) {
		n = -1;
		switch(errno) {
			case EAGAIN: // or EWOULDBLOCK
				n = 0;
				break;
			case EBADF:
			case EPIPE:
				throw(ClosedChannelException);
				break;
			case EDESTADDRREQ:
				throw(NotYetConnectedException);
				break;
			case EFAULT:
				throw(InvalidAddressError);
				break;
			case EINTR:
				throw(InterruptException);
				break;
			case EINVAL:
				throw(InvalidArgumentException);
				break;
			case EIO:
			case EDQUOT:
			case EFBIG:
			case ENOSPC:
			case EPERM:
				throw(IOException, "%m");
				break;
			case EISDIR:
				throw(IOException, "File %u is a directory", self->_fd);
				break;
			default:
				throw(IOException "Unknown error: %m");
				break;
		}
	} else if (n == 0) {
		n = -1; // End of stream
	}
	return n;
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

SelectionKey ServerSocketChannel_registerTo(ServerSocketChannel *self, aeten_io_Selector sel, uint8_t interest, void *attachment) {
	return registerKey(&self->_keys, Selector_dynamicCast(sel), SelectableChannel_cast(self), self->_fd, interest, attachment);
}

aeten_io_ServerSocketChannel ServerSocketChannel_bind(ServerSocketChannel *self, aeten_io_SocketAddress local, int  backlog) {
	self->_local = local;
	// TODO inspect address type in order to manage other type of socket
	InetAddress local_addr = InetSocketAddress_address(InetSocketAddress_dynamicCast(local));
	if (self->_fd == -1) {
		self->_fd = socket(AF_INET, SOCK_STREAM, 0);
		check(self->_fd > -1, IOException,"Unable to create socket: %m");
	}
	struct sockaddr_in addr = {
		.sin_addr.s_addr = 0,
		.sin_port = htons(InetSocketAddress_port(InetSocketAddress_dynamicCast(local))),
		.sin_family = AF_INET
	};
	size_t addr_size = InetAddress_size(local_addr);
	memcpy(&addr.sin_addr.s_addr, InetAddress_address(local_addr), addr_size);
	check(0 == bind(self->_fd, (struct sockaddr*)&addr, sizeof(addr)), IOException, "%m");
	check(0 == listen(self->_fd, backlog), IOException, "%m") {
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
	// TODO: switch protocol (sockaddr_in, sockaddr_un…)
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

bool SelectKeySet_contains(SelectKeySet *self, SelectionKey k) {
	return (0 != SelectionKey_readyOps(k));
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
	self->_current = NULL;
}

void SelectKeySetIterator_finalize(SelectKeySetIterator *self) {
	// TODO
}

#define ON_FD_ISSET(fd_set, action) \
	if (FD_ISSET(key->_fd, &self->_selection->_selector->_##fd_set##fds)) { \
		action ; \
	}
#define FOREACH_FDSET(action) \
		ON_FD_ISSET(read, action); \
		ON_FD_ISSET(write, action); \
		ON_FD_ISSET(except, action); \
		ON_FD_ISSET(accept, action); \
		ON_FD_ISSET(connect, action);
bool SelectKeySetIterator_hasNext(SelectKeySetIterator *self) {
	Iterator i = HashSet_iterator(&self->_selection->_selector->_keys);
	while(Iterator_hasNext(i)) {
		SelectKey *key = SelectKey_cast(Iterator_next(i));
		if (key != NULL) {
			FOREACH_FDSET(self->_next = key; break);
		}
	}
	Iterator_delete(i);
	return self->_next != NULL;
}

SelectionKey SelectKeySetIterator_next(SelectKeySetIterator *self) {
	if (NULL == self->_next) {
		SelectKeySetIterator_hasNext(self);
	}
	self->_current = self->_next;
	SelectionKey key = SelectionKey_cast(self->_next);
	Object obj = { (struct _aeten_Object_Class*)key._class_, (aeten_Object*)key._object_ };
	check(!Object_isNull(obj), NullPointerException);
	return key;
//	return SelectionKey_cast(self->_next);
}

/* SelectKey */
void SelectKey_new(SelectKey *self, Selector *selector, SelectableChannel channel, int fd, uint8_t interest, void *attachment) {
	self->_selector = selector;
	self->_channel = channel;
	self->_fd = fd;
	self->_attachment = attachment;
	SelectKey_setInterestOps(self, interest);
	if ((selector->_nfds - 1) < fd) {
		selector->_nfds = fd + 1;
	}
}

uint8_t SelectKey_interestOps(SelectKey *self) {
	return self->_interest;
}

SelectionKey SelectKey_setInterestOps(SelectKey *self, uint8_t interest) {
	self->_interest = interest;
	if (IS_INTEREST(interest, ACCEPT)) {
		FD_SET(self->_fd, &self->_selector->_acceptfds);
		FD_SET(self->_fd, &self->_selector->_readfds);
	}
	if (IS_INTEREST(interest, CONNECT)) {
		FD_SET(self->_fd, &self->_selector->_connectfds);
	}
	if (IS_INTEREST(interest, READ)) {
		FD_SET(self->_fd, &self->_selector->_readfds);
	}
	if (IS_INTEREST(interest, WRITE)) {
		FD_SET(self->_fd, &self->_selector->_writefds);
	}
	return SelectionKey_cast(self);
}

uint8_t SelectKey_readyOps(SelectKey *self) {
	int ops = 0;
	if (IS_INTEREST(self->_interest, READ)) {
		if (FD_ISSET(self->_fd, &self->_selector->_readfds) && !FD_ISSET(self->_fd, &self->_selector->_acceptfds) && !FD_ISSET(self->_fd, &self->_selector->_connectfds)) {
			ops |= aeten_io_SelectionKey_class.OP_READ;
		}
	}
	if (IS_INTEREST(self->_interest, WRITE)) {
		if (FD_ISSET(self->_fd, &self->_selector->_writefds) && !FD_ISSET(self->_fd, &self->_selector->_acceptfds) && !FD_ISSET(self->_fd, &self->_selector->_connectfds)) {
			ops |= aeten_io_SelectionKey_class.OP_WRITE;
		}
	}
	if (IS_INTEREST(self->_interest, ACCEPT)) {
		if (FD_ISSET(self->_fd, &self->_selector->_readfds) && FD_ISSET(self->_fd, &self->_selector->_acceptfds)) {
			ops |= aeten_io_SelectionKey_class.OP_ACCEPT;
		}
	}
	if (IS_INTEREST(self->_interest, CONNECT)) {
		if (FD_ISSET(self->_fd, &self->_selector->_readfds) && FD_ISSET(self->_fd, &self->_selector->_connectfds)) {
			ops |= aeten_io_SelectionKey_class.OP_CONNECT;
		}
	}
	return ops;
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

bool SelectKey_equals(SelectKey *self, Object other) {
	if (!instanceOf(&other, SelectKey)) {
		return false;
	}
	SelectKey *key = SelectKey_cast(other);
	return (instanceOf(&other, SelectKey) &&
		(self->_fd == key->_fd) &&
		(self->_interest == key->_interest) &&
		(self->_attachment == key->_attachment)
	);
}

#define SELECT_KEY_HASH_BITS 7
uint64_t SelectKey_hashCode(SelectKey *self) {
	return Hash_hash64(((uint64_t)self->_interest << 32) + (uint64_t)self->_fd, SELECT_KEY_HASH_BITS);
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

static SelectionKey registerKey(ArrayList **keys, Selector *selector, SelectableChannel channel, int fd, uint8_t interest, void *attachment) {
	if (*keys == NULL) {
		*keys = new_ArrayList(3);
	}
	SelectionKey key = SelectionKey_cast(findKey(*keys, selector));
	if (isNull(&key)) {
		key = SelectionKey_cast(new_SelectKey(selector, channel, fd, interest, attachment));
		ArrayList_add(*keys, Object_cast(&key));
		HashSet_add(&selector->_keys, Object_cast(&key));
	} else {
		SelectionKey_setInterestOps(key, interest);
		SelectionKey_attach(key, attachment);
	}
	return key;
}


