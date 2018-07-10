#pragma once
#include <set>
#include"CGameTelegram.h"

#define MSG_DISPATCHER	CGameMessageDispatcher::Instance()
class CGameObject;

class CGameMessageDispatcher
{
public:
	static CGameMessageDispatcher* Instance();
	
	// �ٸ� ������Ʈ���� ���۹��� �޽����� �����Ͽ� ť�� ��� �޼����̴�.
	void DispatchMessageToQueue(float delay, int sender_ID, int receiver_ID, MESSAGE_CONTENT msg, void* p_extra_info);
	
	// ť���� delay��ŭ�� �ð��� ����� �޽����� �����ϴ� �޼����̴�.
	// �� ���� �������� ȣ��ȴ�. // ���� ������ ��ũ ������Ʈ���� ȣ��� ��.
	// Discharge �޼��尡 ȣ��ȴ�.
	void DispatchDelayedMessages(float delay, int sender_ID, int receiver_ID, MESSAGE_CONTENT msg, void* p_extra_info);

private:
	CGameMessageDispatcher() {}
	CGameMessageDispatcher(const CGameMessageDispatcher&);
	CGameMessageDispatcher& operator=(const CGameMessageDispatcher&);

	void Discharge(CGameObject* p_receiver, const Telegram& r_t);
	
	std::set<Telegram> message_queue_;	// �޽��� ť�μ� �ڷ��׷� ����Ʈ ������ ���Ѵ�.
};

