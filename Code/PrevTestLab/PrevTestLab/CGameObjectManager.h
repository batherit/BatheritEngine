#pragma once
#include<map>
#include<cassert>
#include<string>

class CGameObject;

#define GameObjectMgr	CGameObjectManager::Instance()

class CGameObjectManager
{
public:
	static CGameObjectManager* Instance();
	void RegisterObject(CGameObject* p_new_object);
	CGameObject* GetObjectFromID(int ID)const;
	void RemoveObject(CGameObject* p_object);

private:
	typedef std::map<int, CGameObject*> ObjectMap;
	
	CGameObjectManager() {}
	CGameObjectManager(const CGameObjectManager&);
	CGameObjectManager& operator=(const CGameObjectManager&);

	ObjectMap object_map_;
};

