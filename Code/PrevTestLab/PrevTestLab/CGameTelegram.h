#pragma once
#include<iostream>
#include<cmath>

#define SMALLEST_DELAY_TIME	0.25f	// 고주파의 메시지 발생을 보이는 게임은 더 작은 값을 가질 것이다.
enum MESSAGE_CONTENT {};		// 실질적인 내용은 "CGameMessageContent.h"에 정의

struct Telegram {
	int sender_ID;			// 송신자 아이디
	int receiver_ID;		// 수신자 아이디
	MESSAGE_CONTENT msg;	// 메시지 내용
	float dispatch_time;	// 송신 지연 시간
	void* extra_info;		// 추가적으로 보낼 데이터 (수신자 측에서 cast)

	Telegram()
		: 
		sender_ID(-1),
		receiver_ID(-1),
		msg(static_cast<MESSAGE_CONTENT>(-1)),
		dispatch_time(-1),
		extra_info(nullptr) {
	}

	// 실질적인 전보의 내용이 이 생성자들 통해 생성된다.
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

// MessageDispatcher 내 존재하는 우선순위큐 정렬 방식
inline bool operator<(const Telegram& r_t1, const Telegram& r_t2) {
	if (r_t1 == r_t2) {		// 송신 지연 시간을 고려한 같음 연산자
		return false;
	}
	else {
		// 시간에 따른 정렬이 이루어지도록 한다. => 우선 순위 큐에서
		return (r_t1.dispatch_time < r_t2.dispatch_time);
	}
}

// Telegram 객체의 내용을 확인하기 위한 정의, std::cout << (Telegram 객체) 시.
inline std::ostream& operator<<(std::ostream& os, const Telegram& r_t) {
	os << "Time: " << r_t.dispatch_time << "  Sender: " << r_t.sender_ID
		<< "   Receiver: " << r_t.receiver_ID << "   Message: " << r_t.msg;

	return os;
}

// void* extra_info를 cast하기 위한 인라인 함수
template<class T>
inline T DereferenceToType(void* p) {
	return *(T*)(p);
}