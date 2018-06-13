#include "CTestPersonStates.h"
#include <iostream>

#define IDLE_STATE_INST IDLE::Instance()

IDLE* IDLE::Instance() {
	static IDLE instance;
	return &instance;
}

void IDLE::Enter() {
	std::cout << "���� ������ ���� ���̴�." << std::endl;
}

void IDLE::Execute() {
	std::cout << "\'�״� ������ �ִ� ���̴�.\'" << std::endl;
}

void IDLE::Exit() {
	std::cout << "���� �ٸ� ���� �غ����� �ұ�?" << std::endl;
}