#pragma once
#include "CGameFramework.h"
#include "CTestPerson.h"
#include "CTestPersonStates.h"

class CTestFramework : public CGameFramework
{
public:
	CTestFramework() :
		p_test_object(nullptr) {}
	~CTestFramework() { Release(); };

	void Init(void);
	
private:
	void Update(float tick);
	void Render(float tick);
	void Release(void) { if (!p_test_object) delete p_test_object; }

	CTestPerson* p_test_object;
};

inline void CTestFramework::Init(void) {
	p_test_object = new CTestPerson(0);
	p_test_object->RegisterComponent(
		new CGameStateMachine<CTestPerson>(IDLE_STATE, GLOBAL_STATE));
}

inline void CTestFramework::Update(float tick) {
	std::cout << tick << std::endl;
	p_test_object->Update(tick);
}

inline void CTestFramework::Render(float tick) {
	// TODO : p_test_obejct->Render(tick);
}
