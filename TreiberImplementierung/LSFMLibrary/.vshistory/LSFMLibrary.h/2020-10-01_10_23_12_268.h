#pragma once
#ifdef LSFMLIBRARY_EXPORTS

#define LSFMLIBRARY_API __declspec(dllexport)
#else
#define LSFMLIBRARY_API __declspec(dllimport)
#endif

#include "LSFM_Camera.h"

extern "C" LSFMLIBRARY_API void Connect(); 

extern "C" LSFMLIBRARY_API void Disconnect();

extern "C" LSFMLIBRARY_API void MoveAbsolut(int stage, double position);

extern "C" LSFMLIBRARY_API void MoveRelativ(int stage, double position);

extern "C" LSFMLIBRARY_API void LaserOn(int wavelength);

extern "C" LSFMLIBRARY_API void LaserOff(int wavelength);

extern "C" LSFMLIBRARY_API void SingleImageAquisition();

extern "C" LSFMLIBRARY_API void GetActualModulation();

extern "C" LSFMLIBRARY_API void GetModulation();

extern "C" LSFMLIBRARY_API void GetOperatingMode();

extern "C" LSFMLIBRARY_API double GetPowerInPercentage(int wavelength);

extern "C" LSFMLIBRARY_API double ReadMaxPower(int wavelength);


extern "C" LSFMLIBRARY_API int ReadWaveLength();

extern "C" LSFMLIBRARY_API void SetModulation(int wavelength,int modulation);

extern "C" LSFMLIBRARY_API void SetOperatingMode(int wavelength,int operantingMode);

extern "C" LSFMLIBRARY_API void SetPower(int wavelength,float power);

extern "C" LSFMLIBRARY_API bool LaserStatus();

extern "C" LSFMLIBRARY_API DataPack* LiveView();

extern "C" LSFMLIBRARY_API void Image_Sequence(double zUp, double zDown, double xValue, double yValue, double focusUp, double focusDown, int N, double dz);









