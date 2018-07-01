#include "CVehicle.h"
#include "CMathUtil.h"
#include "CSteeringBehaviors.h"

CVehicle::~CVehicle()
{
}

void CVehicle::Update(float time_elapsed) {
	CVector2D old_pos = p_owner_->transform_.pos_;
	CVector2D steering_force = p_steering_->Calculate();
	CVector2D acceleration = steering_force / mass_;
	v_velocity_ += acceleration * time_elapsed;
	v_velocity_.Truncate(max_speed_);
	p_owner_->transform_.pos_ += v_velocity_ * time_elapsed;

	if (v_velocity_.LengthSq() > 0.00000001)
	{
		p_owner_->transform_.look_ = Vec2DNormalize(v_velocity_);

		p_owner_->transform_.right_ = p_owner_->transform_.look_.Perp();
	}

	//EnforceNonPenetrationConstraint(this, World()->Agents());

	//treat the screen as a toroid
	/*WrapAround(m_vPos, m_pWorld->cxClient(), m_pWorld->cyClient());*/

	//update the vehicle's current cell if space partitioning is turned on
	/*if (Steering()->isSpacePartitioningOn())
	{
		World()->CellSpace()->UpdateEntity(this, OldPos);
	}

	if (isSmoothingOn())
	{
		m_vSmoothedHeading = m_pHeadingSmoother->Update(Heading());
	}*/
}