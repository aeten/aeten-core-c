#include "SelectableChannel.h"

/*
@startuml
!include Channel.c
!include Selector.c
!include SelectorService.c
!include SelectionKey.c
namespace aeten.io {
	interface SelectableChannel extends Channel {
		+ SelectorService provider()
		+ SelectionKey registerTo(Selector sel, uint8_t ops, void* attachment)
	}
}
@enduml
*/
