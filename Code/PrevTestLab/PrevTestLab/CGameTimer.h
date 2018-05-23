#pragma once

typedef __int64 ClockCnt;
enum TimerType { TIMER_TYPE_TIME, TIMER_TYPE_WINDOWS};

class CGameTimer sealed
{
public:
	CGameTimer(TimerType timer_type = TIMER_TYPE_TIME);
	~CGameTimer();

	void Reset();						// 타이머 속성을 초기화
	void RunTick();						// 한 번의 틱 (곧 경과된 시간을 측정한다.)
	void RunToPause();					// 일시 정지
	void PauseToRun();					// 타이머 작동 (연속 호출 시 첫 호출 기준으로 시간 측정)

	float GetElapsedTimePerFrame();	// 프레임 당 경과된 시간(sec)
	float GetCurTime();				// 현재 시간(sec)
	float GetTotalTimeFromRun();		// 최초 RunTick 실행 후 경과된 시간

private:
	bool is_paused_;
	TimerType timer_type_;
	ClockCnt base_cnt_;
	ClockCnt before_cnt_;
	ClockCnt current_cnt_;
	ClockCnt pause_point_;
	ClockCnt pause_cnt_;
	ClockCnt frequency_;
	float elapsed_time_per_frame_;
	// float elapsed_time_from_run_;
};

