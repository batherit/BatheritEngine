#include "CVehicleParamLoader.h"

CVehicleParamLoader* CVehicleParamLoader::Instance() {
	static CVehicleParamLoader instance;

	return &instance;
}

CVehicleParamLoader::~CVehicleParamLoader()
{
}
