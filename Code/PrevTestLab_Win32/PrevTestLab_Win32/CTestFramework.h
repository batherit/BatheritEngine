#pragma once
#include "CGameFramework.h"
#include "CWIn32RenderComponent.h"
#include "CVehicle.h"
#include "CTestPerson.h"
#include "CTestPersonStates.h"

class CTestFramework : public CGameFramework
{
public:
	CTestFramework() :
		p_game_world_(nullptr),
		p_test_object_(nullptr) {}
	~CTestFramework() { 
		if (!p_test_object_) { delete p_test_object_; }
		std::cout << "Destory" << std::endl;
	}

	void Init(int cx, int cy);

	void Update(float tick);
	void Render(float tick);

private:
	CGameWorld* p_game_world_;
	CTestPerson* p_test_object_;
};

inline void CTestFramework::Init(int cx, int cy) {
	
	p_test_object_ = new CTestPerson(0);
	p_test_object_->transform_.scale_ = CVector2D(50.0f, 50.0f);
	p_test_object_->RegisterComponent(
		new CGameStateMachine<CTestPerson>(IDLE_STATE, GLOBAL_STATE));
	p_test_object_->RegisterComponent(
		new CVehicle(
			20.0f,						// bounding_radius
			CVector2D(20.0f, 20.0f),	// velocity
			200.0f,						// max_speed
			CVector2D(1.0f, 1.0f),		// look
			30.0f,						// mass
			10.0f,						// max_turn_rate
			10.0f						// max_force
		));
	p_test_object_->RegisterRenderComponent(new CWIn32RenderComponent(VehicleVB()));

	p_game_world_ = new CGameWorld(cx, cy);
	std::vector<CGameObject*> agents;
	agents.push_back(p_test_object_);
	p_game_world_->RegisterAgents(agents);
}

inline void CTestFramework::Update(float tick) {
	main_timer_->RunTick();
	if (p_game_world_) {
		p_game_world_->Update(main_timer_->GetElapsedTimePerFrame());
	}
	else {
		p_test_object_->Update(main_timer_->GetElapsedTimePerFrame());
	}
}

inline void CTestFramework::Render(float tick) {
	if (p_game_world_) {
		p_game_world_->Render();
	}
	else {
		p_test_object_->Render();
	}
}
