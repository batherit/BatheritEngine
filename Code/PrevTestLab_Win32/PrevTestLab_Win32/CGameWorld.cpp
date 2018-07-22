#include "CGameWorld.h"
#include "CGameObject.h"

CGameWorld::CGameWorld(int cx, int cy) :
	client_width_(cx),
	client_height_(cy)
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