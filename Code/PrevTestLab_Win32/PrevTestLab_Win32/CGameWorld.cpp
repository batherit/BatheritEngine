#include "CGameWorld.h"
#include "CGameObject.h"

CGameWorld::CGameWorld(int cx, int cy) :
	client_width_(cx),
	client_height_(cy),
	pick_point_(CVector2D(ClientWidth() / 2.0f, ClientHeight() / 2.0f))
{
}


CGameWorld::~CGameWorld()
{
}

void CGameWorld::Update(float tick) {
	for (unsigned int a = 0; a< agents_.size(); ++a)
	{
		agents_[a]->Update(tick);
	}
}

void CGameWorld::Render() {
	//render the agents
	for (unsigned int a = 0; a < agents_.size(); ++a)
	{
		agents_[a]->Render();
	}
}

void CGameWorld::RegisterAgents(std::vector<CGameObject*>& agents) {
	for (int i = 0; i < agents.size(); i++) {
		agents[i]->EnterToGameWorld(this);
	}
	agents_ = agents;
}
void CGameWorld::ReleaseAgents(void) {
	for (int i = 0; i < agents_.size(); i++) {
		agents_[i]->ExitFromGameWorld();
	}
	agents_.clear();
}

void CGameWorld::SetPickPoint(POINTS p) {
	CVector2D pick_point((float)p.x, (float) p.y);

	// TODO : 해당 픽 지점이 장애물 안에 없다는 것을 보장하는 코드 삽입

	pick_point_ = pick_point;
}