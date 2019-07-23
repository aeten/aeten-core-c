#include "Selector.h"
#define import
#include "SelectorService.h"

/*
@startuml(id=Selector)
!include Closeable.c
!include SelectorService.c
!include Set.c
namespace aeten.io {
	interface Selector extends Closeable {
		+ int select()
		+ int selectNanos(uint64_t nanosTimeout)
		+ Set<SelectionKey> keys()
		+ Set<SelectionKey> selectedKeys()
		+ SelectorService provider()
	}
}
@enduml
*/

