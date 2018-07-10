#pragma once
#include <set>
#include"CGameTelegram.h"

#define MSG_DISPATCHER	CGameMessageDispatcher::Instance()
class CGameObject;

class CGameMessageDispatcher
{
public:
	static CGameMessageDispatcher* Instance();
	
	// 다른 에이전트들이 전송받을 메시지를 생성하여 큐에 담는 메서드이다.
	void DispatchMessageToQueue(float delay, int sender_ID, int receiver_ID, MESSAGE_CONTENT msg, void* p_extra_info);
	
	// 큐에서 delay만큼의 시간이 경과된 메시지를 전송하는 메서드이다.
	// 매 게임 루프마다 호출된다. // 게임 프레임 워크 업데이트에서 호출될 것.
	// Discharge 메서드가 호출된다.
	void DispatchDelayedMessages(float delay, int sender_ID, int receiver_ID, MESSAGE_CONTENT msg, void* p_extra_info);

private:
	CGameMessageDispatcher() {}
	CGameMessageDispatcher(const CGameMessageDispatcher&);
	CGameMessageDispatcher& operator=(const CGameMessageDispatcher&);

	void Discharge(CGameObject* p_receiver, const Telegram& r_t);
	
	std::set<Telegram> message_queue_;	// 메시지 큐로서 텔레그램 리스트 정보를 지닌다.
};

