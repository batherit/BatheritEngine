#pragma once
#include<vector>
#include"CWall2D.h"
#include"CVector2D.h"
#include"CObjectFuncTemplates.h"

class CGameObject;
class Obstacle;
class Path;

typedef std::vector<CGameObject*>::iterator		ObIt;

class CGameWorld
{
public:
	CGameWorld(int cx, int cy);
	~CGameWorld();

	void RegisterAgents(std::vector<CGameObject*>& agents);
	void ReleaseAgents(void);

	int Width(void) { return client_width_; }
	int Height(void) { return client_height_; }

	void Update(float tick);
	void Render();

	void  TagVehiclesWithinViewRange(CGameObject* pVehicle, float range) {
		TagNeighbors(pVehicle, agents_, range);
	}

	void  TagObstaclesWithinViewRange(CGameObject* pVehicle, float range) {
		TagNeighbors(pVehicle, obstacles_, range);
	}

	const std::vector<CWall2D>&			Walls() { return walls_; }
	const std::vector<CGameObject*>&	Obstacles()const { return obstacles_; }
	const std::vector<CGameObject*>&	Agents() { return agents_; }

	// WM_COMMAND 메시지들을 핸들한다.
	CVector2D PickPoint() const { return pick_point_; }
	void SetPickPoint(POINTS p);
	void SetPickPoint(CVector2D v) { pick_point_ = v; }

	int ClientWidth()const { return client_width_; }
	int ClientHeight()const { return client_height_; }

private:
	void CreateObstacles() {}
	void CreateWalls() {}

	std::vector<CGameObject*>	agents_;
	std::vector<CGameObject*>	obstacles_;
	std::vector<CWall2D>		walls_;

	Path*						p_path_;
	bool						b_paused_;
	int							client_width_, client_height_;
	CVector2D					pick_point_;
};
