// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "LSFMLibrary.h"
#include <iostream>
#include <String>
#include <vector>


// DLL internal state variables:
static unsigned long long previous_;  // Previous value, if any
static unsigned long long current_;   // Current sequence value
static unsigned index_;               // Current seq. position



HANDLE cameraPointer, flimCameraPointer, cameraRecorderPointer, flimCameraRecorderPointer;

vector<LSFM_Motion_PI> controllers;

vector<LSFM_Laser> lasersList;

Composite microscope;

LSFM_Camera camera;
Component* cameraComponent = static_cast<Component*>(&camera);

LSFM_Camera flimCamera;
Component* flimCameraComponent = static_cast<Component*>(&flimCamera);

LSFM_Filter filter;
Component* pFilter = static_cast<Component*>(&filter);

IFilterController* filterController;


ICameraController* pmicIm_1;

ICameraController* pmicIm_2;


vector<IMotionController*>motions;



vector<ILaserController*>lasers;




void Connect()

{
	try
	{

		/*
		
		const char* motionsConfigFile = "C:\\LSFM_Software\\TreiberImplementierung\\LSFMLibrary\\ConfigFile.txt";

       
		cout << "Connection in loading" << endl;
		char szUSB[300];
		controllers = controllerDetection(szUSB, 299, motionsConfigFile);

		for (int i = 0; i < controllers.size(); i++)
		{
			Component* p1 = static_cast<Component*> (&controllers.at(i));
			motions.push_back(static_cast<IMotionController*> (&controllers.at(i)));
			microscope.add(p1);


		}
		*/
		
		const char* laserConfigFile = "C:\\LSFM_Software\\TreiberImplementierung\\LSFMLibrary\\LaserConfigFile.txt";

		lasersList = LaserPortDetection(laserConfigFile);


		if (lasersList.size())
		{
			for (int i = 0; i < lasersList.size(); i++)
			{
				Component* p1 = static_cast<Component*> (&lasersList.at(i));
				lasers.push_back(static_cast<ILaserController*> (&lasersList.at(i)));
				microscope.add(p1);

			}
			

		}
		else
		{
			cout << "Any Laser  to disposition" << endl;
			cout << "Please check your device" << endl;
		}
		
		

		

		

		microscope.add(cameraComponent);
		printf("PCO edge  Camera was succesfful added to the Composite \n");

		microscope.add(flimCameraComponent);
		printf("Flim Camera was succesfful added to the Composite \n ");

		

		pmicIm_1 = static_cast<ICameraController*> (&camera);


		pmicIm_2 = static_cast<ICameraController*> (&flimCamera);


		filterController = static_cast<IFilterController*>(&filter);
			
		microscope.add(pFilter);

		microscope.Connect();

		

	}
	catch (exception& e)
	{
		cout << "Standard exception: " << e.what() << endl;
	}

}

void GetConnectedMotions(int* listOfAvailabeMotion)
{

	if (controllers.size() > 0)
	{
		for (int i = 0; i < controllers.size(); i++)
		{
			if (motions.at(i)->GetMotionStatusConnection())
				listOfAvailabeMotion[i] = motions.at(i)->GetStage();

		}
	}

}

bool GetActualPositions(double* xPosition, double* yPosition, double* zPosition, double* focusPosition)
{

	for (int i = 0; i < controllers.size(); i++)
	{
		switch (motions.at(i)->GetStage())
		{
		case xRichtung:
			*xPosition = motions.at(i)->getActualPosition();
			break;
		case yRichtung:
			*yPosition = motions.at(i)->getActualPosition();
			break;

		case zRichtung:
			*zPosition = motions.at(i)->getActualPosition();
			break;

		case focus:
			*focusPosition = motions.at(i)->getActualPosition();
			break;

		default:
			break;
		}

	}

	return true;

}

void MoveAbsolut(int stage, double position, bool acquisitionRunning)
{
	if (stage != 0 && stage != 1 && stage != 2 && stage != 3)
	{
		printf("The selected stage doesn't Exist. Please select a stage between 0, 1 ,2 and 3\n ");


	}
	
	for (int i = 0; i < controllers.size(); i++)
	{
		if (motions.at(i)->GetStage() == stage)
		{
			motions.at(i)->MoveAbsolutXYZ(stage, position, acquisitionRunning);
			break;
		}
	}

}

int GetStageActualPosition(int stage)
{
	int position = 0;

	if (stage != 0 && stage != 1 && stage != 2 && stage != 3)
	{
		printf("The selected stage doesn't Exist. Please select a stage between 0, 1 ,2 and 3\n ");


	}
	for (int i = 0; i < controllers.size(); i++)
	{
		if (motions.at(i)->GetStage() == stage)
		{
			position = motions.at(i)->getActualPosition();
			break;
		}
	}

	return position;
}

