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
	
	// �ܺο��� ���� Ÿ�̸Ӹ� �������־�� �մϴ�.
	virtual void Update(float tick) = 0;
	virtual void Render(float tick) = 0;
	virtual void Destroy(void) { delete this; }

	// ������ Ű �Է� ó�� �޼���
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
};