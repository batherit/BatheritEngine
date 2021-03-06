#pragma once
#include<cstring>
#include"CGameTelegram.h"
#include"CGameObjectManager.h"
#include"CTransformComponent.h"
#include"CGameWorld.h"

class CGameComponent;

// CGameObject는 상속을 받지 아니하여도 되지만
// 상태 컴포넌트를 소유하게 될 경우, 상속하여 상태를 나타내는
// 변수들을 선언하여야 합니다.
class CGameObject
{
public:
	CTransformComponent transform_;

	CGameObject(int ID) : 
		registered_components_num_(0),
		playground_(nullptr) { 
		SetID(ID);
		memset(components_, '\0', sizeof(components_));
		// 게임 객체는 생성되면 자동으로 게임 객체 관리자에 등록된다.
		GameObjectMgr->RegisterObject(this);
	}
	virtual ~CGameObject();

	int GetID() const { return ID_; }
	void RegisterComponent(CGameComponent* p_component);
	void SendMessage(const Telegram& r_msg);	// 외부/내부 메시지
	void Update(float elapsed_time);

private:
	CGameWorld* playground_;

	int ID_;						// 모든 엔티티들은 고유한 식별 번호를 갖는다.
	static int next_valid_ID_;		// 이것은 유효한 다음 식별 번호이다.
	
	static const int MAX_COMPONENTS = 10;
	CGameComponent* components_[MAX_COMPONENTS];
	int registered_components_num_;

	void SetID(int ID);
};