#include "aeten/io/Buffer.h"

/*
@startuml
namespace aeten.io {
	interface Buffer {
		size_t capacity()
		size_t remaining()
		clear()
		bool isDirect()
		mark()
		size_t position()
		setPosition(size_t newPosition)
		reset()
		rewind()
	}
}
@enduml
*/
