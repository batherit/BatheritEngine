#include "CFMOD.h"
#include <iostream>
#include <string>


CFMOD::CFMOD(const char* file_name) : CIniFileLoaderBase(file_name)
{
	std::cout << "Create FMOD System" << std::endl;
	FMOD_System_Create(&p_fmod_system_);
	FMOD_System_Init(p_fmod_system_, 32, FMOD_INIT_NORMAL, NULL);

	sounds_num_ = GetNextParameterInt();

	std::string sound_ID;
	std::string sound_route;
	bool is_sound_loop;
	for (int i = 0; i < sounds_num_; i++) {
		sound_ID = GetNextParameterString();
		sound_route = GetNextParameterString();
		is_sound_loop = GetNextParameterBool();

		std::cout << "Sound ID : " << sound_ID << std::endl;
		std::cout << "Sound Route : " << sound_route << std::endl;

		FMOD_System_CreateSound(
			p_fmod_system_,
			sound_route.c_str(),
			is_sound_loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT, 
			0,
			&p_sounds_[i]);
	}
}


CFMOD::~CFMOD()
{
	for (int i = 0; i < SOUND_NUM; i++) {
		if (p_sounds_[i]) FMOD_Sound_Release(p_sounds_[i]);
	}
	FMOD_System_Close(p_fmod_system_);
	FMOD_System_Release(p_fmod_system_);
}

CFMOD* CFMOD::Instance() {
	static CFMOD instance("test_sound.ini");
	return &instance;
}

void CFMOD::Update() {
	FMOD_System_Update(p_fmod_system_);
}