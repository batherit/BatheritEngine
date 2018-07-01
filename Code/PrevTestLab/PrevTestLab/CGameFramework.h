#pragma once
#include"CUtil.h"
#include"CGameTimer.h"
#include"CGameWorld.h"
#include"CGameObject.h"

class CGameFramework
{
public:
	CGameFramework() { main_timer_ = new CGameTimer(TIMER_TYPE_WINDOWS); }
	virtual ~CGameFramework() { Release(); }

	virtual void Init() = 0;
	void Run(void) { 
		while (true) {
			main_timer_->RunTick();
			Update(main_timer_->GetElapsedTimePerFrame());
			Render(main_timer_->GetElapsedTimePerFrame());
		}
	}
	
protected:
	virtual void Update(float tick) = 0;
	virtual void Render(float tick) = 0;

private:
	void Release(void) { if (!main_timer_) delete main_timer_; }
	
	CGameTimer* main_timer_;
};