#pragma once
#ifdef LSFMLIBRARY_EXPORTS

#define LSFMLIBRARY_API __declspec(dllexport)
#else
#define LSFMLIBRARY_API __declspec(dllimport)
#endif

#include "Component.h"
#include "GlobalFunctions.h"
#include "LSFM_Camera.h"
#include "LSFM_Laser.h"
#include "ILaserController.h"
#include "LSFM_Motion_PI.h"
#include "Composite.h"
#include "ICameraController.h"
#include "IFilterController.h"
#include "XagylFilter.h"

extern HANDLE cameraPointer, flimCameraPointer, cameraRecorderPointer, flimCameraRecorderPointer;

extern "C" LSFMLIBRARY_API void Connect(); 

extern "C" LSFMLIBRARY_API void Disconnect();

extern "C" LSFMLIBRARY_API void MoveAbsolut(int stage, double position, bool acquisitionRunning);

extern "C" LSFMLIBRARY_API void MoveRelativ(int stage, double position, bool acquisitionRunning);

extern "C" LSFMLIBRARY_API int GetStageActualPosition(int stage);

extern "C" LSFMLIBRARY_API void LaserOn(int wavelength, int specPower);

extern "C" LSFMLIBRARY_API void LaserOff(int wavelength, int specPower);

extern "C" LSFMLIBRARY_API  bool SingleImageAquisition(int cameratyp, char* filename);

extern "C" LSFMLIBRARY_API int GetActualModulation(int wavelength, int specPower);

extern "C" LSFMLIBRARY_API int GetModulation(int wavelength, int specPower);

extern "C" LSFMLIBRARY_API double GetPowerInPercentage(int wavelength, int specPower);

extern "C" LSFMLIBRARY_API double ReadMaxPower(int wavelength, int specPower);


extern "C" LSFMLIBRARY_API void SetModulation(int wavelength, int specPower, int modulation);

extern "C" LSFMLIBRARY_API void SetOperatingMode(int wavelength, int specPower, int operantingMode);

extern "C" LSFMLIBRARY_API  void SetPower(int value, int specPower,  float power);

extern "C" LSFMLIBRARY_API DataPack* LiveView(int camera);

extern "C" LSFMLIBRARY_API bool SetFilmPhaseSequenceParameter(int phaseNumber, int phaseSymmetry, int phaseorder, int tapSelect);

extern "C" LSFMLIBRARY_API bool GetFilmPhaseSequenceParameter(int *phaseNumber, int *phaseSymmetry, int *phaseorder, int *tapSelect);

extern "C" LSFMLIBRARY_API void Image_Sequence(int cameraType, const char* path, double zUp, double zDown, double xValue, double yValue, double focusUp, double focusDown, int N, double dz, int phaseNumber);

extern "C" LSFMLIBRARY_API void GetConnectedCamera(int* listOfAvailableCamera);

extern "C" LSFMLIBRARY_API void GetConnectedMotions(int* listOfAvailabeMotion);

extern "C" LSFMLIBRARY_API bool GetActualPositions(double* xPosition, double* yPosition, double* zPosition, double* focusPosition);

extern "C" LSFMLIBRARY_API void SetExposureTime(int camera, int exposureTime, int delay);

extern "C" LSFMLIBRARY_API void  GetExposureTime(int camera, int* exposureTime , int* expousretimeBase);

extern "C" LSFMLIBRARY_API void  GetAllFiltersPositionsName(char* positionsList);

extern "C" LSFMLIBRARY_API int  MoveFilterToPosition(int position);

extern "C" LSFMLIBRARY_API int  GetFilterCurrentPosition();

extern "C" LSFMLIBRARY_API void ListNotConnectedHardware(char* arraylist);

extern "C" LSFMLIBRARY_API bool SetFlimParamter(int camera , int phaseNumber, int frequency);

extern "C" LSFMLIBRARY_API bool GetWaveLengthAndSpecPower(Laserpack* LaserList);













