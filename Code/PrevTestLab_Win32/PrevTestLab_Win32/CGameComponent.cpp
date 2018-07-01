#include "CGameComponent.h"
#include "CGameObject.h"

CGameComponent::~CGameComponent()
{
}

void CGameComponent::SendMessage(const Telegram& r_msg) {
	if(p_owner_) p_owner_->SendMessage(r_msg);
}

void CGameComponent::SetOwner(CGameObject* p_owner) {
	p_owner_ = p_owner_;
}