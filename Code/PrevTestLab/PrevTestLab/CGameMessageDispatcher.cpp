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

	// ������ �����Ѵ�.
	Telegram telegram(sender_ID, receiver_ID, msg, 0, p_extra_info);

	if (delay <= 0.0f) {
		// TODO : ���� �ð��� ������ �� �ֵ��� Ÿ�̸� �޼��带 ������ �Ѵ�.
		/*cout << "\nInstant telegram dispatched at time: " << Clock->GetCurrentTime()
			<< " by " << GetNameOfEntity(pSender->ID()) << " for " << GetNameOfEntity(pReceiver->ID())
			<< ". Msg is " << MsgToStr(msg);*/

		// �����ڿ� ������ ������.
		Discharge(p_receiver, telegram);
	}
	else {
		//float current_time = WorldTimer->GetCurTime(); //TODO : ���� �ð��� ���ϴ� �޼��� �߰� �ʿ�.;
		//telegram.dispatch_time = current_time + delay;

		// �޽��� ť�� ��´�.
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

		// �����ڿ� ������ ������.
		Discharge(p_receiver, telegram);

		// ť�κ��� �ش� ������ �����Ѵ�.
		message_queue_.erase(message_queue_.begin());
	}
}

void CGameMessageDispatcher::Discharge(CGameObject* p_receiver, const Telegram& r_t) {
	p_receiver->SendMessage(r_t);
}