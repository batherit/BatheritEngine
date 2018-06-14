#include "CTestPersonStates.h"
#include "CTestPerson.h"
#include <iostream>
#include <cstdlib>
#include<ctime>

#define DEFINE_RAND_SEED srand(time(nullptr));
inline double RandFloat() { return ((rand()) / (RAND_MAX + 1.0)); }

CTestPersonGlobalState* CTestPersonGlobalState::Instance() {
	static CTestPersonGlobalState instance;
	return &instance;
}

void CTestPersonGlobalState::Execute() {
	if ((RandFloat() < 0.05f) && !p_state_machine_->IsInState(*LookAround::Instance())) {
		p_state_machine_->ChangeState(LookAround::Instance());
	}
}

LookAround* LookAround::Instance() {
	static LookAround instance;
	return &instance;
}

void LookAround::Enter() {
	std::cout << "����...�ֺ� �� ����?" << std::endl;
}
void LookAround::Execute() {
	std::cout << "\'�θ��� �θ���\'" << std::endl;
	if (RandFloat() < 0.05f) {
		p_state_machine_->RevertToPreviousState();
	}
}
void LookAround::Exit() {
	std::cout << "���� ������?" << std::endl;
}

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