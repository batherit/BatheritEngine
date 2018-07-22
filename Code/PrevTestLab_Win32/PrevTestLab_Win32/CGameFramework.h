#pragma once
#include"CUtil.h"
#include"CWin32GDI.h"
#include"CGameTimer.h"
#include"CGameWorld.h"
#include"CGameObject.h"

class CGameFramework
{
public:
	CGameFramework() { main_timer_ = new CGameTimer(TIMER_TYPE_WINDOWS); }
	virtual ~CGameFramework() { if (!main_timer_) delete main_timer_;  }

	virtual void Init(int cx = 1920, int cy = 1080) = 0;
	void RunWithLoop(void) { 
		while (true) {
			main_timer_->RunTick();
			Update(main_timer_->GetElapsedTimePerFrame());
			Render(main_timer_->GetElapsedTimePerFrame());
		}
	}
	void Run(void) {
		main_timer_->RunTick();
		Update(main_timer_->GetElapsedTimePerFrame());
		Render(main_timer_->GetElapsedTimePerFrame());
	}
	
	virtual void Update(float tick) = 0;
	virtual void Render(float tick) = 0;
	virtual void Destroy(void) { delete this; }

protected:
	CGameTimer* main_timer_;
};