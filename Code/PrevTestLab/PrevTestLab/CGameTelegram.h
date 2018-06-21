#pragma once
#include<iostream>
#include<cmath>

#define SMALLEST_DELAY_TIME	0.25f	// �������� �޽��� �߻��� ���̴� ������ �� ���� ���� ���� ���̴�.
enum MESSAGE_CONTENT {};		// �������� ������ "CGameMessageContent.h"�� ����

struct Telegram {
	int sender_ID;			// �۽��� ���̵�
	int receiver_ID;		// ������ ���̵�
	MESSAGE_CONTENT msg;	// �޽��� ����
	float dispatch_time;	// �۽� ���� �ð�
	void* extra_info;		// �߰������� ���� ������ (������ ������ cast)

	Telegram()
		: 
		sender_ID(-1),
		receiver_ID(-1),
		msg(static_cast<MESSAGE_CONTENT>(-1)),
		dispatch_time(-1),
		extra_info(nullptr) {
	}

	// �������� ������ ������ �� �����ڵ� ���� �����ȴ�.
	Telegram(
		int sender_ID,
		int receiver_ID,
		MESSAGE_CONTENT msg,
		float dispatch_time,
		void* extra_info = nullptr)
		:
		sender_ID(sender_ID),
		receiver_ID(receiver_ID),
		msg(msg),
		dispatch_time(dispatch_time),
		extra_info(extra_info) {

	}
};

inline bool operator==(const Telegram& r_t1, const Telegram& r_t2) {
	return (fabs(r_t1.dispatch_time - r_t2.dispatch_time) < SMALLEST_DELAY_TIME) &&
		(r_t1.sender_ID == r_t2.sender_ID) &&
		(r_t1.receiver_ID == r_t2.receiver_ID) &&
		(r_t1.msg == r_t2.msg);
}

// MessageDispatcher �� �����ϴ� �켱����ť ���� ���
inline bool operator<(const Telegram& r_t1, const Telegram& r_t2) {
	if (r_t1 == r_t2) {		// �۽� ���� �ð��� ����� ���� ������
		return false;
	}
	else {
		// �ð��� ���� ������ �̷�������� �Ѵ�. => �켱 ���� ť����
		return (r_t1.dispatch_time < r_t2.dispatch_time);
	}
}

// Telegram ��ü�� ������ Ȯ���ϱ� ���� ����, std::cout << (Telegram ��ü) ��.
inline std::ostream& operator<<(std::ostream& os, const Telegram& r_t) {
	os << "Time: " << r_t.dispatch_time << "  Sender: " << r_t.sender_ID
		<< "   Receiver: " << r_t.receiver_ID << "   Message: " << r_t.msg;

	return os;
}

// void* extra_info�� cast�ϱ� ���� �ζ��� �Լ�
template<class T>
inline T DereferenceToType(void* p) {
	return *(T*)(p);
}