#pragma once

#include <windows.h>

struct DataPack
{
	int width;
	int height;
	size_t buffersize;
	WORD* bufferptr;

};
class ICameraController
{
public:
	virtual ~ICameraController() {}
	virtual void SingleImageAquisition(char* file_name) = 0;
	virtual  WORD GetExposureAndDelayTime() = 0;
	virtual void SetExposureTime(DWORD value, DWORD unit) = 0;
	virtual DataPack* Live_View() = 0;
	virtual void Start_Sequence() = 0;


};