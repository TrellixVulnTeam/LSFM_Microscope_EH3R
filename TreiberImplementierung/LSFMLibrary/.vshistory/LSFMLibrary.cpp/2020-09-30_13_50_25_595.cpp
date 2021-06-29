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

// DLL internal state variables:
static unsigned long long previous_;  // Previous value, if any
static unsigned long long current_;   // Current sequence value
static unsigned index_;               // Current seq. position



Composite microscope;

LSFM_Camera camera;
Component* p2 = static_cast<Component*>(&camera);

LSFM_Laser l1("\\\\.\\COM11");
LSFM_Laser l2("\\\\.\\COM12");

Component* plaser1 = static_cast<Component*>(&l1);
Component* plaser2 = static_cast<Component*>(&l2);

ICameraController* pmicIm;

ILaserController* pmlaser;
ILaserController* pmlaser2;



void Connect()

{
	cout << "Connection in loading" << endl;
	char szUSB[300];
	vector<LSFM_Motion_PI> controllers = controllerDetection(szUSB, 299);

	for (int i = 0; i < controllers.size(); i++)
	{
		Component* p1 = static_cast<Component*> (&controllers.at(i));
		microscope.add(p1);
		cout << controllers.at(i).getIdentification() << "was added to the Composite" << endl;

	}

	microscope.add(p2);
	printf("Camera was succesfful added to the Composite \n ");

	pmicIm = static_cast<ICameraController*> (&camera);

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
	microscope.MoveAbsolutXYZ(stage, position);
	Sleep(1000);
}

void MoveRelativ(int stage, double position)
{
	microscope.MoveRelativXYZ(stage, position);
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

void SingleImageAquisition()
{
	char filename[200];
	sprintf_s(filename, "C:\\Users\\admin\\Documents\\Visual Studio 2019\\Bilder\\image.tif");
	pmicIm->SingleImageAquisition(filename);

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


