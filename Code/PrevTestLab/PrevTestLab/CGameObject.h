#pragma once
#include<cstring>
class CGameComponent;

// CGameObject�� ����� ���� �ƴ��Ͽ��� ������
// ���� ������Ʈ�� �����ϰ� �� ���, ����Ͽ� ���¸� ��Ÿ����
// �������� �����Ͽ��� �մϴ�.
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
	void SendMessage(int message);	// �ܺ�/���� �޽���
	void Update(float elapsed_time);

private:
	int ID_;						// ��� ��ƼƼ���� ������ �ĺ� ��ȣ�� ���´�.
	static int next_valid_ID_;		// �̰��� ��ȿ�� ���� �ĺ� ��ȣ�̴�.

	static const int MAX_COMPONENTS = 10;
	CGameComponent* components_[MAX_COMPONENTS];
	int registered_components_num_;

	void SetID(int ID);
};