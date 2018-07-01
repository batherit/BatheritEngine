#pragma once
#include "CGameObject.h"

class CTestPerson : public CGameObject
{
public:
	CTestPerson();
	CTestPerson(int ID) : CGameObject(ID) {}
	~CTestPerson();

private:
	// 상태를 추가.
};

