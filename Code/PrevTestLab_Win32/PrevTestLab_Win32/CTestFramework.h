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
	p_test_object_->Steering()->ObstacleAvoidanceOn();
	p_test_object_->transform_.scale_ = CVector2D(50.0f, 50.0f);
	
	// 상태 기계 컴포넌트 장착
	p_test_object_->RegisterComponent(
		new CGameStateMachine<CTestPerson>(IDLE_STATE, GLOBAL_STATE));

	CGameMesh* p_mesh = new CGameMesh();
	p_mesh->SetMeshVB(VehicleVB());
	p_mesh->SetBoundingRad(20.5f);
	p_test_object_->SetMesh(p_mesh);
	p_test_object_->RegisterRenderComponent(new CWIn32RenderComponent());

	p_game_world_ = new CGameWorld(cx, cy);
	std::vector<CGameObject*> agents;
	agents.push_back(p_test_object_);
	p_game_world_->RegisterAgents(agents);

	// 장애물을 생성한다.
	// 랜덤 사이즈의 장애물들을 생성한다.
	std::vector<CGameObject*> obstacles;
	for (int o = 0; o < VehiclePrm.num_obstacles_; ++o)
	{
		bool is_overlapped = true;

		//keep creating tiddlywinks until we find one that doesn't overlap
		//any others.Sometimes this can get into an endless loop because the
		//obstacle has nowhere to fit. We test for this case and exit accordingly
		// 장애물이 다른 어떤 것에 겹쳐지기 전까지는 장애물을 계속해서 생성한다.
		// 장애물이 어떤 곳에서도 설치될 수 없을 수 있기 때문에, 때때로 끝없는 루프에 진입할 수 있다.
		// 이런 사례를 테스트하고 그렇다면 탈출하도록 한다.
		// 해당 코드에서는 최대 2000번을 수행하도록 한다. (?)
		int num_trys = 0; int num_allowable_trys = 2000;

		while (is_overlapped)
		{
			num_trys++;

			if (num_trys > num_allowable_trys) return;

			int radius = RandInt((int)VehiclePrm.min_obstacle_radius_, (int)VehiclePrm.max_obstacle_radius_);

			const int border = 10;
			const int min_gap_between_obstacles = 20;

			CGameObject* ob = new CGameObject(1 + o);
			ob->transform_.pos_.x_ = RandInt(radius + border, cx - radius - border);
			ob->transform_.pos_.y_ = RandInt(radius + border, cy - radius - 30 - border);
			ob->RegisterRenderComponent(new CWIn32RenderComponent());
			CGameMesh* p_mesh = new CGameMesh();
			p_mesh->SetMeshVB(ObstacleVB(radius));
			p_mesh->SetBoundingRad(radius);
			ob->SetMesh(p_mesh);

			if (!Overlapped(ob, obstacles, min_gap_between_obstacles))
			{
				// 이것이 다른 장애물과 겹쳐지지 않았기에 추가한다.
				obstacles.push_back(ob);

				is_overlapped = false;
			}

			else
			{
				delete ob;
			}
		}
	}
	p_game_world_->RegisterObstacles(obstacles);
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