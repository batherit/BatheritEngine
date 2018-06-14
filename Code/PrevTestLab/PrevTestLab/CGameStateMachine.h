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
		// ���� ���´� ����Ǿ�� �Ѵ�.
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
	// FSM�� �����Ѵ�.
	void Update(float elapsed_time) {
		if (p_global_state_) p_global_state_->Execute();
		if (p_current_state_) p_current_state_->Execute();
	}
	// ���ο� ���·� ��ȭ��Ų��.
	void ChangeState(CGameState<entity_type>* p_new_state) {
		assert(p_new_state && "<CGameStateMachine::ChangeState> : trying to change to a null state");
		
		p_previous_state_ = p_current_state_;
		p_current_state_->Exit();
		p_current_state_ = p_new_state;
		p_current_state_->p_owner_= dynamic_cast<entity_type*>(p_owner_);
		p_current_state_->p_state_machine_ = this;
		p_current_state_->Enter();
	}
	// ���¸� ���� ���·� �ٽ� ��ȭ��Ų��.
	void RevertToPreviousState() {
		ChangeState(p_previous_state_);
	}

	// ������
	CGameState<entity_type>* CurrentState() const { return p_current_state_; }
	CGameState<entity_type>* GlobalState() const { return p_global_state_; }
	CGameState<entity_type>* PreviousState() const { return p_previous_state_; }

	// Ȯ����
	bool IsInState(const CGameState<entity_type>& r_state) const {
		if (typeid(*p_current_state_) == typeid(r_state)) return true;
		return false;
	}
	
	std::string GetNameOfCurrentState() const {
		std::string s(typeid(*p_current_state_).name());

		// ���ڿ����� 'class'�κ��� �����Ѵ�.
		if (s.size() > 5)
		{
			s.erase(0, 6);
		}

		return s;
	}

	void Receive(int message) {}

private:
	CGameState<entity_type>* p_current_state_;
	CGameState<entity_type>* p_previous_state_;
	CGameState<entity_type>* p_global_state_;
};

