#include "CGameComponent.h"
#include "CGameObject.h"

CGameComponent::~CGameComponent()
{
}

void CGameComponent::Send(int message) {
	if(p_owner_) p_owner_->SendMessage(message);
}

void CGameComponent::SetOwner(CGameObject* p_owner) {
	p_owner_ = p_owner_;
}