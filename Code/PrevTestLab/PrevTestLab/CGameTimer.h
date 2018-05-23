#pragma once

typedef __int64 ClockCnt;
enum TimerType { TIMER_TYPE_TIME, TIMER_TYPE_WINDOWS};

class CGameTimer sealed
{
public:
	CGameTimer(TimerType timer_type = TIMER_TYPE_TIME);
	~CGameTimer();

	void Reset();						// Ÿ�̸� �Ӽ��� �ʱ�ȭ
	void RunTick();						// �� ���� ƽ (�� ����� �ð��� �����Ѵ�.)
	void RunToPause();					// �Ͻ� ����
	void PauseToRun();					// Ÿ�̸� �۵� (���� ȣ�� �� ù ȣ�� �������� �ð� ����)

	float GetElapsedTimePerFrame();	// ������ �� ����� �ð�(sec)
	float GetCurTime();				// ���� �ð�(sec)
	float GetTotalTimeFromRun();		// ���� RunTick ���� �� ����� �ð�

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

