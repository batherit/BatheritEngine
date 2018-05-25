#pragma once

typedef __int64 ClockCnt;
enum TimerType { TIMER_TYPE_TIME, TIMER_TYPE_WINDOWS};

class CGameTimer sealed
{
public:
	CGameTimer(TimerType timer_type = TIMER_TYPE_WINDOWS);
	~CGameTimer();

	void Reset();						// 타이머 속성을 초기화
	void RunTick();						// 한 번의 틱 (곧 경과된 시간을 측정한다.)
	void RunToPause();					// 일시 정지
	void PauseToRun();					// 타이머 작동 (연속 호출 시 첫 호출 기준으로 시간 측정)

	float GetElapsedTimePerFrame();		// 프레임 당 경과된 시간(sec)
	float GetCurTime();					// 현재 시간(sec)
	float GetTotalTimeFromRun();		// 최초 RunTick 실행 후 경과된 시간

private:
	bool is_paused_;					// 일시정지되었는지?
	TimerType timer_type_;				// 타이머 타입 (time.h || windows.h)
	ClockCnt base_time_;				// 최초의 기본 시간 (카운트)
	ClockCnt before_time_;				// 이전 시간 (카운트)
	ClockCnt current_time_;				// 현재 시간 (카운트)
	ClockCnt pause_point_time_;			// 일시정지된 시점 (카운트)
	ClockCnt pause_total_time_;			// 누적된 일시정지 총 시간 (카운트)

	float period_;						// 주기 : 한 번의 카운트 당 소요되는 시간 (sec)
	float elapsed_time_per_frame_;		// 프레임 당 경과 시간 (sec)
};

