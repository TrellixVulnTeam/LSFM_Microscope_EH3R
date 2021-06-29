#pragma once

#include <windows.h>
#include <vector>


struct DataPack
{
	int width;
	int height;
	size_t buffersize;
	WORD* bufferptr;
};


enum WaveForm
{
	sinusoidal = 1,
	rectangular = 2
};

enum PhaseNumber {
	Two = 1,
	Four = 2,
	Eight = 3,
	Sixteen = 4
};

enum PhaseSymmetry {
	singular = 0,
	twice = 1
};

enum PhaseOrder {
	ascending = 0,
	opposite = 1
};

enum TapSelect {
	both = 0,
	Tap_A = 1,
	Tap_B = 2
};

enum CameraTyp {
	LSFM = 0,
	LSFM_FLIM = 1
};

class ICameraController
{
public:
	virtual ~ICameraController() {}
	virtual bool SingleImageAquisition(char* file_name) = 0;
	virtual  void GetExposureAndDelayTime(WORD* expousreTime, WORD* exposuretimeBase) = 0;
	virtual void SetExposureTime(DWORD value, DWORD unit) = 0;
	virtual DataPack* Live_View() = 0;
	virtual void Start_Sequence() = 0;
	virtual bool SetFlimPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect, DWORD frequency) = 0;
	virtual long int Get_Camera_Modulation_Frequency() = 0;
	virtual bool Set_Camera_Modulation_Frequency(DWORD waveform) = 0;
	virtual bool GetFlimPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect) = 0;
	virtual bool GetCameraStatusConnection() = 0;
	virtual bool IsHardwareConnected() = 0;
	virtual int GetCameraIdentification() = 0;


};
