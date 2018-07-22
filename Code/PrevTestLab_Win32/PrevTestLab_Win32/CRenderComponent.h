#pragma once
#include <vector>
#include "CGameObject.h"
#include "CGameComponent.h"

class CRenderComponent : public CGameComponent
{
public:
	CRenderComponent() {}
	~CRenderComponent();

	void Receive(const Telegram& r_msg) {}
	void Update(float tick) {}

	virtual void Render(void) = 0;
};

