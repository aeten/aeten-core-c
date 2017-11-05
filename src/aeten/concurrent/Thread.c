#include "Thread.h"

/*!
@startuml
!include Object.c
namespace aeten.concurrent {
	interface Thread<T> {
		+ start()
		+ stop()
		+ T* join()
	}
}
@enduml
*/

