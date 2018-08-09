#pragma once

struct Telegram;
class CGameObject;

class CGameComponent
{
public:
	CGameComponent() : p_owner_(nullptr) {}
	virtual ~CGameComponent();

	void SetOwner(CGameObject* p_owner);
	virtual void Receive(const Telegram& r_msg) = 0;	// CGameObject�� ȣ��
	virtual void Update(float elapsed_time) = 0;		// SendMessage ȣ�� ����, CGameObject�� ȣ��

protected:
	void FeedbackToObject(const Telegram& r_msg);
	CGameObject* p_owner_;
};