void MoveRelativ(int stage, double position, bool acquisitionRunning)
{
	if (stage != 0 && stage != 1 && stage != 2 && stage != 3)
	{
		printf("The selected stage doesn't Exist. Please select a stage between 0, 1 ,2 and 3\n ");


	}
	for (int i = 0; i < controllers.size(); i++)
	{
		if (motions.at(i)->GetStage() == stage)
		{
			motions.at(i)->MoveRelativXYZ(stage, position, acquisitionRunning);
			break;
		}
	}


}

void LaserOn(int wavelength, int specPower)
{
	
	for (int i = 0; i < lasersList.size(); i++)
	{
		
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			cout << "Test" << endl;
			lasers.at(i)->LaserOn();
			break;
		}
		
	}
	
}

void LaserOff(int wavelength, int specPower)
{
	
	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			lasers.at(i)->LaserOff();
			break;
		}

	}
	
}


void Disconnect()
{
	microscope.Disconnect();
	for (int i = 0; i < controllers.size(); i++)
		{
			Component* p1 = static_cast<Component*> (&controllers.at(i));
			microscope.remove(p1);
		}

	for (int i = 0; i < lasersList.size(); i++)
	{
		Component* p1 = static_cast<Component*> (&lasersList.at(i));
		microscope.remove(p1);
	}


	microscope.remove(cameraComponent);

	microscope.remove(flimCameraComponent);

	microscope.remove(pFilter);

	motions.clear();
}

bool SingleImageAquisition(int cameratyp, char* filename)
{
	bool result;


   //int imgWidth, imgHeight;
   //vector<WORD*>bufferList = flimCameracontroller->Live_View(2, camHandle, camRecorderHandle, &imgWidth, &imgHeight);

	if (cameratyp == pmicIm_1->GetCameraIdentification())
	{
		result = pmicIm_1->SingleImageAquisition(filename);

	}
	else
	{

		result = pmicIm_2->SingleImageAquisition(filename);

		return result;
	}




}



void  GetExposureTime(int camera, int* exposureTime, int* delay)
{
	if (camera == pmicIm_1->GetCameraIdentification())
	{
		WORD dwexposureTime, dw_delay;
		pmicIm_1->GetExposureAndDelayTime(&dwexposureTime, &dw_delay);

		cout << dwexposureTime << dw_delay << endl;
		*exposureTime = int(dwexposureTime);
		*delay = int(dw_delay);
	}

	else
	{
		WORD dwexposureTime, dw_delay;
		pmicIm_2->GetExposureAndDelayTime(&dwexposureTime, &dw_delay);

		cout << dwexposureTime << dw_delay << endl;
		*exposureTime = int(dwexposureTime);
		*delay = int(dw_delay);
	}

}


int GetActualModulation(int wavelength, int specPower)
{
	int actualModulation = 0;


	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength)
		{
			actualModulation = lasers.at(i)->GetActualModulation(); 
			break;
		}

	}

	return actualModulation;
}

void SetExposureTime(int camera, int exposureTime, int delay)
{
	if (camera == pmicIm_1->GetCameraIdentification())
	{
		DWORD dwExpousreTime = DWORD(exposureTime);
		DWORD dwTimebase = DWORD(delay);
		pmicIm_1->SetExposureTime(exposureTime, delay);

	}

	else
	{
		DWORD dwExpousreTime = DWORD(exposureTime);
		DWORD dwTimebase = DWORD(delay);
		pmicIm_2->SetExposureTime(exposureTime, delay);
	}
}

int GetModulation(int wavelength, int specPower)
{
	int operatingMode = 0;
	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			operatingMode = lasers.at(i)->GetModulation();
			break;
		}

	}


	return operatingMode;
}


 bool GetWaveLengthAndSpecPower(Laserpack* LaserList)
{


	 for (int i = 0; i < lasersList.size(); i++)
	 {


		 Laserpack laserpack;
		 if (lasers.at(i)->IsHardwareConnected())
		 {
			 laserpack.waveLegnth = lasers.at(i)->ReadWaveLength();
			 laserpack.specPower = lasers.at(i)->ReadSpecPower();
			 LaserList[i] = laserpack;
		 }

	 }


	return true;
}


double GetPowerInPercentage(int wavelength, int specPower)
{
	double power = 0;


	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			power = lasers.at(i)->GetPowerInpercentage();

			break;
		}

	}
	

	return power;
}


double ReadMaxPower(int wavelength, int specPower)
{
	double maxPower = 0;

	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			maxPower = lasers.at(i)->ReadMaxPower();
			break;
		}

	}


	return maxPower;
}



void SetModulation(int wavelength, int specPower, int modulation)
{
	
	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			lasers.at(i)->SetModulation(modulation);
			break;
		}

	}

	
}

