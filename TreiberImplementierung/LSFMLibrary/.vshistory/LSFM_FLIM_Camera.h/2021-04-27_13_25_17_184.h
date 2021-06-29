#pragma once
#include "LSFM_Camera.h"



using namespace std;

class LSFM_FLIM_Camera :public LSFM_Camera
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
	bool isNoFlimCameraConnected = true;

public:

	long int Get_Camera_Modulation_Frequency();
	bool Set_Camera_Modulation_Frequency(DWORD waveform);
	bool GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect);
	bool SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect);
	void Connect();
	void Disconnect();
	void GetCameraAndRecorderHandle(HANDLE* camera, HANDLE* recorder);
	void CloseConnectionWithComment(DWORD dwerr, const char* comment);
	bool IsNoErrorOccured(int iRet, const char* message);
	bool Set_Camera_Master_modulation_frequency(DWORD frequency);
	bool GetCameraStatusConnection() override;





};