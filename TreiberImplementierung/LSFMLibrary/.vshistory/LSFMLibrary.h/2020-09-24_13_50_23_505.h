#pragma once
#ifdef LSFMLIBRARY_EXPORTS

#define LSFMLIBRARY_API __declspec(dllexport)
#else
#define LSFMLIBRARY_API __declspec(dllimport)
#endif


extern "C" LSFMLIBRARY_API void Connect(); 

extern "C" LSFMLIBRARY_API void Disconnect();

extern "C" LSFMLIBRARY_API void MoveAbsolut(int stage, double position);

extern "C" LSFMLIBRARY_API void MoveRelativ(int stage, double position);

extern "C" LSFMLIBRARY_API void LaserOn();

extern "C" LSFMLIBRARY_API void LaserOff();

extern "C" LSFMLIBRARY_API void SingleImageAquisition();

extern "C" LSFMLIBRARY_API void GetActualModulation();

extern "C" LSFMLIBRARY_API void GetModulation();

extern "C" LSFMLIBRARY_API void GetOperatingMode();

extern "C" LSFMLIBRARY_API double GetPowerInPercentage();

extern "C" LSFMLIBRARY_API double ReadMaxPower();


extern "C" LSFMLIBRARY_API int ReadWaveLength();

extern "C" LSFMLIBRARY_API void SetModulation(int modulation);

extern "C" LSFMLIBRARY_API void SetOperatingMode(int operantingMode);

extern "C" LSFMLIBRARY_API void SetPower(float power);








