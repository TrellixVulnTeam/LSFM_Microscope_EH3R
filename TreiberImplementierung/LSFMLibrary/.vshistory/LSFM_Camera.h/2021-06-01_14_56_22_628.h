
#pragma once

#include <iostream>
#include "ICameraController.h"
#include "Component.h"
#include "include/dp_status.h"
#include "include/jetraw_tiff.h"
#include "include/stdafx.h"
#include "include/pco_errt.h"
#include "include/SC2_SDKStructures.h"
#include "include/sc2_common.h"
#include "include/SC2_Defs.h"
#include "include/SC2_CamExport.h"
#include "include/pco_err.h"
#include "include/PCO_Recorder_Export.h"
#include "include/PCO_Recorder_Defines.h"




using namespace std;


class LSFM_Camera : public Component, public ICameraController
{
private:


	HANDLE hRec;  // Pointer to a Pco recorder HANDLE 

	HANDLE cam; //  Pointer to a Pco Camera  HANDLE 

	bool isNoCameraConnected = true;



protected:
	int iRet;

	HANDLE hCamArr[2]; //  // Array of handles to the cameras that should be used by the pco.recorder. Here there's only one camera available. 

	DWORD maxImgCountArr[1];   // Array for retrieving the number of possible images that can be recorded.

	DWORD reqImgCountArr[1] = { 100 };


	DWORD cameraWarning, cameraError, cameraStatus;
	PCO_Description strDescription;
	WORD RecordingState;
	DWORD dwDelay, dwExposure;
	bool defaultParameter = true;
	WORD imgWidth = 0, imgHeight = 0;

	DWORD cameraMode;




public:
	PCO_Description GetCameraDescription(HANDLE camera);

	void GetCameraHealthStatus(HANDLE camera);

	void CloseConnectionWithComment(DWORD dwerr, const char* comment);

	bool IsNoErrorOccured(int iRet, const char* message);

	bool GetRecodingstate(HANDLE camera);

	bool SetRecordingstate(HANDLE camera, WORD recordingstate);

	void RecordedStateOn(HANDLE camera);

	void RecordedStateOff(HANDLE camera);

	bool ResetToDefaultParameters(HANDLE camera);

	bool CreateRecorder(HANDLE camera, HANDLE* recorder, DWORD mode, char localDisk, DWORD* maxImgCountArr);

	bool GetAndSave_Image(CameraTyp cameraTyp, HANDLE recorder, HANDLE camera, PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, TapSelect tapSelect, char* file_name);


	void  Prepare_Recording(HANDLE camera, HANDLE* recorder, DWORD mode);

	void CheckAndSetTriggerMode(HANDLE camera, WORD requiredMode);

	void Save_image(HANDLE camera, WORD XResAct, WORD YResAct, WORD* BufAdr);

	void Connect() override;

	void Disconnect() override;

	void GetExposureAndDelayTime(HANDLE camera, WORD* expousreTime, WORD* delay) override;

	void SetExposureTime(HANDLE camera, DWORD exposureTime, DWORD delay) override;

	void add(Component* component) override;

	void remove(Component* component) override;

	bool SingleImageAquisition(int cameraTyp, HANDLE recorder, HANDLE camera, int phaseNumber, int phasesymmetry, int tapSelect, char* file_name) override;

	void Start_Sequence(HANDLE camera) override;

	DataPack* Live_View(int cameraTyp, HANDLE camera, HANDLE recorder) override;

	void GetCameraAndRecorderHandle(HANDLE* camera, HANDLE* recorder) override;

	bool SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect) override;

	long int Get_Camera_Modulation_Frequency() override;

	bool Set_Camera_Modulation_Frequency(DWORD waveform) override;

	bool GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect) override;

	bool GetCameraStatusConnection() override;

	bool IsHardwareConnected() override;

};
