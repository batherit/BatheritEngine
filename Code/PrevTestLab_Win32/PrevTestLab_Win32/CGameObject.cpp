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
	// �ǵ�� ���� : CGameObject�� ������ ��� ������Ʈ�� 
	// �޽����� ������ ���� �����Դϴ�.
	// 1. �� ������Ʈ�� CGameObject ��ü�� ������ �� �־�� �մϴ�. (Ex. �Ű������� ����.)
	// 2. �� ������Ʈ�� �ش� �޽����� ó���ϱ� ���� �ڵ鷯�� ����� �մϴ�.
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