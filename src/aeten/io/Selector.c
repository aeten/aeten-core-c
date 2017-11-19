#include "Selector.h"
#define import
#include "SelectorService.h"

/*
@startuml Selector
!include Closeable.c
!include SelectorService.c
!ifndef aeten_io_Selector
!define aeten_io_Selector
namespace aeten.io {
	interface Selector extends Closeable {
		+ int select()
		+ int selectNanos(uint64_t nanosTimeout)
		+ SelectorService* provider()
	}
}
!endif
@enduml
*/

