#include "CGameTimer.h"
#include<time.h>
#include<windows.h>


CGameTimer::CGameTimer(TimerType timer_type)
	: is_paused_(false),
	timer_type_(timer_type),
	pause_point_time_(0),
	pause_total_time_(0),
	elapsed_time_per_frame_(0.0f){
	ClockCnt frequency;
	switch (timer_type)
	{
	case TIMER_TYPE_TIME:
		current_time_ = clock();
		before_time_ = current_time_;
		base_time_ = current_time_;
		frequency = CLOCKS_PER_SEC;
		break;
	case TIMER_TYPE_WINDOWS:
		QueryPerformanceCounter((LARGE_INTEGER*) &current_time_);
		before_time_ = current_time_;
		base_time_ = current_time_;
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		break;
	default:
		break;
	}

	period_ = 1.0f / (float)frequency;
}


CGameTimer::~CGameTimer()
{
}

void CGameTimer::Reset() {
	is_paused_ = false;
	pause_point_time_ = 0;
	pause_total_time_ = 0;
	elapsed_time_per_frame_ = 0.0f;

	ClockCnt frequency;
	switch (timer_type_)
	{
	case TIMER_TYPE_TIME:
		current_time_ = clock();
		before_time_ = current_time_;
		base_time_ = current_time_;
		break;
	case TIMER_TYPE_WINDOWS:
		QueryPerformanceCounter((LARGE_INTEGER*)&current_time_);
		before_time_ = current_time_;
		base_time_ = current_time_;
		break;
	default:
		break;
	}
}

void CGameTimer::RunTick() {
	if (is_paused_) {
		elapsed_time_per_frame_ = 0.0f;
		return;
	}

	switch (timer_type_)
	{
	case TIMER_TYPE_TIME:
		current_time_ = clock();
		break;
	case TIMER_TYPE_WINDOWS:
		QueryPerformanceCounter((LARGE_INTEGER*)&current_time_);
		break;
	default:
		break;
	}

	elapsed_time_per_frame_ = (current_time_ - before_time_) * period_;
	before_time_ = current_time_;

	// 프로세서가 절전 모드로 들어가거나 다른 프로세서와 엉키는 경우
	// elapsed_time_per_frame_은 음수가 될 수 있다.
	if (elapsed_time_per_frame_ < 0.0) {
		elapsed_time_per_frame_ = 0.0;
	}
}

void CGameTimer::RunToPause() {
	if (!is_paused_) {
		switch (timer_type_)
		{
		case TIMER_TYPE_TIME:
			pause_point_time_ = clock();
			break;
		case TIMER_TYPE_WINDOWS:
			QueryPerformanceCounter((LARGE_INTEGER*)&pause_point_time_);
			break;
		default:
			break;
		}

		is_paused_ = true;
	}
}

void CGameTimer::PauseToRun() {
	if (is_paused_) {
		ClockCnt run_point_time;
		switch (timer_type_)
		{
		case TIMER_TYPE_TIME:
			run_point_time = clock();
			break;
		case TIMER_TYPE_WINDOWS:
			QueryPerformanceCounter((LARGE_INTEGER*)&run_point_time);
			break;
		default:
			break;
		}

		pause_total_time_ += (run_point_time - pause_point_time_);
		before_time_ = run_point_time;
		pause_point_time_ = 0;
		is_paused_ = false;
	}
}

float CGameTimer::GetElapsedTimePerFrame() {
	return elapsed_time_per_frame_;
}

float CGameTimer::GetCurTime() {
	return current_time_ * period_;
}

float CGameTimer::GetTotalTimeFromRun() {
	float total_time;
	if (is_paused_) {
		total_time = (float)((pause_point_time_ - pause_total_time_) - base_time_) * period_;
	}
	else {
		switch (timer_type_)
		{
		case TIMER_TYPE_TIME:
			current_time_ = clock();
			break;
		case TIMER_TYPE_WINDOWS:
			QueryPerformanceCounter((LARGE_INTEGER*)&current_time_);
			break;
		default:
			break;
		}

		total_time = (float)((current_time_ - pause_total_time_) - base_time_) * period_;
	}
	return total_time;
}
