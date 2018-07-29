#pragma once

#include"CMathUtil.h"
#include"CVehicleParamLoader.h"

class CVehicle;
class CGameObject;

//--------------------------- ������ ----------------------------------

// ��ȸ �ൿ�� ���� ��ȸ���� ������
const float WANDER_RAD = 1.2f;
// ������Ʈ �տ� ��ȸ���� ����Ǵ� �Ÿ� 
const float WANDER_DIST = 2.0f;
// �� �����ӿ� ���� ��ȸ���� �ִ� ������
const float WANDER_JITTER_PER_SEC = 80.0f;

// �� ���󰡱⿡ ���δ�.
const float WAY_POINT_SEEK_DIST = 20.0f;

class CSteeringBehavior
{
public:
	enum SUMMING_METHOD { WEIGHTED_AVERAGE, PRIORITIZED, DITHERED };

	CSteeringBehavior(CVehicle* agent);
	~CSteeringBehavior();

	CVector2D Calculate();

	// ~On : �ش� �ൿ�� Ȱ��ȭ��Ų��.
	void SeekOn() { behavior_flags_ |= flee; }
	void FleeOn() { behavior_flags_ |= seek; }
	void ArriveOn() { behavior_flags_ |= arrive; }
	void PursuitOn(CVehicle* v) { behavior_flags_ |= pursuit; p_target_agent1_ = v; }
	void OffsetPursuitOn(CVehicle* v1, const CVector2D offset) { behavior_flags_ |= offset_pursuit;  p_target_agent1_ = v1; }
	void EvadeOn(CVehicle* v) { behavior_flags_ |= evade; p_target_agent1_ = v; }
	void WanderOn() { behavior_flags_ |= wander; }

	// ~Off : �ش� �ൿ�� ��Ȱ��ȭ��Ų��.
	void SeekOff() { if (On(seek)) behavior_flags_ ^= seek; }
	void FleeOff() { if (On(flee)) behavior_flags_ ^= flee; }
	void ArriveOff() { if (On(arrive)) behavior_flags_ ^= arrive; }
	void PursuitOff() { if (On(pursuit)) behavior_flags_ ^= pursuit; }
	void OffsetPursuitOff() { if (On(offset_pursuit)) behavior_flags_ ^= offset_pursuit; }
	void EvadeOff() { if (On(evade)) behavior_flags_ ^= evade; }
	void WanderOff() { if (On(wander)) behavior_flags_ ^= wander; }

private:
	enum BEHAVIOR_TYPE
	{
		none = 0x00000,
		seek = 0x00002,
		flee = 0x00004,
		arrive = 0x00008,
		wander = 0x00010,
		cohesion = 0x00020,
		separation = 0x00040,
		allignment = 0x00080,
		obstacle_avoidance = 0x00100,
		wall_avoidance = 0x00200,
		follow_path = 0x00400,
		pursuit = 0x00800,
		evade = 0x01000,
		interpose = 0x02000,
		hide = 0x04000,
		flock = 0x08000,
		offset_pursuit = 0x10000,
	};

	// ���� �ൿ�� �����ϴ� ������Ŭ
	CVehicle* p_vehicle_;
	CVector2D v_steering_force_;

	CVehicle* p_target_agent1_;
	CVehicle* p_target_agent2_;
	CVector2D v_target_;

	// ������Ʈ�� ������ �ϴ� 'wander ��'�� ���� ��ǥ.
	CVector2D wander_target_;

	// ��ֹ� ȸ�ǿ� ���Ǵ� ���� ������ ����
	float detection_box_length_;

	// ���ʸ��� ��ǥ���� ������ �� �ִ� ������ ����
	float wander_jitter_;

	// ��ȸ�� �ݰ�
	float wander_radius_;

	// ��ȸ���� ����Ǵ� �Ÿ�
	float wander_distance_;

	// ������. �ൿ�� �ڿ��������� ���� ���ȴ�.
	float weight_seek_;
	float weight_flee_;
	float weight_arrive_;
	float weight_pursuit_;
	float weight_offset_pursuit_;
	float weight_evade_;
	float weight_wander_;

	// �����̼��̳� ������ ������ ���Ǵ� ������
	CVector2D offset_;
	// Ư�� �ൿ�� Ȱ��ȭ�Ǿ� �ִ��� �ƴ����� ����Ű�� ���� �÷����̴�.
	int behavior_flags_;

	// ������Ʈ�� ��ǥ�� �����ϴ� �� �󸶳� ���� �ð��� ���� ���ϴ°��� ���
	enum Deceleration { slow = 3, normal = 2, fast = 1 };

	//default
	Deceleration deceleration_;

	// �ش� ���� �÷��װ� ��ȿ(�ൿ�� Ȱ��ȭ�Ǿ�����)���� �˻��Ѵ�.
	bool On(BEHAVIOR_TYPE bt) { return (behavior_flags_ & bt) == bt; }

	bool AccumulateForce(CVector2D &sf, CVector2D ForceToAdd);

	// �ֺ��� ��ֹ��� ���ϴ� �ൿ�� ���Ǵ� '������'�� �����Ѵ�.
	void CreateFeelers();

	//this behavior moves the agent towards a target position
	CVector2D Seek(CVector2D TargetPos);

	//this behavior returns a vector that moves the agent away
	//from a target position
	CVector2D Flee(CVector2D TargetPos);

	//this behavior is similar to seek but it attempts to arrive 
	//at the target position with a zero velocity
	CVector2D Arrive(CVector2D     TargetPos, Deceleration deceleration);

	//this behavior predicts where an agent will be in time T and seeks
	//towards that point to intercept it.
	CVector2D Pursuit(const CVehicle* agent);

	//this behavior attempts to evade a pursuer
	CVector2D Evade(const CVehicle* agent);

	//this behavior makes the agent wander about randomly
	CVector2D Wander();

	// �̰��� ������Ʈ�� �����ϴ� ��� ��ֹ���� �־��������ϴ� ��������
	// ��ȯ�ϵ��� �մϴ�.
	CVector2D ObstacleAvoidance(const std::vector<CGameObject*>& obstacles);

	// Ȱ��ȭ�� ��� �ൿ�鿡�� �������� ����ϰ� ���Ѵ�.
	CVector2D CalculateWeightedSum();
	CVector2D CalcuclatePrioritized();
	CVector2D CalculateDithered();
};

