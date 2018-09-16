#pragma once
#include "CGameObject.h"
#include "CPhysicsComponent.h"
#include "CSteeringBehaviors.h"
#include "CSmoother.h"

class CGameWorld;

class CVehicle : public CGameObject
{
	friend class CSteeringBehavior;
public:
	CVehicle(
		int ID,
		CVector2D velocity,
		float max_speed,
		CVector2D look,
		float mass,
		float max_turn_rate,
		float max_force) : 
		CGameObject(ID),
		smoothed_heading_(CVector2D(0.0f, 0.0f)),
		is_smoothing_on_(false)
	{
		physics_ = new CPhysicsComponent(
			velocity,
			max_speed,
			look,
			mass,
			max_turn_rate,
			max_force
		);
		physics_->SetOwner(this);
		p_steering_ = new CSteeringBehavior(this);
		heading_smoother_ = new CSmoother<CVector2D>(VehiclePrm.num_samples_for_smoothing_, CVector2D(0.0f, 0.0f));
	}
	~CVehicle() {
		delete p_steering_;
		delete heading_smoother_;
	}

	void Update(float time_elapsed);
	void Receive(const Telegram& r_msg) {};

	CSteeringBehavior*const  Steering()const { return p_steering_; }

	CVector2D    SmoothedHeading()const { return smoothed_heading_; }
	bool        IsSmoothingOn()const { return is_smoothing_on_; }
	void        SmoothingOn() { is_smoothing_on_ = true; }
	void        SmoothingOff() { is_smoothing_on_ = false; }
	void        ToggleSmoothing() { is_smoothing_on_ = !is_smoothing_on_; }

private:
	CVehicle(const CVehicle&);
	CVehicle& operator=(const CVehicle&);
	CSteeringBehavior * p_steering_;

	// �Ϻ� ���� �ൿ�� �����Ÿ��� ��ó�� ���̴� �������� ���δ�.
	// ���� ��� �������� ����Ŭ�� ��� ���͸� �ε巴���ϴµ� ���ȴ�.
	CSmoother<CVector2D>* heading_smoother_;

	// �� ���ʹ� �ֱ� �� �����ӵ��� �ε巴�Ե� ����Ŭ�� ��� ���͵��� �����
	// ��Ÿ����.
	CVector2D smoothed_heading_;
	bool is_smoothing_on_;
};

