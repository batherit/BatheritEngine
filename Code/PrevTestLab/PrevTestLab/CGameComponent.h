#pragma once
class CGameObject;

class CGameComponent
{
public:
	CGameComponent(CGameObject& r_owner) : r_owner_(r_owner) {}
	virtual ~CGameComponent();
	void Send(int message);
	virtual void Receive(int message) = 0;	// 브로드 캐스트 방식 통신

private:
	CGameObject& r_owner_;
};

