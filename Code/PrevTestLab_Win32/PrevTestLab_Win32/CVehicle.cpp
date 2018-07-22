#include "CVehicle.h"
#include "CMathUtil.h"
#include "CSteeringBehaviors.h"

CVehicle::~CVehicle()
{
}

void CVehicle::Update(float time_elapsed) {
	CVector2D old_pos = p_owner_->transform_.pos_;				// 이전 위치 좌표
	CVector2D steering_force = p_steering_->Calculate();		// 조종 행동 힘 계산
	CVector2D acceleration = steering_force / mass_;			// 가속도 계산
	v_velocity_ += acceleration * time_elapsed;					// 속도 계산
	v_velocity_.Truncate(max_speed_);							// 소수점 이하의 수 버리기
	p_owner_->transform_.pos_ += v_velocity_ * time_elapsed;	// 위치 갱신

	// 룩벡터와 사이드벡터를 갱신한다.
	if (v_velocity_.LengthSq() > 0.00000001)
	{
		p_owner_->transform_.look_ = Vec2DNormalize(v_velocity_);

		p_owner_->transform_.right_ = p_owner_->transform_.look_.Perp();
	}

	//treat the screen as a toroid
	WrapAround(p_owner_->transform_.pos_, p_owner_->GameWorld()->Width(), p_owner_->GameWorld()->Height());
}