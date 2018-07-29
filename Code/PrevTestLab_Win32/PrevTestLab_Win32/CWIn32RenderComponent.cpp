#include "CWIn32RenderComponent.h"
#include "CWin32GDI.h"


CWIn32RenderComponent::~CWIn32RenderComponent()
{
}

void CWIn32RenderComponent::Render(void) {
	static std::vector<CVector2D> vec_vehicleVB_trans;

	if (p_owner_->Mesh()) {
		std::vector<CVector2D> meshVB = p_owner_->Mesh()->GetMeshVB();
		vec_vehicleVB_trans = WorldTransform(meshVB,
			p_owner_->transform_.pos_,
			p_owner_->transform_.look_,
			p_owner_->transform_.right_,
			p_owner_->transform_.scale_);

		gdi->ClosedShape(vec_vehicleVB_trans);
	}
}
