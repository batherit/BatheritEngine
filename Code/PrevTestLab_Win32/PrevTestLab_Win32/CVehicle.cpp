#include "CVehicle.h"
#include "CMathUtil.h"
#include "CSteeringBehaviors.h"

CVehicle::~CVehicle()
{
}

void CVehicle::Update(float time_elapsed) {
	CVector2D old_pos = p_owner_->transform_.pos_;				// ���� ��ġ ��ǥ
	CVector2D steering_force = p_steering_->Calculate();		// ���� �ൿ �� ���
	CVector2D acceleration = steering_force / mass_;			// ���ӵ� ���
	v_velocity_ += acceleration * time_elapsed;					// �ӵ� ���
	v_velocity_.Truncate(max_speed_);							// �Ҽ��� ������ �� ������
	p_owner_->transform_.pos_ += v_velocity_ * time_elapsed;	// ��ġ ����

	// �躤�Ϳ� ���̵庤�͸� �����Ѵ�.
	if (v_velocity_.LengthSq() > 0.00000001)
	{
		p_owner_->transform_.look_ = Vec2DNormalize(v_velocity_);

		p_owner_->transform_.right_ = p_owner_->transform_.look_.Perp();
	}

	//treat the screen as a toroid
	WrapAround(p_owner_->transform_.pos_, p_owner_->GameWorld()->Width(), p_owner_->GameWorld()->Height());
}