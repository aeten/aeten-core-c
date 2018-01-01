#include "InetAddress.h"
#include "aeten/io/posix/InetAddress.h"

/*
@startuml
namespace aeten.io {
	interface InetAddress {
		+ char* hostname()
		+ uint8_t* address()
		+ size_t size()
		{static} InetAddress create(char* address)
	}
}
@enduml
*/

InetAddress InetAddress_create(char* address) {
	return InetAddress_cast(new_aeten_io_posix_InetAddress(address));
}

