#pragma once

typedef __int64 ClockCnt;
enum TimerType { TIMER_TYPE_TIME, TIMER_TYPE_WINDOWS};

class CGameTimer sealed
{
public:
	CGameTimer(TimerType timer_type = TIMER_TYPE_WINDOWS);
	~CGameTimer();

	void Reset();						// Ÿ�̸� �Ӽ��� �ʱ�ȭ
	void RunTick();						// �� ���� ƽ (�� ����� �ð��� �����Ѵ�.)
	void RunToPause();					// �Ͻ� ����
	void PauseToRun();					// Ÿ�̸� �۵� (���� ȣ�� �� ù ȣ�� �������� �ð� ����)

	float GetElapsedTimePerFrame();		// ������ �� ����� �ð�(sec)
	float GetCurTime();					// ���� �ð�(sec)
	float GetTotalTimeFromRun();		// ���� RunTick ���� �� ����� �ð�

private:
	bool is_paused_;					// �Ͻ������Ǿ�����?
	TimerType timer_type_;				// Ÿ�̸� Ÿ�� (time.h || windows.h)
	ClockCnt base_time_;				// ������ �⺻ �ð� (ī��Ʈ)
	ClockCnt before_time_;				// ���� �ð� (ī��Ʈ)
	ClockCnt current_time_;				// ���� �ð� (ī��Ʈ)
	ClockCnt pause_point_time_;			// �Ͻ������� ���� (ī��Ʈ)
	ClockCnt pause_total_time_;			// ������ �Ͻ����� �� �ð� (ī��Ʈ)

	float period_;						// �ֱ� : �� ���� ī��Ʈ �� �ҿ�Ǵ� �ð� (sec)
	float elapsed_time_per_frame_;		// ������ �� ��� �ð� (sec)
};

