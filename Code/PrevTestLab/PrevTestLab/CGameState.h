#pragma once
#include"CGameComponent.h"

template<class entity_type>
class CGameState
{
public:
	CGameState() : p_owner_(nullptr) {}
	virtual ~CGameState() {}

	virtual void Enter(void) = 0;
 	virtual void Execute(void) = 0;
	virtual void Exit(void) = 0;

protected:
	entity_type * p_owner_;
};

