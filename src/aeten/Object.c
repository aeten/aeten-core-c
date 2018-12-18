#include "Object.h"

#define import
#include "Hash.h"

/*
@startuml
namespace aeten {
	interface Object<T extends Object> {
		# finalize() <<default>>
		+ delete() <<default>>
		+ uint64_t hashCode() <<default>>
		+ bool equals(Object other) <<default>>

		+ {static} bool isObject(void* pointer)
		+ {static} bool isNull(Object object)
		+ {static} null: Object <<final>>
	}
}
@enduml
*/

#define DEFAULT_HASH_BITS 7

struct instance {
	aeten_Object object;
	bool _isInStack_;
};

__attribute__((constructor))
static void Object_Class_constructor(void) {
	READONLY_INIT(aeten_Object_class.null, {0});
}

void Object_finalize(Object* self) {}
void Object_delete(Object* self) {
	aeten_Object_finalize(*self);
	if ((self->_object_ != NULL) && ((struct instance*)self->_object_)->_isInStack_ == false) {
		free(self->_object_);
	}
}

bool Object_equals(Object *self, Object other) {
	return (self->_object_ == other._object_);
}

bool Object_isNull(Object object) {
	return ((object._object_ == NULL) || (object._class_ == NULL));
}

uint64_t Object_hashCode(Object *self) {
	return Hash_hashPointer(self->_object_, DEFAULT_HASH_BITS);
}

bool Object_isObject(void *pointer) {
	return ((pointer != NULL) &&
	        ((void*)((Object*)pointer)->_class_ >= _aeten_class_min_) &&
	        ((void*)((Object*)pointer)->_class_ <= _aeten_class_max_) &&
	        (((Object*)pointer)->_class_->_mark_ == _AETEN_MARK));
}
