#include<iostream>
#include"CGameTimer.h"
using namespace std;

int main(void){
	CGameTimer test_timer(TIMER_TYPE_WINDOWS);
	while (true) {
		/*CCommand cmd = GetCommand();
		handleCommand(cmd);
		Update();
		Render();*/
		test_timer.RunTick();
		cout << test_timer.GetElapsedTimePerFrame() << "sec" << endl;
	}
	return 0;
}