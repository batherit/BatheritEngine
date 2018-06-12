#pragma once
class CGameComponent;

class CGameObject
{
public:
	CGameObject(int ID) { SetID(ID); }
	virtual ~CGameObject();

	int GetID() const { return ID_; }
	void SendMessage(int message);	// �ܺ�/���� �޽���
	void Update(float elapsed_time);

private:
	int ID_;						// ��� ��ƼƼ���� ������ �ĺ� ��ȣ�� ���´�.
	static int next_valid_ID_;		// �̰��� ��ȿ�� ���� �ĺ� ��ȣ�̴�.

	static const int MAX_COMPONENTS = 10;
	CGameComponent* components_[MAX_COMPONENTS];

	void SetID(int ID);
};