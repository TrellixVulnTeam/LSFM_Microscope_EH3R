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
	Eight = 3,
	Sixteen = 4
};

enum PhaseSymmetry {
	singular = 1,
	twice = 2
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
	virtual DataPack* SingleImageAquisition(int cameraTyp, HANDLE recorder, HANDLE camera, int phaseNumber, int phasesymmetry, int tapSelect, char* file_name) = 0;
	virtual  void GetExposureAndDelayTime(HANDLE camera, WORD* expousreTime, WORD* exposuretimeBase) = 0;
	virtual void SetExposureTime(HANDLE camera, DWORD value, DWORD unit) = 0;
	virtual DataPack* Live_View(int cameraTyp, HANDLE camera, HANDLE recorder) = 0;
	virtual void Start_Sequence(HANDLE camera) = 0;
	virtual void GetCameraAndRecorderHandle(HANDLE* camera, HANDLE* recorder) = 0;
	virtual bool SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect) = 0;
	virtual long int Get_Camera_Modulation_Frequency() = 0;
	virtual bool Set_Camera_Modulation_Frequency(DWORD waveform) = 0;
	virtual bool GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect) = 0;
	virtual bool GetCameraStatusConnection() = 0;
	
};
