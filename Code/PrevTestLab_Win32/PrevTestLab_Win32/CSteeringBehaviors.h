#pragma once

#include"CMathUtil.h"
#include"CPath2D.h"
#include"CVehicleParamLoader.h"

class CWall2D;
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
	void SeekOn() { behavior_flags_ |= seek; }
	void FleeOn() { behavior_flags_ |= flee; }
	void ArriveOn() { behavior_flags_ |= arrive; }
	void PursuitOn(CVehicle* v) { behavior_flags_ |= pursuit; p_target_agent1_ = v; }
	void OffsetPursuitOn(CVehicle* v1, const CVector2D offset) { behavior_flags_ |= offset_pursuit;  p_target_agent1_ = v1; }
	void EvadeOn(CVehicle* v) { behavior_flags_ |= evade; p_target_agent1_ = v; }
	void WanderOn() { behavior_flags_ |= wander; }
	void ObstacleAvoidanceOn() { behavior_flags_ |= obstacle_avoidance; }
	void WallAvoidanceOn() { behavior_flags_ |= wall_avoidance; }
	void FollowPathOn() { behavior_flags_ |= follow_path; }
	void InterposeOn(CVehicle* v1, CVehicle* v2) { behavior_flags_ |= interpose; p_target_agent1_ = v1; p_target_agent2_ = v2; }
	void HideOn(CVehicle* v) { behavior_flags_ |= hide; p_target_agent1_ = v; }
	void CohesionOn() { behavior_flags_ |= cohesion; }
	void SeparationOn() { behavior_flags_ |= separation; }
	void AlignmentOn() { behavior_flags_ |= allignment; }
	void FlockingOn() { CohesionOn(); AlignmentOn(); SeparationOn(); WanderOn(); }

	// ~Off : �ش� �ൿ�� ��Ȱ��ȭ��Ų��.
	void SeekOff() { if (On(seek)) behavior_flags_ ^= seek; }
	void FleeOff() { if (On(flee)) behavior_flags_ ^= flee; }
	void ArriveOff() { if (On(arrive)) behavior_flags_ ^= arrive; }
	void PursuitOff() { if (On(pursuit)) behavior_flags_ ^= pursuit; }
	void OffsetPursuitOff() { if (On(offset_pursuit)) behavior_flags_ ^= offset_pursuit; }
	void EvadeOff() { if (On(evade)) behavior_flags_ ^= evade; }
	void WanderOff() { if (On(wander)) behavior_flags_ ^= wander; }
	void ObstacleAvoidanceOff() { if (On(obstacle_avoidance)) behavior_flags_ ^= obstacle_avoidance; }
	void WallAvoidanceOff() { if (On(wall_avoidance)) behavior_flags_ ^= wall_avoidance; }
	void FollowPathOff() { if (On(follow_path)) behavior_flags_ ^= follow_path; }
	void InterposeOff() { if (On(interpose)) behavior_flags_ ^= interpose; }
	void HideOff() { if (On(hide))  behavior_flags_ ^= hide; }
	void CohesionOff() { if (On(cohesion)) behavior_flags_ ^= cohesion; }
	void SeparationOff() { if (On(separation)) behavior_flags_ ^= separation; }
	void AlignmentOff() { if (On(allignment)) behavior_flags_ ^= allignment; }
	void FlockingOff() { CohesionOff(); AlignmentOff(); SeparationOff(); WanderOff(); }

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

	float TurnAroundTime(const CVehicle* pAgent, CVector2D TargetPos) const;

	// ���� �ൿ�� �����ϴ� ������Ŭ
	CVehicle* p_vehicle_;
	CVector2D v_steering_force_;

	CVehicle* p_target_agent1_;
	CVehicle* p_target_agent2_;
	CVector2D v_target_;

	// ������Ʈ�� ������ �ϴ� 'wander ��'�� ���� ��ǥ.
	CVector2D wander_target_;

	//a vertex buffer to contain the feelers rqd for wall avoidance  
	std::vector<CVector2D> feelers_;

	//the length of the 'feeler/s' used in wall detection
	float wall_detection_feeler_length_;    

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
	float weight_obstacle_avoidance_;
	float weight_wall_avoidance_;
	float weight_follow_path_;
	float weight_interpose_;
	float weight_hide_;

	// ������Ʈ�� �ٸ� ������Ʈ�� ������ �� �ִ� �ִ� �Ÿ�
	float view_distance_;

	// ���� ��θ� ����Ű�� ������
	CPath2D* p_path_;

	// ����Ŭ�� ���� ��������Ʈ�� ã�� ������ ��� ��������Ʈ�������� �־�߸� �ϴ� �Ÿ�(���� ����)
	float waypoint_seek_dist_sq_;

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

	// �ش� �ൿ�� ������Ʈ�� Ÿ�� ��ġ�� �����̵��� �Ѵ�.
	CVector2D Seek(CVector2D TargetPos);

	// �ش� �ൿ�� Ÿ�� ��ġ�κ��� ������Ʈ�� ���������� �ϴ� ���͸� ��ȯ�Ѵ�.
	CVector2D Flee(CVector2D TargetPos);

	// �ش� �ص��� 'Seek'�� �����ϳ�, ���� �ӵ��� Ÿ�� ��ġ�� �����Ϸ��� �õ��Ѵ�. 
	CVector2D Arrive(CVector2D     TargetPos, Deceleration deceleration);

	// �ش� �ൿ�� ��� ������Ʈ�� �ð� T�� ���� ������ �����ϰ� ������Ʈ��
	// ����ç ������ ���ϵ��� �Ѵ�.
	CVector2D Pursuit(const CVehicle* agent);

	// �� �ൿ�� Ÿ�� ����Ŭ���� �������� �������� ��ġ��
	// �����ϵ��� �Ѵ�.
	CVector2D OffsetPursuit(const CVehicle* agent, const CVector2D offset);

	// �� �ൿ�� �����ڸ� ���ϵ��� �Ѵ�.
	CVector2D Evade(const CVehicle* agent);

	// �� �ൿ�� ������Ʈ�� �������� ��ȸ�ϵ��� �Ѵ�.
	CVector2D Wander();

	// �̰��� ������Ʈ�� �����ϴ� ��� ��ֹ���� �־��������ϴ� ��������
	// ��ȯ�ϵ��� �Ѵ�.
	CVector2D ObstacleAvoidance(const std::vector<CGameObject*>& obstacles);

	// �̰��� ������Ʈ�� ����ġ�� ���� ���ϵ��� �ϴ� �������� ��ȯ�Ѵ�.
	CVector2D WallAvoidance(const std::vector<CWall2D> &walls);

	//�Ϸ��� Vector2D�� �־�����, �� �޼���� 
	//������Ʈ�� �߰� ������ ���� ������� �̵���Ű�� ���� �����Ѵ�.
	CVector2D FollowPath();

	// �̰��� �����̴� �� ������Ʈ�� �����ϴ� ������ �߾�����
	// ����Ŭ�� �����Ϸ��� ���� ��ȯ�Ѵ�.
	CVector2D Interpose(const CVehicle* VehicleA, const CVehicle* VehicleB);

	// �������� ����Ǵ� �ٸ� ������Ʈ ��ġ�� ���� ������Ʈ ����Ʈ�� �־����ٸ�,
	// �� �޼���� �ڽŰ� ��� ���̿� ��ֹ��� �η��� �õ��� �Ѵ�.
	CVector2D Hide(const CVehicle* hunter, const std::vector<CGameObject*>& obstacles);

	// -- �׷� �ൿ�� -- //

	// ����
	CVector2D Cohesion(const std::vector<CVehicle*> &agents);

	// �и�
	CVector2D Separation(const std::vector<CVehicle*> &agents);

	// ����
	CVector2D Alignment(const std::vector<CVehicle*> &agents);

	// ���� �� ���� �޼���� ���� �Ͱ� ���������� �̿� ������Ʈ�� ã�� ����
	// �� ���� ������ ����Ѵٴ� ���� �ٸ���.
	CVector2D CohesionPlus(const std::vector<CVehicle*> &agents);
	CVector2D SeparationPlus(const std::vector<CVehicle*> &agents);
	CVector2D AlignmentPlus(const std::vector<CVehicle*> &agents);

	// Ȱ��ȭ�� ��� �ൿ�鿡�� �������� ����ϰ� ���Ѵ�.
	CVector2D CalculateWeightedSum();
	CVector2D CalcuclatePrioritized();
	CVector2D CalculateDithered();

	// ���� �ൿ�� ���� ������ �޼����̴�. �����ڿ� �ݴ��� ��ֹ��� ��ġ��
	// ��ȯ�Ѵ�.
	CVector2D GetHidingPosition(const CVector2D& posOb,
		const float     radiusOb,
		const CVector2D& posHunter);
};