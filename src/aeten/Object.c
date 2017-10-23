#include "Object.h"

#define import
#include "Hash.h"

/*
@startuml
namespace aeten {
	interface Object<T> {
		# finalize() <<default>>
		+ delete() <<default>>
		+ long hashCode() <<default>>
		+ bool equals(void* other) <<default>>
	}
}
@enduml
*/


#define DEFAULT_HASH_BITS 7

void _finalize(Object* self) {}
void _delete(Object* self) {
	self->finalize(self);
	free(self);
}

bool _equals(Object *self, void *other) {
	check(other!=NULL, NullPointerException, "Object.equals(other), other is null");
	return self==other;
}

long _hashCode(Object *self) {
	Hash_hashPointer(self, DEFAULT_HASH_BITS);
}


