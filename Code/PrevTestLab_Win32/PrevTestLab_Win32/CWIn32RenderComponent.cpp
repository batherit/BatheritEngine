#include "CWIn32RenderComponent.h"
#include "CWin32GDI.h"


CWIn32RenderComponent::~CWIn32RenderComponent()
{
}

void CWIn32RenderComponent::Render(void) {
	static std::vector<CVector2D> vec_vehicleVB_trans;

	vec_vehicleVB_trans = WorldTransform(meshVB_,
		p_owner_->transform_.pos_,
		p_owner_->transform_.look_,
		p_owner_->transform_.right_,
		p_owner_->transform_.scale_);

	gdi->ClosedShape(vec_vehicleVB_trans);
}
