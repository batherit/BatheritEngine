#pragma once
#include"CGameTimer.h"
#include"CGameObject.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	void Run(void);
	void Release(void);

protected:
	void Update(void);
	void Render(void);

private:
	CGameTimer main_timer_;
};

