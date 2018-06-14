#pragma once
class CGameObject;

class CGameComponent
{
public:
	CGameComponent() : p_owner_(nullptr) {}
	virtual ~CGameComponent();
	void Send(int message);
	void SetOwner(CGameObject* p_owner);
	virtual void Receive(int message) = 0;	// 브로드 캐스트 방식 통신
	virtual void Update(float elapsed_time) = 0;

protected:
	CGameObject* p_owner_;
};

