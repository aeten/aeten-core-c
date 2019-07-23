#include "SelectorService.h"

// TODO: Implements ServiceLoader then use it
#include "aeten/io/posix/SelectorProvider.h"

#define import
#include "Selector.h"
#include "SocketChannel.h"
#include "ServerSocketChannel.h"

/*
@startuml(id=SelectorService)
!include Selector.c
!include SocketChannel.c
!include ServerSocketChannel.c
namespace aeten.io {
	interface SelectorService {
		+ SocketChannel openSocketChannel()
		+ ServerSocketChannel openServerSocketChannel()
		+ Selector openSelector()

		+ {static} SelectorService provider()
	}
}
@enduml
*/

/* Returns the system-wide default selector provider */
SelectorService SelectorService_provider() {
	return SelectorService_cast(new_aeten_io_posix_SelectorProvider());
}

