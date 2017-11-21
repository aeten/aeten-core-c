#include "SelectableChannel.h"

/*
@startuml
!include Channel.c
!include Selector.c
!include SelectorService.c
!include SelectionKey.c
!ifndef aeten_io_SelectableChannel
!define aeten_io_SelectableChannel
namespace aeten.io {
	interface SelectableChannel extends Channel {
		+ SelectorService* provider()
		+ SelectionKey* registerTo(Selector* sel, int ops, void* attachment)
	}
}
!endif
@enduml
*/
