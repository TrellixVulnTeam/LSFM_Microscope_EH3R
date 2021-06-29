
#pragma once

#include <iostream>
#include "ICameraController.h"
#include "Component.h"
#include "include/dp_status.h"
#include "include/jetraw_tiff.h"
#include "../LSFMLibrary/pch.h"
#include "include/pco_errt.h"
#include "include/SC2_SDKStructures.h"
#include "include/sc2_common.h"
#include "include/SC2_Defs.h"
#include "include/SC2_CamExport.h"
#include "include/pco_err.h"
#include "include/PCO_Recorder_Export.h"
#include "include/PCO_Recorder_Defines.h"




using namespace std;

struct DataPack
{
	int width;
	int height;
	size_t buffersize;
	WORD* bufferptr;

};
class LSFM_Camera : public Component, public ICameraController
{
private:

	int iRet;
	HANDLE hRec;  // Pointer to a Pco recorder HANDLE 



	HANDLE hCamArr[1]; //  // Array of handles to the cameras that should be used by the pco.recorder. Here there's only one camera available. 

	HANDLE cam; //  Pointer to a Pco Camera  HANDLE 

	DWORD maxImgCountArr[1];   // Array for retrieving the number of possible images that can be recorded.

	DWORD reqImgCountArr[1] = { 100 };


	DWORD cameraWarning, cameraError, cameraStatus;
	PCO_Description strDescription;
	WORD RecordingState;
	DWORD dwDelay, dwExposure;
	bool defaultParameter = true;
	bool isNoCameraConnected = true;
	WORD imgWidth = 0, imgHeight = 0;


	DWORD cameraMode;

	PCO_Description GetCameraDescription();

	void GetCameraHealthStatus();

	void CloseConnectionWithComment(DWORD dwerr, const char* comment);

	bool IsNoErrorOccured(int iRet, const char* message);

	bool GetRecodingstate();

	bool SetRecordingstate(WORD recordingstate);

	void RecordedStateOn();

	void RecordedStateOff();

	bool ResetToDefaultParameters();

	bool CreateRecorder(DWORD mode, char localDisk, DWORD* maxImgCountArr);

	bool GetAndSave_Image(char* file_name);


	void  Prepare_Recording(DWORD mode);

	void CheckAndSetTriggerMode(WORD requiredMode);


public:

	void Save_image(WORD XResAct, WORD YResAct, WORD* BufAdr);

	void Connect() override;

	void Disconnect() override;

	WORD GetExposureAndDelayTime() override;

	void SetExposureTime(DWORD value, DWORD unit) override;

	void add(Component* component) override;

	void remove(Component* component) override;

	void SingleImageAquisition(char* file_name) override;

	void Start_Sequence() override;

	WORD* Live_View(DWORD mode) override;

	bool MoveRelativXYZ(int stage, double position) override;

	bool MoveAbsolutXYZ(int stage, double position)override;


};
