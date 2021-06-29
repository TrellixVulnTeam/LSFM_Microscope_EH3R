
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
#include <vector>




using namespace std;


class LSFM_Camera : public Component, public ICameraController
{
private:


	HANDLE hRec = NULL;  // Pointer to a Pco recorder HANDLE 

	HANDLE cam = NULL; //  Pointer to a Pco Camera  HANDLE 

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

	WaveForm waveform;
	long int ModFrequency;
	PhaseNumber phasenumber;
	PhaseSymmetry phasesymmetry;
	PhaseOrder phaseOrder;
	TapSelect tapSelect;


public:
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

	bool GetAndSave_Image(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, TapSelect tapSelect, char* file_name);


	void  Prepare_Recording(DWORD mode);

	void CheckAndSetTriggerMode(WORD requiredMode);

	void Save_image(WORD XResAct, WORD YResAct, WORD* BufAdr);

	int GetCameraIdentification();


	void Connect() override;



	void Disconnect() override;

	void GetExposureAndDelayTime(WORD* expousreTime, WORD* exposuretimeBase) override;

	void SetExposureTime(DWORD value, DWORD unit) override;

	void add(Component* component) override;

	void remove(Component* component) override;

	bool SingleImageAquisition(int phaseNumber, int phasesymmetry, int tapSelect, char* file_name) override;

	void Start_Sequence() override;

	vector<WORD*> Live_View(int* img_width, int* img_Height) override;

	bool SetFlimPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect) override;

	long int Get_Camera_Modulation_Frequency() override;

	bool Set_Camera_Modulation_Frequency(DWORD waveform) override;

	bool GetFlimPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect) override;
	
	bool GetCameraStatusConnection() override;

	bool Set_Camera_Master_modulation_frequency(DWORD frequency);

};