void SetOperatingMode(int wavelength, int specPower,  int operantingMode)
{

	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			lasers.at(i)->SetOperatingMode(operantingMode);
			break;
		}

	}

	
}

void SetPower(int wavelength, int specPower, float power)
{
	
	for (int i = 0; i < lasersList.size(); i++)
	{
		if (lasers.at(i)->ReadWaveLength() == wavelength && lasers.at(i)->ReadSpecPower() == specPower)
		{
			lasers.at(i)->SetPower(power);
			break;
		}

	}

	
}

DataPack* LiveView(int camera)
{
	if (camera == pmicIm_1->GetCameraIdentification())
		return pmicIm_1->Live_View();

	else
		return pmicIm_2->Live_View();
		
}

bool SetFlimParamter(int camera , int phaseNumber, int frequency)
{

	if (camera == pmicIm_1->GetCameraIdentification())
	{
		PhaseNumber phaseNumber1 = static_cast<PhaseNumber>(phaseNumber);


		pmicIm_1->SetFlimPhaseSequenceParameter(phaseNumber1, PhaseSymmetry::twice, PhaseOrder::opposite, TapSelect::both, frequency);

		//pmicIm_2->Set_Camera_Master_modulation_frequency(frequency);

		return true;
	}

	else
	{

		PhaseNumber phaseNumber1 = static_cast<PhaseNumber>(phaseNumber);


		pmicIm_2->SetFlimPhaseSequenceParameter(phaseNumber1, PhaseSymmetry::twice, PhaseOrder::opposite, TapSelect::both, frequency);

		//pmicIm_2->Set_Camera_Master_modulation_frequency(frequency);

		return true;
	}
	
	

}



void Image_Sequence(int cameraType, const char* path, double zUp, double zDown, double xValue, double yValue, double focusUp, double focusDown, int N, double dz, int phaseNumber)
{
	vector<double>zPositions;
	vector<double>focusPositions;
	vector<double>xPositions;
	vector<double>yPositions;

	zPositions.push_back(zUp);

	for (int i = 1; i < N; i++)
	{
		double z_new = zUp + i * dz;
		zPositions.push_back(z_new);
	}



	focusPositions.push_back(focusUp);

	for (int i = 1; i < N; i++)
	{
		double dFocus = ((focusUp - focusDown) / (zUp - zDown)) * dz;
		double focusPositon = dFocus * i + focusUp;
		focusPositions.push_back(focusPositon);
	}


	for (int i = 0; i < N; i++)
	{
		xPositions.push_back(xValue);
		yPositions.push_back(yValue);
	}

	pmicIm_1->Start_Sequence();


	if (cameraType == pmicIm_1->GetCameraIdentification())
	{
		for (int i = 0; i < N; i++)
		{

			char filename[200];
			//char filename1[200];
			//char filename2[200];
			//char filename3[200];

			sprintf_s(filename, "\\image_%03d.tif", i);
			//sprintf_s(filename1, "\\image_%03d.tif", (4*i +1));
			//sprintf_s(filename2, "\\image_%03d.tif", (4*i +2));
			//sprintf_s(filename3, "\\image_%03d.tif", (4*i +3));

			char* TotalLine{ new char[strlen(path) + strlen(filename) + 1] };
			TotalLine = strcpy(TotalLine, path);
			TotalLine = strcat(TotalLine, filename);

			/*
			char* TotalLine1{ new char[strlen(path) + strlen(filename1) + 1] };
			TotalLine1 = strcpy(TotalLine1, path);
			TotalLine1 = strcat(TotalLine1, filename1);

			char* TotalLine2{ new char[strlen(path) + strlen(filename2) + 1] };
			TotalLine2 = strcpy(TotalLine2, path);
			TotalLine2 = strcat(TotalLine2, filename2);

			char* TotalLine3{ new char[strlen(path) + strlen(filename3) + 1] };
			TotalLine3 = strcpy(TotalLine3, path);
			TotalLine3 = strcat(TotalLine3, filename3);
			*/

			//MoveAbsolut(0, xPositions.at(i));
			//MoveAbsolut(1, yPositions.at(i));
			MoveAbsolut(2, zPositions.at(i), 1);
			MoveAbsolut(3, focusPositions.at(i), 1);


			pmicIm_1->SingleImageAquisition(TotalLine);

			delete[] TotalLine;


			/*
			MoveRelativ(0, 0.001,1); // Move Raltiv x+1 and y

			pmicIm_1->SingleImageAquisition(1, cameraRecorderPointer, cameraPointer, NULL, NULL, NULL, TotalLine1);

			MoveRelativ(0, -0.001, 1); // Move relativ x and y+1
			MoveRelativ(1, +0.001, 1); // Move relativ x and y+1

			pmicIm_1->SingleImageAquisition(1, cameraRecorderPointer, cameraPointer, NULL, NULL, NULL, TotalLine2);


			MoveRelativ(0, +0.001, 1); // Move relativ x+1 and y+1
			pmicIm_1->SingleImageAquisition(1, cameraRecorderPointer, cameraPointer, NULL, NULL, NULL, TotalLine3);

			*/


			Sleep(500);
		}

	}

	else
	{
		for (int i = 0; i < N; i++)
		{

			char filename[200];
			

			sprintf_s(filename, "\\image_%04d", i);
			
			char* TotalLine{ new char[strlen(path) + strlen(filename) + 1] };
			TotalLine = strcpy(TotalLine, path);
			TotalLine = strcat(TotalLine, filename);

			
			MoveAbsolut(2, zPositions.at(i), 1);
			MoveAbsolut(3, focusPositions.at(i), 1);


			PhaseNumber phasenumber1 = static_cast<PhaseNumber>(phaseNumber);
			pmicIm_2->SingleImageAquisition(TotalLine);

			delete[] TotalLine;

			/*
			MoveRelativ(0, 0.001,1); // Move Raltiv x+1 and y

			pmicIm_1->SingleImageAquisition(1, cameraRecorderPointer, cameraPointer, NULL, NULL, NULL, TotalLine1);

			MoveRelativ(0, -0.001, 1); // Move relativ x and y+1
			MoveRelativ(1, +0.001, 1); // Move relativ x and y+1

			pmicIm_1->SingleImageAquisition(1, cameraRecorderPointer, cameraPointer, NULL, NULL, NULL, TotalLine2);


			MoveRelativ(0, +0.001, 1); // Move relativ x+1 and y+1
			pmicIm_1->SingleImageAquisition(1, cameraRecorderPointer, cameraPointer, NULL, NULL, NULL, TotalLine3);

			*/


			Sleep(500);
		}

	}

}



