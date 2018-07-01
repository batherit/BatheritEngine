#pragma once
#include "CGameFramework.h"
#include "CTestPerson.h"
#include "CTestPersonStates.h"

class CTestFramework : public CGameFramework
{
public:
	CTestFramework() :
		p_game_world_(nullptr),
		p_test_object_(nullptr) {}
	~CTestFramework() { Release(); };

	void Init(int cx, int cy);
	
private:
	void Update(float tick);
	void Render(float tick);
	void Release(void) { if (!p_test_object_) delete p_test_object_; }

	CGameWorld* p_game_world_;
	CTestPerson* p_test_object_;
};

inline void CTestFramework::Init(int cx, int cy) {
	p_game_world_ = new CGameWorld(cx, cy);
	p_test_object_ = new CTestPerson(0);
	p_test_object_->RegisterComponent(
		new CGameStateMachine<CTestPerson>(IDLE_STATE, GLOBAL_STATE));
}

inline void CTestFramework::Update(float tick) {
	std::cout << tick << std::endl;
	p_test_object_->Update(tick);
}

inline void CTestFramework::Render(float tick) {
	// TODO : p_test_obejct->Render(tick);
}
