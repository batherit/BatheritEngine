#pragma once
#include"CGameComponent.h"
#include"CGameState.h"
#include<cassert>
#include<string>

template<class entity_type>
class CGameStateMachine : public CGameComponent
{
public:
	CGameStateMachine(CGameObject& r_owner)
		:
		CGameComponent(r_owner),
		p_current_state_(nullptr),
		p_previous_state_(nullptr),
		p_global_state_(nullptr)
	{};
	~CGameStateMachine() {};

	void SetCurrentState(CGameState<entity_type>* p_state) { p_current_state_ = p_state; }
	void SetPreviousState(CGameState<entity_type>* p_state) { p_previous_state_ = p_state; }
	void SetGlobalState(CGameState<entity_type>* p_state) { p_global_state_ = p_state; }
	// FSM�� �����Ѵ�.
	void Update() const {
		if (p_global_state_) p_global_state_->Execute(r_owner_);
		if (p_current_state_) p_current_state_->Execute(r_owner_);
	}
	// ���ο� ���·� ��ȭ��Ų��.
	void ChangeState(CGameState<entity_type>* p_new_state) {
		assert(p_new_state && "<CGameStateMachine::ChangeState> : trying to change to a null state");
		
		p_previous_state_ = p_current_state_;
		p_current_state_->Exit(r_owner_);
		p_current_state_ = p_new_state;
		p_current_state_->Enter(r_owner_);
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

private:
	//entity_type & r_owner_;
	CGameState<entity_type>* p_current_state_;
	CGameState<entity_type>* p_previous_state_;
	CGameState<entity_type>* p_global_state_;
};

