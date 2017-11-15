#include "Object.h"

#define import
#include "Hash.h"

/*
@startuml
namespace aeten {
	interface Object<T> {
		# finalize() <<default>>
		+ delete() <<default>>
		+ uint64_t hashCode() <<default>>
		+ bool equals(void* other) <<default>>
	}
}
@enduml
*/


#define DEFAULT_HASH_BITS 7

void Object_finalize(Object* self) {}
void Object_delete(Object* self) {
	Object_finalize(self);
	if (self->_isInStack == false) {
		free(self);
	}
}

bool Object_equals(Object *self, void *other) {
	check(other!=NULL, NullPointerException, "Object.equals(other), other is null");
	return self==other;
}

uint64_t Object_hashCode(Object *self) {
	return Hash_hashPointer(self, DEFAULT_HASH_BITS);
}


