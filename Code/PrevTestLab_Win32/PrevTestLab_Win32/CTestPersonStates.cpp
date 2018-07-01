#include "CTestPersonStates.h"
#include "CTestPerson.h"
#include <iostream>

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
	std::cout << "흐음...주변 좀 볼까?" << std::endl;
}
void LookAround::Execute() {
	std::cout << "\'두리번 두리번\'" << std::endl;
	if (RandFloat() < 0.05f) {
		p_state_machine_->RevertToPreviousState();
	}
}
void LookAround::Exit() {
	std::cout << "이제 뭐하지?" << std::endl;
}

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