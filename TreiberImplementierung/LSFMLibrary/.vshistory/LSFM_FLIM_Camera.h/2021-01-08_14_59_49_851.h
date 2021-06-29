#pragma once
#include "LSFM_Camera.h"
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



class LSFM_FLIM_Camera : public LSFM_Camera
{
private:
	HANDLE flimCam; //  Pointer to a Pco Camera  HANDLE 
	HANDLE flimHrec;  // Pointer to a Pco recorder HANDLE 

	WaveForm waveform;
	long int ModFrequency;
	PhaseNumber phasenumber;
	PhaseSymmetry phasesymmetry;
	PhaseOrder phaseOrder;
	TapSelect tapSelect;

public:

	long int Get_Camera_Modulation_Frequency();
	bool Set_Camera_Modulation_Frequency(DWORD waveform);
	bool GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect);
	bool SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect);
	void Connect();
	void Disconnect();
	void GetCameraAndRecorderHandle(HANDLE* camera, HANDLE* recorder);
	void CloseConnectionWithComment( DWORD dwerr, const char* comment);
	bool IsNoErrorOccured(int iRet, const char* message);


	
};