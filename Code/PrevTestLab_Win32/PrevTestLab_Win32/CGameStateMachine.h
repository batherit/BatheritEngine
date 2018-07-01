#pragma once
#include"CGameComponent.h"
#include"CGameState.h"
#include<cassert>
#include<string>

template<class entity_type>
class CGameStateMachine : public CGameComponent
{
public:
	CGameStateMachine(
		CGameState<entity_type>* p_current_state,
		CGameState<entity_type>* p_global_state = nullptr)
		:
		p_current_state_(p_current_state),
		p_previous_state_(nullptr),
		p_global_state_(p_global_state)
	{
		// 현재 상태는 보장되어야 한다.
		assert(p_current_state_ && "<CGameStateMachine::CGameStateMachine> : tring to set to null state");
		SetCurrentState(p_current_state);
		SetGlobalState(p_global_state);
	};
	~CGameStateMachine() {};

	void SetCurrentState(CGameState<entity_type>* p_state) { 
		p_current_state_ = p_state;
		p_current_state_->p_owner_ = dynamic_cast<entity_type*>(p_owner_);
		p_current_state_->p_state_machine_ = this;
	}
	void SetPreviousState(CGameState<entity_type>* p_state) { p_previous_state_ = p_state; }
	void SetGlobalState(CGameState<entity_type>* p_state) { 
		p_global_state_ = p_state;
		p_global_state_->p_owner_ = dynamic_cast<entity_type*>(p_owner_); 
		p_global_state_->p_state_machine_ = this;
	}
	// FSM을 갱신한다.
	void Update(float elapsed_time) {
		if (p_global_state_) p_global_state_->Execute();
		if (p_current_state_) p_current_state_->Execute();
	}
	// 새로운 상태로 변화시킨다.
	void ChangeState(CGameState<entity_type>* p_new_state) {
		assert(p_new_state && "<CGameStateMachine::ChangeState> : trying to change to a null state");
		
		p_previous_state_ = p_current_state_;
		p_current_state_->Exit();
		p_current_state_ = p_new_state;
		p_current_state_->p_owner_= dynamic_cast<entity_type*>(p_owner_);
		p_current_state_->p_state_machine_ = this;
		p_current_state_->Enter();
	}
	// 상태를 이전 상태로 다시 변화시킨다.
	void RevertToPreviousState() {
		ChangeState(p_previous_state_);
	}

	// 접근자
	CGameState<entity_type>* CurrentState() const { return p_current_state_; }
	CGameState<entity_type>* GlobalState() const { return p_global_state_; }
	CGameState<entity_type>* PreviousState() const { return p_previous_state_; }

	// 확인자
	bool IsInState(const CGameState<entity_type>& r_state) const {
		if (typeid(*p_current_state_) == typeid(r_state)) return true;
		return false;
	}
	
	std::string GetNameOfCurrentState() const {
		std::string s(typeid(*p_current_state_).name());

		// 문자열에서 'class'부분을 제거한다.
		if (s.size() > 5)
		{
			s.erase(0, 6);
		}

		return s;
	}

	void Receive(const Telegram& r_msg) {
		// 우선 현재 상태가 유효하고 메시지를 처리할 수 있는지를 알아본다.
		if (p_current_state_ && p_current_state_->OnMessage(dynamic_cast<entity_type*>(p_owner_), r_msg))
		{
			return;
		}

		// 처리할 수 없고 전역 상태가 설치되어 있다면, 메시지를 전역 상태로 보낸다.
		// => 어떤 상태에서든 반응해야만 하는 메시지이다.
		if (p_global_state_ && p_current_state_->OnMessage(dynamic_cast<entity_type*>(p_owner_), r_msg))
		{
			return;
		}

		return;
	}

private:
	CGameState<entity_type>* p_current_state_;
	CGameState<entity_type>* p_previous_state_;
	CGameState<entity_type>* p_global_state_;
};

