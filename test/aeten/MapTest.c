#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#include "MapTest.h"

#define import
#include "Object.h"
#include "Integer.h"
#include "Long.h"
#include "Number.h"
#include "HashMap.h"
#include "Map.h"
#include "test/Testable.h"

/*!
@startuml
!include aeten/test/Testable.c
!include aeten/Map.c
namespace aeten {
	class MapTest implements aeten.test.Testable {
		{static} + MapTest(Map map) <<constructor>>

		{static} - counter: unsigned
		- map: Map
	}
} 
@enduml
*/

#define MAP(map, ...) { #map, Map_cast(new_##map( __VA_ARGS__ )) }
struct test_map {
	char *name;
    Map map;
};

int main(int argc, char** argv) {
	_counter=0;
	bool result;

	struct test_map map[] = { MAP(HashMap) };

	for (uint32_t i=0; i<(sizeof(map)/sizeof(struct test_map)); ++i) {
		Testable test = Testable_cast(new_MapTest(map[i].map));
		result = Testable_test(test);
		if (!result) {
			printf("[ FAIL ]");
			++_counter;
		} else {
			printf("[SUCCES]");
		}
		printf(" Test %s of %s\n", argv[0], map[i].name);
		Map_delete(map[i].map);
		Testable_delete(test);
    }
	return _counter;
}

void MapTest_new(MapTest *self, Map map) {
	self->_map = map;
}

bool MapTest_test(MapTest* self) {
	Map map = self->_map;
	Number number_get;
	uint32_t i;
	Long number[7290-6139];
	Integer idx[7290-6139+1];
	for (i=6139; i<7290; ++i) {
		init_Long(&number[i-6139], 0xFFFFFFFF00000000 + i);
		init_Integer(&idx[i-6139], i);
		Map_put(map, Object_cast(&idx[i-6139]), Object_cast(&number[i-6139]));
		number_get = Number_staticCast(Map_get(map, Object_cast(&idx[i-6139])));
		if (isNull(&number_get)) {
			fprintf(stderr, "[ FAIL ] (Map_get(map, %u) = null)\n", i);
			return false;
		}
		if (!Number_equals(number_get, Object_cast(&number[i-6139]))) {
			fprintf(stderr, "[ FAIL ] (Map_get(map, %u) = 0x%lx) != 0x%lx\n", i, Number_unsignedLongValue(number_get), Long_unsignedLongValue(&number[i-6139]));
			return false;
		}
	}

	init_Integer(&idx[i-6139], i);
	number_get = Number_staticCast(Map_get(map, Object_cast(&idx[i-6139])));
	if (!isNull(&number_get)) {
		fprintf(stderr, "[ FAIL ] (Map_get(map, %u) != null)\n", i);
		return false;
	}
	return true;
}
