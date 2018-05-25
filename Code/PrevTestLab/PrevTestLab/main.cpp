#include<iostream>
#include<conio.h>
#include"CGameTimer.h"
using namespace std;

CGameTimer test_timer(TIMER_TYPE_WINDOWS);

char GetKey(void) {
	// 키보드 입력 감지를 true, false로 반환
	if (_kbhit()) {
		return _getch();
	}
	return '\0';
}

void HandleCommand(char& ch) {
	switch (ch) {
	case 'r': case 'R':
		cout << "reset" << endl;
		test_timer.Reset();
		break;
	case 'p': case 'P':
		cout << "pause" << endl;
		test_timer.RunToPause();
		break;
	case 's': case 'S':
		cout << "start" << endl;
		test_timer.PauseToRun();
		break;
	case 't': case 'T':
		cout << "print result" << endl;
		float time = test_timer.GetElapsedTimePerFrame();
		cout << "time per frame "<< time << "sec " << 1.0f / time << "fps" << endl;
		time = test_timer.GetTotalTimeFromRun();
		cout << "total time " << time << "sec " << endl << endl;
	}
}

int main(void){
	bool is_paused = false;

	// 게임 루프
	while (true) {
		test_timer.RunTick();
		char input = GetKey();
		HandleCommand(input);
	}
	return 0;
}