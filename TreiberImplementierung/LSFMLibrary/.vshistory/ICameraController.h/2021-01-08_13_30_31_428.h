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
	virtual void SingleImageAquisition(HANDLE camera, char* file_name) = 0;
	virtual  WORD GetExposureAndDelayTime(HANDLE camera) = 0;
	virtual void SetExposureTime(HANDLE camera, DWORD value, DWORD unit) = 0;
	virtual DataPack* Live_View(HANDLE camera, HANDLE recorder) = 0;
	virtual void Start_Sequence(HANDLE camera) = 0;


};