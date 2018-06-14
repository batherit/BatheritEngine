#pragma once
#include"CGameState.h"
#include"CGameStateMachine.h"

class CTestPerson;

class CTestPersonGlobalState : public CGameState<CTestPerson> {
public:
	static CTestPersonGlobalState* Instance();

	void Enter() {}
	void Execute();
	void Exit() {}

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

private:
	IDLE() {}
	IDLE(const IDLE&);
	IDLE& operator=(const IDLE&);
};