#pragma once
#include<cstring>
#include"CGameTelegram.h"
#include"CGameObjectManager.h"
#include"CTransformComponent.h"
#include"CGameWorld.h"

class CGameComponent;
class CRenderComponent;

// CGameObject�� ����� ���� �ƴ��Ͽ��� ������
// ���� ������Ʈ�� �����ϰ� �� ���, ����Ͽ� ���¸� ��Ÿ����
// �������� �����Ͽ��� �մϴ�.
class CGameObject
{
public:
	CTransformComponent transform_;

	CGameObject(int ID) : 
		registered_components_num_(0),
		playground_(nullptr) { 
		SetID(ID);
		memset(components_, '\0', sizeof(components_));
		// ���� ��ü�� �����Ǹ� �ڵ����� ���� ��ü �����ڿ� ��ϵȴ�.
		GameObjectMgr->RegisterObject(this);
	}
	virtual ~CGameObject();

	int GetID() const { return ID_; }
	void RegisterComponent(CGameComponent* p_component);
	void RegisterRenderComponent(CRenderComponent* p_component);

	void SendMessageToComponents(const Telegram& r_msg);	// �ܺ�/���� �޽���
	
	void EnterToGameWorld(CGameWorld* p_game_world) { playground_ = p_game_world; }
	void ExitFromGameWorld(void) { playground_ = nullptr; }
	CGameWorld* GameWorld(void) { return playground_; }

	void Update(float elapsed_time);
	void Render(void);

private:
	CGameWorld* playground_;

	int ID_;						// ��� ��ƼƼ���� ������ �ĺ� ��ȣ�� ���´�.
	static int next_valid_ID_;		// �̰��� ��ȿ�� ���� �ĺ� ��ȣ�̴�.
	
	CRenderComponent* renderer_;

	static const int MAX_COMPONENTS = 10;
	CGameComponent* components_[MAX_COMPONENTS];
	int registered_components_num_;

	void SetID(int ID);
};