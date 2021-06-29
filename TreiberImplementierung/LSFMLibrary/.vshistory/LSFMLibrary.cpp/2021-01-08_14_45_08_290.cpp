// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "LSFMLibrary.h"
#include <iostream>
#include<String>
#include <vector>
#include "Component.h"
#include "GlobalFunctions.h"
#include "LSFM_Camera.h"
#include "LSFM_Laser.h"
#include "ILaserController.h"
#include "LSFM_Motion_PI.h"
#include "Composite.h"
#include "ICameraController.h"
#include "LSFM_FLIM_Camera.h"

// DLL internal state variables:
static unsigned long long previous_;  // Previous value, if any
static unsigned long long current_;   // Current sequence value
static unsigned index_;               // Current seq. position



HANDLE cameraPointer, flimCameraPointer, cameraRecorderPointer, flimCameraRecorderPointer;

vector<LSFM_Motion_PI> controllers;
Composite microscope;

LSFM_Camera camera;
Component* cameraComponent = static_cast<Component*>(&camera);

LSFM_FLIM_Camera flimCamera;
Component* flimCameraComponent = static_cast<Component*>(&flimCamera);


LSFM_Laser l1("\\\\.\\COM11");
LSFM_Laser l2("\\\\.\\COM12");

Component* plaser1 = static_cast<Component*>(&l1);
Component* plaser2 = static_cast<Component*>(&l2);

ICameraController* pmicIm_1;

ICameraController* pmicIm_2;



ILaserController* pmlaser;
ILaserController* pmlaser2;

vector<IMotionController*>motions;



void Connect()

{
	cout << "Connection in loading" << endl;
	char szUSB[300];
	controllers = controllerDetection(szUSB, 299);

	for (int i = 0; i < controllers.size(); i++)
	{
		Component* p1 = static_cast<Component*> (&controllers.at(i));
		motions.push_back(static_cast<IMotionController*> (&controllers.at(i)));
		microscope.add(p1);
		cout << controllers.at(i).getIdentification() << "was added to the Composite" << endl;


	}

	microscope.add(cameraComponent);
	printf("Camera was succesfful added to the Composite \n ");

	microscope.add(flimCameraComponent);
	printf("Flim Camera was succesfful added to the Composite \n ");


	pmicIm_1 = static_cast<ICameraController*> (&camera);

	pmicIm_1->GetCameraAndRecorderHandle(&cameraPointer, &cameraRecorderPointer);

	pmicIm_2 = static_cast<ICameraController*> (&flimCamera);

	pmicIm_2->GetCameraAndRecorderHandle(&flimCameraPointer, &flimCameraRecorderPointer);

	pmlaser = static_cast<ILaserController*> (&l1);

	pmlaser2 = static_cast<ILaserController*> (&l2);




	microscope.add(plaser1);
	printf("Laser 1 was succesfful added to the Composite \n ");
	microscope.add(plaser2);
	printf("Laser 2 was succesfful added to the Composite \n ");
	//pmicIm->SingleImageAquisition();
	microscope.Connect();

}

void MoveAbsolut(int stage, double position)
{
	if (stage != 0 && stage != 1 && stage != 2 && stage != 3)
	{
		printf("The selected stage doesn't Exist. Please select a stage between 0, 1 ,2 and 3\n ");

		
	}
	for (int i = 0; i < controllers.size(); i++)
	{
		if (motions.at(i)->GetStage() == stage)
		{
			motions.at(i)->MoveAbsolutXYZ(stage, position);
				break;
		}
	}

	Sleep(1000);
}



void MoveRelativ(int stage, double position)
{
	if (stage != 0 && stage != 1 && stage != 2 && stage != 3)
	{
		printf("The selected stage doesn't Exist. Please select a stage between 0, 1 ,2 and 3\n ");


	}
	for (int i = 0; i < controllers.size(); i++)
	{
		if (motions.at(i)->GetStage() == stage)
		{
			motions.at(i)->MoveRelativXYZ(stage, position);
			break;
		}
	}

	Sleep(1000);

}

void LaserOn(int wavelength)
{
	if (pmlaser->ReadWaveLength() == wavelength)
		pmlaser->LaserOn();
	else if (pmlaser2->ReadWaveLength() == wavelength)
		pmlaser2->LaserOn();
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;
}

void LaserOff(int wavelength)
{
	if (pmlaser->ReadWaveLength() == wavelength)
		pmlaser->LaserOff();
	else if (pmlaser2->ReadWaveLength() == wavelength)
		pmlaser2->LaserOff();
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;

}


void Disconnect()
{
	microscope.Disconnect();
}

void CameraSingleImageAquisition()
{
	char filename[200];
	sprintf_s(filename, "C:\\Users\\admin\\Documents\\Visual Studio 2019\\Bilder\\image.tif");
	pmicIm_1->SingleImageAquisition(cameraPointer,filename);

}

void FlimCameraSingleImageAquisition()
{
	char filename[200];
	sprintf_s(filename, "C:\\Users\\admin\\Documents\\Visual Studio 2019\\Bilder\\image.tif");
	pmicIm_2->SingleImageAquisition(flimCameraPointer,filename);
}

