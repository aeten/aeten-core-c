#include <stdio.h>

#define implements
#include "SelectorTest.h"

#define import
#include "test/Testable.h"
#include "aeten/io/SelectorService.h"
#include "aeten/io/Selector.h"
#include "aeten/io/SocketChannel.h"
#include "aeten/io/SelectableChannel.h"
#include "aeten/io/ServerSocketChannel.h"
#include "aeten/io/SocketAddress.h"
#include "aeten/io/SelectionKey.h"
#include "aeten/io/InetSocketAddress.h"
#include "aeten/Set.h"
#include "aeten/Iterator.h"
#include "aeten/Object.h"

/*!
@startuml
!include Object.c
!include test/Testable.c
!include aeten/io/posix/SelectorProvider.c
namespace aeten.io {
	class SelectorTest implements aeten.test.Testable {
		+ {static} SelectorTest() <<constructor>>
	}
}
@enduml
*/

void SelectorTest_new(SelectorTest *self) {
}


bool SelectorTest_test(SelectorTest *self) {
	SelectorService     selectorService = SelectorService_provider();
	ServerSocketChannel server          = SelectorService_openServerSocketChannel(selectorService);
//	SocketChannel       client          = SelectorService_openSocketChannel(selectorService);
	Selector            selector        = SelectorService_openSelector(selectorService);

	InetSocketAddress sock_addr = InetSocketAddress_create("localhost", 10000);
	ServerSocketChannel_bind(server, SocketAddress_cast(&sock_addr), 0);
	ServerSocketChannel_registerTo(server, selector, SelectionKey_class.OP_ACCEPT | SelectionKey_class.OP_CONNECT | SelectionKey_class.OP_READ, NULL);

	while (true) {
		int r = Selector_select(selector);
		if (r == 0) {
			printf("continue\n");
			continue;
		}
		//after connected, use iterator to scan them all
		Set selectedKeys = Selector_selectedKeys(selector);
		Iterator itr = Set_iterator(selectedKeys);
		while (Iterator_hasNext(itr)) {
			SelectionKey key = SelectionKey_castFrom(Iterator_next(itr));
			if (SelectionKey_isAcceptable(key)) {
				printf("acceptable\n");

				//channel is ready to accept a new socket channel
				SelectableChannel channel = SelectionKey_channel(key);
				ServerSocketChannel ssc = SelectableChannel_castTo(channel, ServerSocketChannel);
//				ServerSocketChannel_configureBlocking(ssc, false);
				//get the socket channel and registed it to selector;

				SocketChannel sc = ServerSocketChannel_accept(ssc);
//				SocketChannel_configureBlocking(sc, false);
				if (isNull(&sc)) {
					continue;
				}

				SocketChannel_registerTo(sc, selector, SelectionKey_class.OP_READ, NULL);
				printf("connected to a new socket channel\n");
			}
			if (SelectionKey_isConnectable(key))
				printf("connectable\n");
			if (SelectionKey_isReadable(key))
				printf("readable\n");
		}
	}
	return true;
}

int main(int argc, char** argv) {
	Testable test = Testable_cast(new_SelectorTest());
	bool success = Testable_test(test);
	Testable_delete(test);
	return (success? 0: -1);
}
