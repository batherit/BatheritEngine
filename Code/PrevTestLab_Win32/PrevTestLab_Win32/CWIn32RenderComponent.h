#pragma once
#include "CWin32GDI.h"
#include "CMathUtil.h"
#include "CRenderComponent.h"

class CWIn32RenderComponent : public CRenderComponent
{
public:
	CWIn32RenderComponent(std::vector<CVector2D> meshVB) : meshVB_(meshVB) {}
	~CWIn32RenderComponent();

	void Render(void);

private:
	std::vector<CVector2D> meshVB_;
};