#pragma once
class CGameObject;

class CGameComponent
{
public:
	CGameComponent() : p_owner_(nullptr) {}
	virtual ~CGameComponent();
	void Send(int message);
	void SetOwner(CGameObject* p_owner);
	virtual void Receive(int message) = 0;	// ��ε� ĳ��Ʈ ��� ���
	virtual void Update(float elapsed_time) = 0;

protected:
	CGameObject* p_owner_;
};

