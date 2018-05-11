#include<iostream>
using namespace std;

int main(void){
	while (true) {
		CCommand cmd = GetCommand();
		handleCommand(cmd);
		Update();
		Render();
	}
	return 0;
}