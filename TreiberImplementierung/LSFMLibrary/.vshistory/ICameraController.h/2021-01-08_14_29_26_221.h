#pragma once

#include <windows.h>

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
	eight = 3,
	sixteen = 4
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

class ICameraController
{
public:
	virtual ~ICameraController() {}
	virtual void SingleImageAquisition(HANDLE camera, char* file_name) = 0;
	virtual  WORD GetExposureAndDelayTime(HANDLE camera) = 0;
	virtual void SetExposureTime(HANDLE camera, DWORD value, DWORD unit) = 0;
	virtual DataPack* Live_View(HANDLE camera, HANDLE recorder) = 0;
	virtual void Start_Sequence(HANDLE camera) = 0;
	virtual void GetCameraAndRecorderHandle(HANDLE* camera, HANDLE* recorder) = 0;
	virtual bool SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect) = 0;
	virtual long int Get_Camera_Modulation_Frequency() = 0;
	virtual bool Set_Camera_Modulation_Frequency(DWORD waveform) = 0;
	virtual bool GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect) = 0;
};

