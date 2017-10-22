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

void finalize(aeten_Object* self) {}
void delete(aeten_Object* self) {
	self->finalize(self);
	free(self);
}