bool GetFilmPhaseSequenceParameter(int* phaseNumber, int* phaseSymmetry, int* phaseorder, int* tapSelect)
{
	PhaseNumber phasenumber = static_cast<PhaseNumber>(*phaseNumber);
	PhaseSymmetry phasesymmetry = static_cast<PhaseSymmetry>(*phaseSymmetry);
	PhaseOrder phaseOrder = static_cast<PhaseOrder>(*phaseorder);
	TapSelect tapselect = static_cast<TapSelect>(*tapSelect);

	bool result = pmicIm_2->GetFlimPhaseSequenceParameter(&phasenumber, &phasesymmetry, &phaseOrder, &tapselect);

	return result;
}

void GetConnectedCamera(int* listOfAvailableCamera)
{
	int count = 0;
	if (pmicIm_1->GetCameraStatusConnection())
		count++;
	if (pmicIm_2->GetCameraStatusConnection())
		count++;

	switch (count)
	{
	case 0:
		listOfAvailableCamera[0] = 0;
		break;
	case 1:
	{
		if (pmicIm_1->GetCameraIdentification() == 1)
			listOfAvailableCamera[0] = 1;
		else
			listOfAvailableCamera[0] = 2;

		break;
	}

	case 2:
	{
		listOfAvailableCamera[0] = 1;
		listOfAvailableCamera[1] = 2;
		break;
	}


	default:
		break;
	}
}

void  GetAllFiltersPositionsName(char* positionsList)
{
	filterController->GetAllPositionName(positionsList);
}

int MoveFilterToPosition(int position)
{
	filterController->MoveToSelectedPosition(position);
	int actposition = filterController->ActualFIlterPosition();

	return actposition;

}

int  GetFilterCurrentPosition()
{
	return filterController->ActualFIlterPosition();
}

void ListNotConnectedHardware(char* arraylist)
{
	string list;
	bool response;

	if (controllers.size() == 0)
		list = list + "PI C-863 Mercury SN 0185500512 // PI C - 413 PIMag Motion Controller SN 0118024559 // PI C - 863 Mercury SN 0185500508 // PI C - 863 Mercury SN 0185500511//";

	else
	{
		for (int i = 0; i < controllers.size(); i++)
		{
			response = motions.at(i)->IsHardwareConnected();

			if (!response)
			{
				char controllerIdentification[100];
				motions.at(i)->getIdentification(controllerIdentification);
				string s(controllerIdentification);
				list = list + s + "//";
			}
		}

	}

	/*
	response = pmlaser->IsHardwareConnected();
	if (!response)
	{
		int waveLength = pmlaser->ReadWaveLength();

		string s = "Laser Blue and Red ";
		list = list + s + "//";

	}

	*/

	response = pmicIm_1->IsHardwareConnected();
	if (response)
		list = list + "FLIM Camera " + "//";

	response = filterController->IsHardwareConnected();
	if (!response)
		list = list + "Xagyl Filter" + "//";


	strcpy(arraylist, list.c_str());
}


