#pragma once
class CGameComponent;

class CGameObject
{
public:
	CGameObject(int ID) { SetID(ID); }
	virtual ~CGameObject();

	int GetID() const { return ID_; }
	void SendMessage(int message);	// 외부/내부 메시지
	void Update(float elapsed_time);

private:
	int ID_;						// 모든 엔티티들은 고유한 식별 번호를 갖는다.
	static int next_valid_ID_;		// 이것은 유효한 다음 식별 번호이다.

	static const int MAX_COMPONENTS = 10;
	CGameComponent* components_[MAX_COMPONENTS];

	void SetID(int ID);
};