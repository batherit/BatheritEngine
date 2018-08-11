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

	void OnMouseDown(WPARAM btnState, int x, int y);

private:
	CGameWorld* p_game_world_;
	CTestPerson* p_test_object_;
};

inline void CTestFramework::Init(int cx, int cy) {
	
	p_test_object_ = new CTestPerson(0,
		CVector2D(0.0f, 0.0f),					// velocity
		VehiclePrm.max_speed_,					// max_speed
		CVector2D(1.0f, 1.0f),					// look
		VehiclePrm.vehicle_mass_,				// mass
		VehiclePrm.max_turn_rate_per_second_,	// max_turn_rate
		VehiclePrm.max_steering_force_			// max_force
	);
	p_test_object_->Steering()->SeekOn();
	p_test_object_->transform_.scale_ = CVector2D(50.0f, 50.0f);
	
	// »óÅÂ ±â°è ÄÄÆ÷³ÍÆ® ÀåÂø
	p_test_object_->RegisterComponent(
		new CGameStateMachine<CTestPerson>(IDLE_STATE, GLOBAL_STATE));

	CGameMesh* p_mesh = new CGameMesh();
	p_mesh->SetMeshVB(VehicleVB());
	p_mesh->SetBoundingRad(1.5f);
	p_test_object_->SetMesh(p_mesh);
	p_test_object_->RegisterRenderComponent(new CWIn32RenderComponent());

	p_game_world_ = new CGameWorld(cx, cy);
	std::vector<CGameObject*> agents;
	agents.push_back(p_test_object_);
	p_game_world_->RegisterAgents(agents);
}

inline void CTestFramework::Update(float tick) {
	FMOD->Instance();
	if (p_game_world_) {
		p_game_world_->Update(tick);
	}
	else {
		p_test_object_->Update(tick);
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

inline void CTestFramework::OnMouseDown(WPARAM btnState, int x, int y) {
	if (p_game_world_) p_game_world_->SetPickPoint(CVector2D(x, y));
}