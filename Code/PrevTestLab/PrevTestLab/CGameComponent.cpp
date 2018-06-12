#include "CGameComponent.h"
#include "CGameObject.h"

CGameComponent::~CGameComponent()
{
}

void CGameComponent::Send(int message) {
	r_owner_.SendMessage(message);
}