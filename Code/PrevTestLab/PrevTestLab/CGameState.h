#pragma once
#include"CGameComponent.h"

struct Telegram;
template<class entity_type>
class CGameStateMachine;

template<class entity_type>
class CGameState
{
	template<class entity_type>
	friend class CGameStateMachine;

public:
	CGameState() : p_owner_(nullptr), p_state_machine_(nullptr) {}
	virtual ~CGameState() {}

	virtual void Enter(void) = 0;
 	virtual void Execute(void) = 0;
	virtual void Exit(void) = 0;

	virtual bool OnMessage(entity_type*, const  Telegram&) = 0;

protected:
	entity_type * p_owner_;
	CGameStateMachine<entity_type>* p_state_machine_;
};

