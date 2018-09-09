#pragma once

#include"CMathUtil.h"
#include"CPath2D.h"
#include"CVehicleParamLoader.h"

class CWall2D;
class CVehicle;
class CGameObject;

//--------------------------- 상수목록 ----------------------------------

// 배회 행동을 위한 배회원의 반지름
const float WANDER_RAD = 1.2f;
// 에이전트 앞에 배회원이 투사되는 거리 
const float WANDER_DIST = 2.0f;
// 각 프레임에 대한 배회원의 최대 변위량
const float WANDER_JITTER_PER_SEC = 80.0f;

// 길 따라가기에 쓰인다.
const float WAY_POINT_SEEK_DIST = 20.0f;

class CSteeringBehavior
{
public:
	enum SUMMING_METHOD { WEIGHTED_AVERAGE, PRIORITIZED, DITHERED };

	CSteeringBehavior(CVehicle* agent);
	~CSteeringBehavior();

	CVector2D Calculate();

	// ~On : 해당 행동을 활성화시킨다.
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

	// ~Off : 해당 행동을 비활성화시킨다.
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

	// 조종 행동이 조종하는 비하이클
	CVehicle* p_vehicle_;
	CVector2D v_steering_force_;

	CVehicle* p_target_agent1_;
	CVehicle* p_target_agent2_;
	CVector2D v_target_;

	// 에이전트가 가려고 하는 'wander 원'의 현재 좌표.
	CVector2D wander_target_;

	//a vertex buffer to contain the feelers rqd for wall avoidance  
	std::vector<CVector2D> feelers_;

	//the length of the 'feeler/s' used in wall detection
	float wall_detection_feeler_length_;    

	// 장애물 회피에 사용되는 감지 상자의 길이
	float detection_box_length_;

	// 매초마다 목표물에 더해질 수 있는 무작위 변위
	float wander_jitter_;

	// 배회원 반경
	float wander_radius_;

	// 배회원이 투사되는 거리
	float wander_distance_;

	// 곱셈자. 행동에 자연스러움을 위해 사용된다.
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

	// 에이전트가 다른 에이전트를 인지할 수 있는 최대 거리
	float view_distance_;

	// 현재 경로를 가리키는 포인터
	CPath2D* p_path_;

	// 비히클이 다음 웨이포인트를 찾기 이전에 경로 웨이포인트에서부터 있어야만 하는 거리(제곱 형태)
	float waypoint_seek_dist_sq_;

	// 포메이션이나 오프셋 추적에 사용되는 오프셋
	CVector2D offset_;
	// 특정 행동이 활성화되어 있는지 아닌지를 가리키는 이진 플래그이다.
	int behavior_flags_;

	// 에이전트가 목표에 도달하는 데 얼마나 많은 시간을 쓰길 원하는가를 계산
	enum Deceleration { slow = 3, normal = 2, fast = 1 };

	//default
	Deceleration deceleration_;

	// 해당 이진 플래그가 유효(행동이 활성화되었는지)한지 검사한다.
	bool On(BEHAVIOR_TYPE bt) { return (behavior_flags_ & bt) == bt; }

	bool AccumulateForce(CVector2D &sf, CVector2D ForceToAdd);

	// 주변의 장애물을 피하는 행동에 사용되는 '더듬이'를 생성한다.
	void CreateFeelers();

	// 해당 행동은 에이전트를 타겟 위치로 움직이도록 한다.
	CVector2D Seek(CVector2D TargetPos);

	// 해당 행동은 타겟 위치로부터 에이전트를 떨어지도록 하는 벡터를 반환한다.
	CVector2D Flee(CVector2D TargetPos);

	// 해당 해동은 'Seek'와 유사하나, 제로 속도로 타겟 위치에 도착하려고 시도한다. 
	CVector2D Arrive(CVector2D     TargetPos, Deceleration deceleration);

	// 해당 행동은 어디에 에이전트가 시간 T에 있을 것인지 예측하고 에이전트를
	// 가로챌 지점을 향하도록 한다.
	CVector2D Pursuit(const CVehicle* agent);

	// 이 행동은 타겟 비히클에서 오프셋의 방향으로 위치를
	// 유지하도록 한다.
	CVector2D OffsetPursuit(const CVehicle* agent, const CVector2D offset);

	// 이 행동은 추적자를 피하도록 한다.
	CVector2D Evade(const CVehicle* agent);

	// 이 행동은 에이전트가 무작위로 배회하도록 한다.
	CVector2D Wander();

	// 이것은 에이전트가 직면하는 몇몇 장애물들과 멀어지도록하는 조종력을
	// 반환하도록 한다.
	CVector2D ObstacleAvoidance(const std::vector<CGameObject*>& obstacles);

	// 이것은 에이전트가 마주치는 벽을 피하도록 하는 조종힘을 반환한다.
	CVector2D WallAvoidance(const std::vector<CWall2D> &walls);

	//일련의 Vector2D가 주어지면, 이 메서드는 
	//에이전트를 중간 지점을 따라 순서대로 이동시키는 힘을 생성한다.
	CVector2D FollowPath();

	// 이것은 움직이는 두 에이전트를 연결하는 벡터의 중앙으로
	// 비히클을 조종하려는 힘을 반환한다.
	CVector2D Interpose(const CVehicle* VehicleA, const CVehicle* VehicleB);

	// 숨기위해 고려되는 다른 에이전트 위치와 게임 오브젝트 리스트가 주어진다면,
	// 이 메서드는 자신과 상대 사이에 장애물을 두려는 시도를 한다.
	CVector2D Hide(const CVehicle* hunter, const std::vector<CGameObject*>& obstacles);

	// -- 그룹 행동들 -- //

	// 응집
	CVector2D Cohesion(const std::vector<CVehicle*> &agents);

	// 분리
	CVector2D Separation(const std::vector<CVehicle*> &agents);

	// 정렬
	CVector2D Alignment(const std::vector<CVehicle*> &agents);

	// 다음 세 개의 메서드는 위의 것과 동일하지만 이웃 에이전트를 찾기 위해
	// 셀 공간 분할을 사용한다는 점이 다르다.
	CVector2D CohesionPlus(const std::vector<CVehicle*> &agents);
	CVector2D SeparationPlus(const std::vector<CVehicle*> &agents);
	CVector2D AlignmentPlus(const std::vector<CVehicle*> &agents);

	// 활성화된 몇가지 행동들에서 조종힘을 계산하고 더한다.
	CVector2D CalculateWeightedSum();
	CVector2D CalcuclatePrioritized();
	CVector2D CalculateDithered();

	// 숨기 행동에 대한 도움자 메서드이다. 추적자에 반대편 장애물에 위치를
	// 반환한다.
	CVector2D GetHidingPosition(const CVector2D& posOb,
		const float     radiusOb,
		const CVector2D& posHunter);
};