void GetActualModulation()
{
}

void GetModulation()
{

 }

void GetOperatingMode()
{

 }

double GetPowerInPercentage(int wavelength)
{
	double power = 0;
	if (pmlaser->ReadWaveLength() == wavelength)
		power = pmlaser->GetPowerInpercentage();
	else if (pmlaser2->ReadWaveLength() == wavelength)
		power = pmlaser2->GetPowerInpercentage();
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;
	return power;
 }



double ReadMaxPower(int wavelength)
{
	double maxPower = 0;
	if (pmlaser->ReadWaveLength() == wavelength)
		maxPower = pmlaser->ReadMaxPower();
	else if (pmlaser2->ReadWaveLength() == wavelength)
		maxPower = pmlaser2->ReadMaxPower();
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;

	return maxPower;
 }



int ReadWaveLength()
{
	int waveLength = pmlaser->ReadWaveLength();
	return waveLength;
 }

void SetModulation(int wavelength, int modulation)
{
	if (pmlaser->ReadWaveLength() == wavelength)
		 pmlaser->SetModulation(modulation);
	else if (pmlaser2->ReadWaveLength() == wavelength)
		 pmlaser2->SetModulation(modulation);
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;
 }

void SetOperatingMode(int wavelength, int operantingMode)
{
	if (pmlaser->ReadWaveLength() == wavelength)
		pmlaser->SetOperatingMode(operantingMode);
	else if (pmlaser2->ReadWaveLength() == wavelength)
		pmlaser2->SetOperatingMode(operantingMode);
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;
 }

void SetPower(int wavelength, float power)
{
	if (pmlaser->ReadWaveLength() == wavelength)
		pmlaser->SetPower(power);
	else if (pmlaser2->ReadWaveLength() == wavelength)
		pmlaser2->SetPower(power);
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;
 }

DataPack* CameraLiveView()
{
	return pmicIm_1->Live_View(cameraPointer,cameraRecorderPointer);
}

DataPack* FLimCameraLiveView()
{
	return pmicIm_2->Live_View(flimCameraPointer, flimCameraRecorderPointer);

}

void Image_Sequence(const char* path, double zUp, double zDown, double xValue, double yValue, double focusUp, double focusDown, int N, double dz)
{
	vector<double>zPositions;
	vector<double>focusPositions;
	vector<double>xPositions;
	vector<double>yPositions;

	zPositions.push_back(zUp);

	for (int i = 1; i <= N; i++)
	{
		double z_new = zUp + i * dz;
		zPositions.push_back(z_new);
	}


	focusPositions.push_back(focusUp);

	for (int i = 1; i <= N; i++)
	{
		double dFocus = ((focusUp - focusDown) / (zUp - zDown) * dz);
		double focusPositon = dFocus + i * focusDown;
		focusPositions.push_back(focusPositon);
	}

	for (int i = 0; i < N; i++)
	{
		xPositions.push_back(xValue);
		yPositions.push_back(yValue);
	}

	pmicIm_1->Start_Sequence(cameraPointer);


	for (int i = 0; i < N; i++)
	{
		char filename[200];
		strcpy_s(filename, path);
		sprintf_s(filename, "image_%02d.tif", i);
		cout << filename << endl;
		MoveAbsolut(0, xPositions.at(i));
		MoveAbsolut(1, yPositions.at(i));
		MoveAbsolut(2, zPositions.at(i));
		MoveAbsolut(3, focusPositions.at(i)); 

		
		pmicIm_1->SingleImageAquisition(cameraPointer,filename);
		Sleep(1000);

	}
}
bool SetFilmPhaseSequenceParameter(int phaseNumber, int phaseSymmetry, int phaseorder, int tapSelect)
{
	PhaseNumber phasenumber = static_cast<PhaseNumber>(phaseNumber);
	PhaseSymmetry phasesymmetry = static_cast<PhaseSymmetry>(phaseSymmetry);
	PhaseOrder phaseOrder = static_cast<PhaseOrder>(phaseorder);
	TapSelect tapselect = static_cast<TapSelect>(tapSelect);

	bool result = pmicIm_2->SetFilmPhaseSequenceParameter(phasenumber, phasesymmetry, phaseOrder, tapselect);

	return result;
}

bool GetFilmPhaseSequenceParameter(int* phaseNumber, int* phaseSymmetry, int* phaseorder, int* tapSelect)
{
	PhaseNumber phasenumber = static_cast<PhaseNumber>(*phaseNumber);
	PhaseSymmetry phasesymmetry = static_cast<PhaseSymmetry>(*phaseSymmetry);
	PhaseOrder phaseOrder = static_cast<PhaseOrder>(*phaseorder);
	TapSelect tapselect = static_cast<TapSelect>(*tapSelect);

	bool result = pmicIm_2->GetFilmPhaseSequenceParameter(&phasenumber, &phasesymmetry, &phaseOrder, &tapselect);

	return result;
}


