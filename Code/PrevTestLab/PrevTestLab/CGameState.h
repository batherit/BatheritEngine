#pragma once
#include"CGameComponent.h"

template<class entity_type>
class CGameState : public CGameComponent
{
public:
	CGameState(CGameObject& r_owner) : CGameComponent(r_owner) {};
	virtual ~CGameState();

	virtual void Enter(entity_type&) = 0;
 	virtual void Execute(entity_type&) = 0;
	virtual void Exit(entity_type&) = 0;
};

