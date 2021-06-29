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





	microscope.add(plaser1);
	printf("Laser 1 was succesfful added to the Composite \n ");
	microscope.add(plaser2);
	printf("Laser 2 was succesfful added to the Composite \n ");
	//pmicIm->SingleImageAquisition();
	microscope.Connect();

}

void Disconnect()
{
	microscope.Disconnect();
}

