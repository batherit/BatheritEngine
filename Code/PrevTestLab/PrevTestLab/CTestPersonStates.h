#pragma once
#include"CGameState.h"

class CTestPerson;

class IDLE : public CGameState<CTestPerson> {
public:
	static IDLE* Instance();

	void Enter();
	void Execute();
	void Exit();

private:
	IDLE() {}
	IDLE(const IDLE&);
	IDLE& operator=(const IDLE&);
};