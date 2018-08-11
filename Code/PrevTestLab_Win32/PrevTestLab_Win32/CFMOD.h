#pragma once
#include "CIniFileLoaderBase.h"
#include<fmod.h>

#define SOUND_NUM 100
#define FMOD	CFMOD::Instance()

class CFMOD : public CIniFileLoaderBase
{
public:
	CFMOD(const char* file_name);
	~CFMOD();

	static CFMOD* Instance();
	void Update();
private:
	FMOD_SYSTEM	*p_fmod_system_;		// FMOD system 변수선언
	FMOD_CHANNEL *p_channels_[SOUND_NUM];
	FMOD_SOUND   *p_sounds_[SOUND_NUM];

	int sounds_num_;
}; 