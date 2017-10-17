#include "Object.h"

/*
@startuml
namespace aeten {
	interface Object<T> {
		# finalize() <<default>>
		+ delete() <<default>>
	}
}
@enduml
*/

void aeten_Object_finalize(aeten_Object* self) {}
void aeten_Object_delete(aeten_Object* self) {
	self->finalize(self);
	free(self);
}

