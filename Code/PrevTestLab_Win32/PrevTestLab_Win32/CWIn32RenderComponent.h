#pragma once
#include "CWin32GDI.h"
#include "CMathUtil.h"
#include "CRenderComponent.h"

class CWIn32RenderComponent : public CRenderComponent
{
public:
	CWIn32RenderComponent(){}
	~CWIn32RenderComponent();

	void Render(void);
};