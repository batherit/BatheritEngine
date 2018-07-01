#include "CGameMessageDispatcher.h"
#include "CGameObject.h"
#include "CGameObjectManager.h"
using std::cout;
using std::set;

CGameMessageDispatcher* CGameMessageDispatcher::Instance() {
	static CGameMessageDispatcher instance;
	return &instance;
}

void CGameMessageDispatcher::DispatchMessage(float delay, int sender_ID, int receiver_ID, MESSAGE_CONTENT msg, void* p_extra_info) {
	CGameObject* p_sender	= GameObjectMgr->GetObjectFromID(sender_ID);
	CGameObject* p_receiver = GameObjectMgr->GetObjectFromID(receiver_ID);

	if (!p_receiver) {
		cout << "\nWarning! No Receiver with ID of " << receiver_ID << " found";
		return;
	}

	// 전보를 생성한다.
	Telegram telegram(sender_ID, receiver_ID, msg, 0, p_extra_info);

	if (delay <= 0.0f) {
		// TODO : 월드 시간을 가져올 수 있도록 타이머 메서드를 만들어야 한다.
		/*cout << "\nInstant telegram dispatched at time: " << Clock->GetCurrentTime()
			<< " by " << GetNameOfEntity(pSender->ID()) << " for " << GetNameOfEntity(pReceiver->ID())
			<< ". Msg is " << MsgToStr(msg);*/

		// 수신자에 전보를 보낸다.
		Discharge(p_receiver, telegram);
	}
	else {
		//float current_time = WorldTimer->GetCurTime(); //TODO : 현재 시간을 구하는 메서드 추가 필요.;
		//telegram.dispatch_time = current_time + delay;

		// 메시지 큐에 담는다.
		message_queue_.insert(telegram);

		/*cout << "\nDelayed telegram from " << GetNameOfEntity(pSender->ID()) << " recorded at time "
			<< Clock->GetCurrentTime() << " for " << GetNameOfEntity(pReceiver->ID())
			<< ". Msg is " << MsgToStr(msg);*/
	}
}

void CGameMessageDispatcher::DispatchDelayedMessages(float delay, int sender_ID, int receiver_ID, MESSAGE_CONTENT msg, void* p_extra_info) {
	// float current_time = WorldTimer->GetCurTime();

	while (!message_queue_.empty() &&
		/*message_queue_.begin()->dispatch_time < current_time	&&*/
		message_queue_.begin()->dispatch_time > 0) {
		const Telegram& telegram = *message_queue_.begin();
		CGameObject* p_receiver = GameObjectMgr->GetObjectFromID(telegram.receiver_ID);

		/*cout << "\nQueued telegram ready for dispatch: Sent to "
			<< GetNameOfEntity(pReceiver->ID()) << ". Msg is " << MsgToStr(telegram.Msg);*/

		// 수신자에 전보를 보낸다.
		Discharge(p_receiver, telegram);

		// 큐로부터 해당 전보를 제거한다.
		message_queue_.erase(message_queue_.begin());
	}
}

void CGameMessageDispatcher::Discharge(CGameObject* p_receiver, const Telegram& r_t) {
	p_receiver->SendMessage(r_t);
}