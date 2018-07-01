#pragma once
#include<vector>
#include"CWall2D.h"
#include"CVector2D.h"

class CGameObject;
class Obstacle;
class Path;

typedef std::vector<CGameObject*>::iterator		ObIt;

class CGameWorld
{
public:
	CGameWorld(int cx, int cy);
	~CGameWorld();

	void Update(float tick);
	void Render();

	const std::vector<CWall2D>&			Walls() { return walls_; }
	// TODO : CellSpacePartition<Vehicle*>*       CellSpace(){return m_pCellSpace;}
	const std::vector<CGameObject*>&	Obstacles()const { return obstacles_; }
	const std::vector<CGameObject*>&	Agents() { return objects_; }

private:
	void CreateObstacles();
	void CreateWalls();

	std::vector<CGameObject*>	objects_;
	std::vector<CGameObject*>	obstacles_;
	std::vector<CWall2D>			walls_;
	// TODO : CellSpacePartition<Vehicle*>* m_pCellSpace;
	Path*						p_path_;
	bool						b_paused_;
	int							client_width_, client_height_;
	CVector2D					crosshair_;
};

