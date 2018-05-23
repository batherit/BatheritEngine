#include "CGameTimer.h"
#include<time.h>
#include<windows.h>


CGameTimer::CGameTimer(TimerType timer_type){
	is_paused_ = false;
	timer_type_ = timer_type;
	elapsed_time_per_frame_ = 0;
	//elapsed_time_from_run_ = 0;

	switch (timer_type)
	{
	case TIMER_TYPE_TIME:
		base_cnt_ = clock();
		before_cnt_ = base_cnt_;
		current_cnt_ = before_cnt_;
		frequency_ = CLOCKS_PER_SEC;
		break;
	case TIMER_TYPE_WINDOWS:
		QueryPerformanceCounter((LARGE_INTEGER*) &base_cnt_);
		before_cnt_ = base_cnt_;
		current_cnt_ = before_cnt_;
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency_);
		break;
	default:
		break;
	}
}


CGameTimer::~CGameTimer()
{
}

void CGameTimer::Reset() {
	is_paused_ = false;
	elapsed_time_per_frame_ = 0;
	//elapsed_time_from_run_ = 0;
	pause_point_ = 0;

	switch (timer_type_) 
	{
	case TIMER_TYPE_TIME:
		current_cnt_ = clock();
		base_cnt_ = current_cnt_;
		current_cnt_ = current_cnt_;
		frequency_ = CLOCKS_PER_SEC;
		break;
	case TIMER_TYPE_WINDOWS:
		break;
	default:
		break;
	}
}

void CGameTimer::RunTick() {
	if (is_paused_) {
		elapsed_time_per_frame_ = 0.0;
		return;
	}

	switch (timer_type_)
	{
	case TIMER_TYPE_TIME:
		current_cnt_ = clock();
		elapsed_time_per_frame_ = (current_cnt_ - before_cnt_) / frequency_;
		// elapsed_time_from_run_ += elapsed_time_per_frame_;			// 경과 시간이 시작 이후로 누적된다.
		before_cnt_ = current_cnt_;
		break;
	case TIMER_TYPE_WINDOWS:
		break;
	default:
		break;
	}

	// 프로세서가 절전 모드로 들어가거나 다른 프로세서와 엉키는 경우
	// elapsed_time_per_frame_은 음수가 될 수 있다.
	if (elapsed_time_per_frame_ < 0.0) {
		elapsed_time_per_frame_ = 0.0;
	}
}

void CGameTimer::RunToPause() {
	if (!is_paused_) {
		is_paused_ = true;

		switch (timer_type_)
		{
		case TIMER_TYPE_TIME:
			pause_point_ = clock();
			break;
		case TIMER_TYPE_WINDOWS:
			break;
		default:
			break;
		}
	}
}

void CGameTimer::PauseToRun() {
	if (is_paused_) {
		is_paused_ = false;
	}
}

float CGameTimer::GetElapsedTimePerFrame() {
	return elapsed_time_per_frame_;
}

float CGameTimer::GetCurTime() {
	float current_time;

	switch (timer_type_)
	{
	case TIMER_TYPE_TIME:
		current_time = clock() / frequency_;
		break;
	case TIMER_TYPE_WINDOWS:
		break;
	default:
		break;
	}

	return current_time;
}

float CGameTimer::GetTotalTimeFromRun() {
	float total_time;
	if (is_paused_) {
		total_time = (float)((pause_point_ - pause_cnt_) - base_cnt_) / frequency_;
	}
	else {
		switch (timer_type_)
		{
		case TIMER_TYPE_TIME:
			current_cnt_ = clock();
			total_time = (float)((current_cnt_ - pause_cnt_) - base_cnt_) / frequency_;
			break;
		case TIMER_TYPE_WINDOWS:
			break;
		default:
			break;
		}
	}
	return total_time;//elapsed_time_from_run_;
}
