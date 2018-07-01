#pragma once
#include"CGameState.h"
#include"CGameStateMachine.h"

#define IDLE_STATE IDLE::Instance()
#define GLOBAL_STATE CTestPersonGlobalState::Instance()

class CTestPerson;

class CTestPersonGlobalState : public CGameState<CTestPerson> {
public:
	static CTestPersonGlobalState* Instance();

	void Enter() {}
	void Execute();
	void Exit() {}

	bool OnMessage(CTestPerson* p_owner, const Telegram& msg) { return false; }

private:
	CTestPersonGlobalState() {}
	CTestPersonGlobalState(const CTestPersonGlobalState&);
	CTestPersonGlobalState& operator=(const CTestPersonGlobalState&);
};

class LookAround : public CGameState<CTestPerson> {
public:
	static LookAround* Instance();

	void Enter();
	void Execute();
	void Exit();

	bool OnMessage(CTestPerson* p_owner, const Telegram& msg) { return false; }

private:
	LookAround() {}
	LookAround(const LookAround&);
	LookAround& operator=(const LookAround&);
};

class IDLE : public CGameState<CTestPerson> {
public:
	static IDLE* Instance();

	void Enter();
	void Execute();
	void Exit();

	bool OnMessage(CTestPerson* p_owner, const Telegram& msg) { return false; }

private:
	IDLE() {}
	IDLE(const IDLE&);
	IDLE& operator=(const IDLE&);
};