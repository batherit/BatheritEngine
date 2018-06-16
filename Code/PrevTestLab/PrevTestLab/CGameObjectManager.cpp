#include "CGameObjectManager.h"
#include "CGameObject.h"

CGameObjectManager* CGameObjectManager::Instance() {
	static CGameObjectManager instance;
	return &instance;
}

void CGameObjectManager::RegisterObject(CGameObject* p_new_object) {
	object_map_.insert(std::make_pair(p_new_object->GetID(), p_new_object));
}

CGameObject* CGameObjectManager::GetObjectFromID(int ID)const {
	ObjectMap::const_iterator ent = object_map_.find(ID);
	// 오브젝트가 맵의 멤버로 존재함을 단언한다.
	assert((ent != object_map_.end()) && "<CGameObjectManager::GetEntityFromID>: invalid ID");
	return ent->second;	// CGameObject*;
}

void CGameObjectManager::RemoveObject(CGameObject* p_object) {
	object_map_.erase(object_map_.find(p_object->GetID()));
}