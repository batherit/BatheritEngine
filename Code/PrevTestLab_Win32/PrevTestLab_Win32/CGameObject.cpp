#include<cassert>
#include "CGameObject.h"
#include "CGameComponent.h"
#include "CRenderComponent.h"


int CGameObject::next_valid_ID_ = 0;

CGameObject::~CGameObject()
{
}

void CGameObject::RegisterComponent(CGameComponent* p_component) {
	if (registered_components_num_ < MAX_COMPONENTS) {
		p_component->SetOwner(this);
		components_[registered_components_num_++] = p_component;
	}
}

void CGameObject::RegisterRenderComponent(CRenderComponent* p_component) {
	p_component->SetOwner(this);
	renderer_ = p_component;
}

void CGameObject::SendMessageToComponents(const Telegram& r_msg) {
	// 피드백 루프 : CGameObject에 설정된 모든 컴포넌트에 
	// 메시지를 보내기 위한 루프입니다.
	// 1. 각 컴포넌트는 CGameObject 객체에 접근할 수 있어야 합니다. (Ex. 매개변수로 전달.)
	// 2. 각 컴포넌트는 해당 메시지를 처리하기 위한 핸들러를 지녀야 합니다.
	for (int i = 0; i < MAX_COMPONENTS; i++) {
		if (components_[i] != nullptr) {
			components_[i]->Receive(r_msg);
		}
	}
}

void CGameObject::Update(float elapsed_time) {
	for (int i = 0; i < MAX_COMPONENTS; i++) {
		if (components_[i] != nullptr) {
			components_[i]->Update(elapsed_time);
		}
	}
}

void CGameObject::Render(void) {
	if(renderer_) renderer_->Render();
}

void CGameObject::SetID(int ID) {
	assert((ID >= next_valid_ID_) && "<CGameObject::SetID> : invalid ID");
	ID_ = ID;
	next_valid_ID_ = ID_ + 1;
}