#pragma once
#include "LSFM_Camera.h"

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
	BOOL Set_Camera_Modulation_Frequency(DWORD waveform);
	BOOL GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect);
	BOOL SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect);

};