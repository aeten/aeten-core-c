#include <errno.h>
#include <sys/select.h>

#include "SelectorProvider.h"
#include "Selector.h"
#include "SocketChannel.h"
#include "ServerSocketChannel.h"

#define import
#include "SelectKey.h"
#include "SelectKeySet.h"
#include "SelectKeySetIterator.h"
#include "List.h"
#include "Set.h"
#include "ArrayList.h"
#include "aeten/io/SelectableChannel.h"
#include "aeten/io/SelectorService.h"
#include "aeten/io/SelectionKey.h"
#include "aeten/io/SocketAddress.h"
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
!include SelectorProvider.c!SelectorProvider
!include aeten/io/Selector.c
!include aeten/io/SelectableChannel.c
!ifndef aeten_io_posix_Selector
!define aeten_io_posix_Selector
namespace aeten.io.posix {
	class Selector implements aeten.io.Selector {
		+ {static} Selector(SelectorProvider *provider) <<constructor>>
		+ SelectionKey* registerTo(SelectableChannel* channel, int fd, int interest, void *attachment)

		- provider: SelectorProvider*
		- selection: Set*
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
!include aeten/io/SocketChannel.c
!include List.c
!include SelectorProvider.c
!ifndef aeten_io_posix_SocketChannel
!define aeten_io_posix_SocketChannel
namespace aeten.io.posix {
	class SocketChannel implements aeten.io.SocketChannel {
		+ {static} SocketChannel(SelectorProvider *provider) <<constructor>>

		- provider: SelectorProvider*
		- fd: int
		- keys: List*
	}
}
!endif
@enduml

@startuml(id=ServerSocketChannel) ServerSocketChannel
!include Object.c
!include aeten/io/ServerSocketChannel.c
!include List.c
!include SelectorProvider.c
!ifndef aeten_io_posix_ServerSocketChannel
!define aeten_io_posix_ServerSocketChannel
namespace aeten.io.posix {
	class ServerSocketChannel implements aeten.io.ServerSocketChannel {
		+ {static} ServerSocketChannel(SelectorProvider *provider) <<constructor>>

		- provider: SelectorProvider*
		- fd: int
		- keys: List*
	}
}
!endif
@enduml

@startuml(id=SelectKeySet) SelectKeySet
!ifndef aeten_io_posix_SelectKeySet
!define aeten_io_posix_SelectKeySet
!include Object.c
!define T SelectKey
!include Set.c
!include SelectorProvider.c!Selector
!include SelectorProvider.c!SelectKey
namespace aeten.io.posix {
	class SelectKeySet<T extends SelectKey> implements aeten.Set {
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
		+ {static} SelectKey(aeten_io_posix_Selector* selector, SelectableChannel* channel, int fd, int interest, void* attachment) <<constructor>>

		- selector: aeten_io_posix_Selector*
		- channel: SelectableChannel*
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
!define T SelectKey
!include Iterator.c
namespace aeten.io.posix {
	class SelectKeySetIterator<T> implements aeten.Iterator {
		+ {static} SelectKeySetIterator(SelectKeySet* selection) <<constructor>>
		+ finalize() <<override>>

		- selection: SelectKeySet*
	}
}
!endif
@enduml
*/

static SelectKey *findKey(List **keys, Selector *selector);
static SelectionKey *registerKey(List **keys, Selector *selector, SelectableChannel *channel, int fd, int interest, void *attachment);

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
	self->_nfds = 0;
	self->_size = 0;
	self->_selection = new_SelectKeySet(self);
	FD_ZERO(&self->_readfds);
	FD_ZERO(&self->_writefds);
	FD_ZERO(&self->_exceptfds);
}

SelectorService *Selector_provider(Selector *self) {
   return (SelectorService*)self->_provider;
}

aeten_io_SelectionKey* Selector_registerTo(Selector *self, SelectableChannel* channel, int fd, int interest, void *attachment) {
	return new_SelectKey(self, channel, fd, interest, attachment);
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

Set* Selector_keys(Selector* self) {
	// TODO
	return NULL;
}

Set* Selector_selectedKeys(Selector* self) {
	return self->_selection;
}

/* SocketChannel */
void SocketChannel_new(SocketChannel *self, SelectorProvider *provider) {
	self->_provider = provider;
	self->_keys = NULL;
	self->_fd = -1;
}

SelectorService *SocketChannel_provider(SocketChannel *self) {
	return (SelectorService*) self->_provider;
}

aeten_io_SelectionKey *SocketChannel_registerTo(SocketChannel *self, aeten_io_Selector* sel, int interest, void *attachment) {
	return registerKey(&self->_keys, (Selector*)sel,  (SelectableChannel*)self, self->_fd, interest, attachment);
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
	self->_keys = NULL;
	self->_fd = -1;
}

SelectorService *ServerSocketChannel_provider(ServerSocketChannel *self) {
   return (SelectorService*)self->_provider;
}

SelectionKey *ServerSocketChannel_registerTo(ServerSocketChannel *self, aeten_io_Selector* sel, int interest, void *attachment) {
	return registerKey(&self->_keys, (Selector*)sel, (SelectableChannel*)self, self->_fd, interest, attachment);
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

/* SelectKeySet */
void SelectKeySet_new(SelectKeySet *self, Selector *selector) {
	self->_selector = selector;
}

bool SelectKeySet_contains(SelectKeySet *self, SelectKey *key) {
	// TODO
	return false;
}

size_t SelectKeySet_size(SelectKeySet *self) {
	// TODO
	return self->_selector->_size;
}

Iterator *SelectKeySet_iterator(SelectKeySet *self) {
	return new_SelectKeySetIterator(self);
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

void *SelectKeySetIterator_next(SelectKeySetIterator *self) {
	// TODO
	return NULL;
}

/* SelectKey */
void SelectKey_new(SelectKey *self, Selector *selector, SelectableChannel *channel, int fd, int interest, void *attachment) {
	self->_selector = selector;
	self->_channel = channel;
	self->_fd = fd;
	self->_interest = interest;
	self->_attachment = attachment;
}

int SelectKey_interestOps(SelectKey *self) {
	return self->_interest;
}

SelectionKey *SelectKey_setInterestOps(SelectKey *self, int interest) {
	self->_interest = interest;
	return (SelectionKey*)self;
}

int SelectKey_readyOps(SelectKey *self) {
	// TODO
	return 0;
}

SelectableChannel *SelectKey_channel(SelectKey *self) {
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

static SelectKey *findKey(List **keys, Selector *selector) {
	SelectKey *key;
	if (*keys == NULL) {
		return NULL;
	}

	int size = List_size(*keys);
	for(int i=0; i<size; ++i) {
		key = (SelectKey*)List_get(*keys, i);
		if((key != NULL) && (key->_selector == (aeten_io_posix_Selector*)selector)) {
			return key;
		}
	}
	return NULL;
}

static SelectionKey *registerKey(List **keys, Selector *selector, SelectableChannel *channel, int fd, int interest, void *attachment) {
	SelectionKey *key = (SelectionKey*)findKey(keys, selector);
	if (*keys == NULL) {
		*keys = new_ArrayList(3, sizeof(SelectionKey*));
	}
	if (key == NULL) {
		key = new_SelectKey(selector, channel, fd, interest, attachment);
		List_add(*keys, key);
	} else {
		SelectionKey_setInterestOps(key, interest);
		SelectionKey_attach(key, attachment);
	}
	return key;
}


