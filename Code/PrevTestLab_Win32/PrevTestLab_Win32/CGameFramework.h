#pragma once
#include"CUtil.h"
#include"CWin32GDI.h"
#include"CGameTimer.h"
#include"CGameWorld.h"
#include"CGameObject.h"

class CGameFramework
{
public:
	CGameFramework() { }
	virtual ~CGameFramework() { }

	virtual void Init(int cx = 1920, int cy = 1080) = 0;
	void RunWithLoop(void) { 
		while (true) {
			WorldTimer->RunTick();
			Update(WorldTimer->GetElapsedTimePerFrame());
			Render(WorldTimer->GetElapsedTimePerFrame());
		}
	}
	void Run(void) {
		WorldTimer->RunTick();
		Update(WorldTimer->GetElapsedTimePerFrame());
		Render(WorldTimer->GetElapsedTimePerFrame());
	}
	
	// 외부에서 월드 타이머를 세팅해주어야 합니다.
	virtual void Update(float tick) = 0;
	virtual void Render(float tick) = 0;
	virtual void Destroy(void) { delete this; }

	// 윈도우 키 입력 처리 메서드
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
};