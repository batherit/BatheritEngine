#include "CTestPersonStates.h"
#include <iostream>

#define IDLE_STATE_INST IDLE::Instance()

IDLE* IDLE::Instance() {
	static IDLE instance;
	return &instance;
}

void IDLE::Enter() {
	std::cout << "나는 가만히 있을 것이다." << std::endl;
}

void IDLE::Execute() {
	std::cout << "\'그는 가만히 있는 중이다.\'" << std::endl;
}

void IDLE::Exit() {
	std::cout << "이제 다른 것을 해보도록 할까?" << std::endl;
}