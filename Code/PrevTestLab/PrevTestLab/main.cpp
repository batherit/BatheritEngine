#include<iostream> 
#include<conio.h> 
#include"CGameTimer.h"
#include"CGameObject.h"
#include"CGameStateMachine.h"
#include"CGameState.h"
#include"CTestPerson.h"
#include"CTestPersonStates.h"
using namespace std;

CGameTimer g_test_timer(TIMER_TYPE_WINDOWS);
#define IDLE_STATE IDLE::Instance()

int main(void) {
	CTestPerson test_object(0);
	test_object.RegisterComponent(
		new CGameStateMachine<CTestPerson>(IDLE_STATE));

	
	// 게임 루프 
	while (true) {
		g_test_timer.RunTick();
		test_object.Update(g_test_timer.GetElapsedTimePerFrame());
	}
	return 0;

}

/*TestSingleton::instance().SetNum(8);
TestSingleton::instance().PrintNum();*/

//char GetKey(void) {
//	 키보드 입력 감지를 true, false로 반환 
//	if (_kbhit()) {
//		return _getch();
//	}
//	return '\0';
//}
//
//void HandleCommand(char& ch) {
//	switch (ch) {
//	case 'r': case 'R':
//		cout << "reset" << endl;
//		g_test_timer.Reset();
//		break;
//	case 'p': case 'P':
//		cout << "pause" << endl;
//		g_test_timer.RunToPause();
//		break;
//	case 's': case 'S':
//		cout << "start" << endl;
//		g_test_timer.PauseToRun();
//		break;
//	case 't': case 'T':
//		cout << "print result" << endl;
//		float time = g_test_timer.GetElapsedTimePerFrame();
//		cout << "time per frame " << time << "sec " << 1.0f / time << "fps" << endl;
//		time = g_test_timer.GetTotalTimeFromRun();
//		cout << "total time " << time << "sec " << endl << endl;
//	}
//}
//
//class TestSingleton {
//private:
//	int num = 5;
//
//public:
//	static TestSingleton& instance() {
//		static TestSingleton *instance = new TestSingleton();
//		return *instance;
//	}
//
//	void SetNum(int n) { num = n; }
//	void PrintNum(void) { cout << num << endl; }
//};
