#pragma once
#include<cstring>
class CGameComponent;

// CGameObject는 상속을 받지 아니하여도 되지만
// 상태 컴포넌트를 소유하게 될 경우, 상속하여 상태를 나타내는
// 변수들을 선언하여야 합니다.
class CGameObject
{
public:
	CGameObject(int ID) : registered_components_num_(0) { 
		SetID(ID);
		memset(components_, '\0', sizeof(components_));
	}
	virtual ~CGameObject();

	int GetID() const { return ID_; }
	void RegisterComponent(CGameComponent* p_component);
	void SendMessage(int message);	// 외부/내부 메시지
	void Update(float elapsed_time);

private:
	int ID_;						// 모든 엔티티들은 고유한 식별 번호를 갖는다.
	static int next_valid_ID_;		// 이것은 유효한 다음 식별 번호이다.

	static const int MAX_COMPONENTS = 10;
	CGameComponent* components_[MAX_COMPONENTS];
	int registered_components_num_;

	void SetID(int ID);
};