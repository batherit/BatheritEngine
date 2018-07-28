#pragma once

#include"CIniFileLoaderBase.h"
#include"CMathUtil.h"

#define VehiclePrm	(*CVehicleParamLoader::Instance())

class CVehicleParamLoader : public CIniFileLoaderBase
{
public:
	static CVehicleParamLoader* Instance();

	int num_agents_;
	int num_obstacles_;
	float min_obstacle_radius_;
	float max_obstacle_radius_;

	// �������ҿ� ���Ǵ� ������ ��
	int num_cells_X_;
	// �������ҿ� ���Ǵ� �������� ��
	int num_cells_Y_;

	// �������� ������ ����ϴµ� ����� ���� ��
	int num_samples_for_smoothing_;

	// ���յ� �������� �����ϴµ� ���Դϴ�. (�ܼ��� �ִ��������� �ٲٴ� �����δ�
	// �۵����� ���� ���Դϴ�. �̷��� �����ڴ� ��� ������ �����ڿ� 
	// ���� ������ ��Ĩ�ϴ�.
	float steering_force_tweaker_;
	
	float max_steering_force_;
	float max_speed_;
	float vehicle_mass_;

	float vehicle_scale_;
	float max_turn_rate_per_second_;

	float separation_weight_;
	float alignment_weight_;
	float cohesion_weight_;
	float obstacle_avoidance_weight_;
	float wall_avoidance_weight_;
	float wander_weight_;
	float seek_weight_;
	float flee_weight_;
	float arrive_weight_;
	float pursuit_weight_;
	float offset_pursuit_weight_;
	float interpose_weight_;
	float hide_weight_;
	float evade_weight_;
	float follow_path_weight_;

	// ������Ʈ�� �̿� ������Ʈ�� �ν��ϱ� ���� �̿� ������Ʈ�� �󸶳� 
	// ��������� �ϴ����� ���� �Ÿ�
	float view_distance_;

	// ��ֹ� ȸ�ǿ� ���˴ϴ�.
	float min_detection_box_length_;

	// �� ȸ�ǿ� ���˴ϴ�.
	float wall_detection_feeler_length_;

	// �̰͵��� '�켱���� ���� ��� ���'�� ���� ��,
	// �������� ���� Ȯ���Դϴ�.
	float pr_wall_avoidance_;
	float pr_obstacle_avoidance_;
	float pr_separation_;
	float pr_alignment_;
	float pr_cohesion_;
	float pr_wander_;
	float pr_seek_;
	float pr_flee_;
	float pr_evade_;
	float pr_hide_;
	float pr_arrive_;

private:
	CVehicleParamLoader() :
		CIniFileLoaderBase("params.ini") {
		num_agents_ = GetNextParameterInt();
		num_obstacles_ = GetNextParameterInt();
		min_obstacle_radius_ = GetNextParameterFloat();
		max_obstacle_radius_ = GetNextParameterFloat();

		num_cells_X_ = GetNextParameterInt();
		num_cells_Y_ = GetNextParameterInt();

		num_samples_for_smoothing_ = GetNextParameterInt();

		steering_force_tweaker_ = GetNextParameterFloat();
		max_steering_force_ = GetNextParameterFloat() * steering_force_tweaker_;
		max_speed_ = GetNextParameterFloat();
		vehicle_mass_ = GetNextParameterFloat();
		vehicle_scale_ = GetNextParameterFloat();

		separation_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		alignment_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		cohesion_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		obstacle_avoidance_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		wall_avoidance_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		wander_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		seek_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		flee_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		arrive_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		pursuit_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		offset_pursuit_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		interpose_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		hide_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		evade_weight_ = GetNextParameterFloat() * steering_force_tweaker_;
		follow_path_weight_ = GetNextParameterFloat() * steering_force_tweaker_;

		view_distance_ = GetNextParameterFloat();
		min_detection_box_length_ = GetNextParameterFloat();
		wall_detection_feeler_length_ = GetNextParameterFloat();

		pr_wall_avoidance_ = GetNextParameterFloat();
		pr_obstacle_avoidance_ = GetNextParameterFloat();
		pr_separation_ = GetNextParameterFloat();
		pr_alignment_ = GetNextParameterFloat();
		pr_cohesion_ = GetNextParameterFloat();
		pr_wander_ = GetNextParameterFloat();
		pr_seek_ = GetNextParameterFloat();
		pr_flee_ = GetNextParameterFloat();
		pr_evade_ = GetNextParameterFloat();
		pr_hide_ = GetNextParameterFloat();
		pr_arrive_ = GetNextParameterFloat();

		max_turn_rate_per_second_ = Pi;
	}
	~CVehicleParamLoader();
};

