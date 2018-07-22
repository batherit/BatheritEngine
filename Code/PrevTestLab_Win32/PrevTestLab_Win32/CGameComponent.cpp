#include "CGameComponent.h"
#include "CGameObject.h"

CGameComponent::~CGameComponent()
{
}

void CGameComponent::FeedbackToObject(const Telegram& r_msg) {
	if(p_owner_) p_owner_->SendMessageToComponents(r_msg);
}

void CGameComponent::SetOwner(CGameObject* p_owner) {
	p_owner_ = p_owner;
}