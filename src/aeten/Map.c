#include "Map.h"

/*
@startuml(id=Map)
!include Collection.c
!include Set.c
namespace aeten {
    interface Map<K extends Object,V extends Object> {
        + V get(K key)
        + V put(K key, V value) <<default>> <<optional>>
        + V remove(K key) <<default>> <<optional>>
		+ size_t size()
        + Collection<V> values()
        + Set<MapEntry<K,V>> entrySet()
        + Set<K> keySet()
	}
!ifdef aeten_Map
	note right of Map::get
		Returns the value to which the specified key is mapped, or null if this map contains no mapping for the key.
	end note
	note right of Map::put
		Associates the specified value with the specified key in this map (optional operation).
	end note
	note right of Map::remove
		Removes the mapping for a key from this map if it is present (optional operation).
	end note
!endif
}
@enduml
*/

aeten_Object Map_put(Map* map, aeten_Object key, aeten_Object value) {
	check(0, UnsupportedOperationException, "Map.put()");
	return (aeten_Object){0};
}

aeten_Object Map_remove(Map* map, aeten_Object key) {
	check(0, UnsupportedOperationException, "Map.remove()");
	return (aeten_Object){0};
}
