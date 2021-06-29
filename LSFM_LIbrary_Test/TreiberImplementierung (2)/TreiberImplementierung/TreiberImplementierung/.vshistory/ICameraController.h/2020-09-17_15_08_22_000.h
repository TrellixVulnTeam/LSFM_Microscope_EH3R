#pragma once

#include <windows.h>

class ICameraController
{
public:
	virtual ~ICameraController() {}
	virtual void SingleImageAquisition(char* file_name) = 0;
	virtual  WORD GetExposureAndDelayTime() = 0;
	virtual void SetExposureTime(DWORD value, DWORD unit) = 0;
	virtual void Live_View(DWORD mode) = 0;
	virtual void Start_Sequence() = 0;


};