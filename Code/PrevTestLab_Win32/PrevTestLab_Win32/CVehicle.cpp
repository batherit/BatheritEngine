#include "CVehicle.h"
#include "CMathUtil.h"
#include "CGameWorld.h"
#include "CSteeringBehaviors.h"

CVehicle::~CVehicle()
{
}

void CVehicle::Update(float time_elapsed) {
	CVector2D old_pos = transform_.pos_;				// 이전 위치 좌표
	CVector2D steering_force = p_steering_->Calculate();		// 조종 행동 힘 계산
	CVector2D acceleration = steering_force / physics_->Mass();			// 가속도 계산
	CVector2D velocity = physics_->Velocity();
	velocity += acceleration * time_elapsed; 					// 속도 계산
	velocity.Truncate(physics_->MaxSpeed());							// 소수점 이하의 수 버리기
	transform_.pos_ += velocity * time_elapsed;	// 위치 갱신

	// 룩벡터와 사이드벡터를 갱신한다.
	if (velocity.LengthSq() > 0.00000001)
	{
		transform_.look_ = Vec2DNormalize(velocity);

		transform_.right_ = transform_.look_.Perp();
	}

	physics_->SetVelocity(velocity);

	//treat the screen as a toroid
	WrapAround(transform_.pos_, GameWorld()->Width(), GameWorld()->Height());

	CGameObject::Update(time_elapsed);
}

void CPhysicsComponent::SetLook(CVector2D new_look)
{
	assert((new_look.LengthSq() - 1.0) < 1E-5);

	p_owner_->transform_.look_ = new_look;
	p_owner_->transform_.right_ = new_look.Perp();
}

bool CPhysicsComponent::RotateLookToFacePosition(CVector2D target)
{
	CVector2D toTarget = Vec2DNormalize(target - p_owner_->transform_.pos_);

	//first determine the angle between the heading vector and the target
	float angle = acos(p_owner_->transform_.look_.Dot(toTarget));

	//return true if the player is facing the target
	if (angle < 1E-5) return true;

	//clamp the amount to turn to the max turn rate
	if (angle > max_turn_rate_) angle = max_turn_rate_;

	//The next few lines use a rotation matrix to rotate the player's heading
	//vector accordingly
	C2DMatrix RotationMatrix;

	//notice how the direction of rotation has to be determined when creating
	//the rotation matrix
	RotationMatrix.Rotate(angle * p_owner_->transform_.look_.Sign(toTarget));
	RotationMatrix.TransformVector2Ds(p_owner_->transform_.look_);
	RotationMatrix.TransformVector2Ds(v_velocity_);

	//finally recreate m_vSide
	p_owner_->transform_.right_ = p_owner_->transform_.look_.Perp();

	return false;
}