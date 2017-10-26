#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#include "MapTest.h"

#define import
#include "HashMap.h"
#include "Map.h"
#include "test/Testable.h"

/*!
@startuml
!include aeten/test/Testable.c
!include aeten/Map.c
namespace aeten {
	class MapTest implements aeten.test.Testable {
		{static} + MapTest(Map* map) <<constructor>>

		{static} - counter: unsigned
		- map: Map*
	}
} 
@enduml
*/

#define MAP(map, ...) { #map, new_##map( __VA_ARGS__ ) }
struct test_map {
	char *name;
	Map  *map;
};

int main(int argc, char** argv) {
	_counter=0;
	bool result;

	struct test_map map[] = { MAP(HashMap, sizeof(uint32_t), sizeof(uint64_t)) };

	for (uint32_t i=0; i<(sizeof(map)/sizeof(struct test_map)); ++i) {
		Testable* test = new_MapTest(map[i].map);
		result = Testable_test(test);
		if (!result) {
			printf("[ FAIL ]");
			++_counter;
		} else {
			printf("[SUCCES]");
		}
		printf(" Test %s of %s\n", argv[0], map[i].name);
		Map_finalize(map[i].map);
	}
	return _counter;
}

void _new(MapTest *self, Map *map) {
	self->_map = map;
}

static bool _test(MapTest* self) {
	Map *map = self->_map;
	uint64_t number;
	uint64_t *number_get;
	uint32_t i;
	long values[10];
	for (i=0; i<1024; ++i) {
		number = 0xFFFFFFFF00000000;
		number += (uint64_t)i;
		Map_put(map, &i, &number);
		number_get = (uint64_t*)Map_get(map, &i);
		if (number_get == NULL) {
			fprintf(stderr, "[ FAIL ] (Map_get(map, %u) = null)\n", i);
			return false;
		}
		if (*number_get != number) {
			fprintf(stderr, "[ FAIL ] (Map_get(map, %u) = 0x%lx) != 0x%lx\n", i, *number_get, number);
			return false;
		}
	}

	number_get = (uint64_t*)Map_get(map, &i);
	if (number_get != NULL) {
		fprintf(stderr, "[ FAIL ] (Map_get(map, %u) != null)\n", i);
		return false;
	}
	return true;
}
