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

void _finalize(aeten_Object* self) {}
void _delete(aeten_Object* self) {
	self->finalize(self);
	free(self);
}

