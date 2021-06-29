//-----------------------------------------------------------------//
// Name        | TreiberImplementierung.cpp         | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     |Master Praxisarbeit  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS                                           //
//-----------------------------------------------------------------//
// Environment | Microsoft VisualStudio                            //
//             |                                                   //
//-----------------------------------------------------------------//
// Notes       | *Includeverzeichnis muss modifiziert weden  //
//             | 1- Full Path zu dem headerdile eingeben  //
//             | 2- Bei Eigentschaften in der "Includeverzeichnisse" //
//             |            Option der Include File auswählen
//             | *Usage of Lib file to link dll statically in MS VC++//
//                   - make sure the linker finds the PI_GCS2_DLL.lib//
//                       (Eigentschaften->Linker->Eingabe->Zusätzliche Abhängigkeiten->"PI_GCS2_DLL.lib"//
//                   - make sure the PI_GCS2_DLL.dll is accessible at program launch    //
//                    Add the Sc2_cam.lib library to the project. To do so, import the lib64 file where all the libraries necessary 
//                     for the project are contained. Then go to properties-> Linker -> Eingabe_> Zusätzliche Abhängigkeiten and write
//                     the Pfad where is contained the file "..\lib64\sc2_cam.lib"
//                   -make sure the Sc2_cam.dll is accessible at program launch
//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
// Author      | Dilane Chanja                                      //
//-----------------------------------------------------------------//

#include<iostream>
#include<String>
#include "Component.h"
#include "GlobalFunctions.h"
#include "LSFM_Camera.h"
#include "LSFM_Laser.h"
#include "ILaserController.h"
#include "LSFM_Motion_PI.h"
#include "LSFM_FLIM_Camera.h"
#include "Composite.h"
#include "IFilterController.h"
#include "XagYl_Filter.h"



using namespace std;


char szUSB[300];
vector<LSFM_Motion_PI> controllers = controllerDetection(szUSB, 299);
Composite microscope;



HANDLE flimcamhandle = NULL;
HANDLE flimcamrecorderhandle = NULL;

HANDLE camHandle = NULL;
HANDLE camRecorderHandle = NULL;

LSFM_Laser l1("\\\\.\\COM3");
LSFM_Laser l2("\\\\.\\COM4");

LSFM_Filter filter;
Component* pFilter = static_cast<Component*>(&filter);

IFilterController* filterController = static_cast<IFilterController*>(&filter);


Component* plaser1 = static_cast<Component*>(&l1);
Component* plaser2 = static_cast<Component*>(&l2);

LSFM_Camera camera;
Component* p4 = static_cast<Component*>(&camera);
ICameraController* cameracontroller = static_cast<ICameraController*>(&camera);

LSFM_FLIM_Camera flim_Camera;
Component* p3 = static_cast<Component*>(&flim_Camera);
ICameraController* flimCameracontroller = static_cast<ICameraController*>(&flim_Camera);



ILaserController* pmlaser = static_cast<ILaserController*>(&l1);
ILaserController* pmlaser2 = static_cast<ILaserController*>(&l2);

vector<IMotionController*>motions;


void Connect()

{
	
	try {

		/*
		cout << "Connection in loading" << endl;
		
		if (controllers.size())
		{
			for (int i = 0; i < controllers.size(); i++)
			{
				Component* p1 = static_cast<Component*> (&controllers.at(i));
				motions.push_back(static_cast<IMotionController*> (&controllers.at(i)));
				microscope.add(p1);
				cout << controllers.at(i).getIdentification() << "was added to the Composite" << endl;


			}
		}
		else
		{
			cout << "Any Controller to disposition" << endl;
			cout << "Please check your device" << endl;
		}
		*/

		//microscope.add(p4);
		//printf("PCO Camera was succesfful added to the Composite \n ");


		microscope.add(p3);
		printf("Flim Camera was succesfful added to the Composite \n ");


		//microscope.add(plaser1);
		//printf("Laser 1 was succesfful added to the Composite \n ");
		//microscope.add(plaser2);
		//printf("Laser 2 was succesfful added to the Composite \n ");

		//microscope.add(pFilter);
		microscope.Connect();

		//cameracontroller->GetCameraAndRecorderHandle(&camHandle, &camRecorderHandle);
		flimCameracontroller->GetCameraAndRecorderHandle(&camHandle, &camRecorderHandle);
	}

	catch (exception& e)
	{
		cout << "Standard exception: " << e.what() << endl;
	}
	

}

void Disconnect()
{
	microscope.Disconnect();
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

void DarkModeImageSequence(const char* path)
{
	pmlaser->LaserOff();
	pmlaser2->LaserOff();

	
		char filename[200];
		strcpy_s(filename, path);
		sprintf_s(filename, "Dark_");
		


		cameracontroller->SingleImageAquisition(2, flimcamrecorderhandle, flimcamhandle, 4, 2, 0, filename);
		Sleep(1000);

}

void NormalModeImageSequence(const char* path, int N, vector<double>zPositions, vector<double>focusPositions, vector<double>xPositions, vector<double>yPositions)
{
	pmlaser->LaserOn();
	pmlaser2->LaserOn();
	for (int i = 0; i < N; i++)
	{
		char filename[200];
		strcpy_s(filename, path);
		sprintf_s(filename, "Flim_img_z_%02d_focus%02d_", zPositions.at(i), focusPositions.at(i));
		MoveAbsolut(0, xPositions.at(i));
		MoveAbsolut(1, yPositions.at(i));
		MoveAbsolut(2, zPositions.at(i));
		MoveAbsolut(3, focusPositions.at(i));


		cameracontroller->SingleImageAquisition(2, flimcamrecorderhandle, flimcamhandle, 4, 2, 0, filename);
		Sleep(1000);

	}

	pmlaser->LaserOff();
	pmlaser2->LaserOff();
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


	cameracontroller->Start_Sequence(flimcamhandle);

	DarkModeImageSequence(path);

	NormalModeImageSequence(path, N, zPositions, focusPositions, xPositions, yPositions);

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

void Live_View()
{
	DataPack *datapack;
	datapack = cameracontroller->Live_View(1, camHandle, camRecorderHandle);

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

void GetActualPositions(double *xPosition, double *yPosition, double *zPosition, double *focusPosition)
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
}

 
void ConnectedCamera(int *listOfAvailableCamera)
{
	int count = 0;
	if (cameracontroller->GetCameraStatusConnection())
		count++;
	if (flimCameracontroller->GetCameraStatusConnection())
		count++;

	switch(count)
	{
	case 0:
		listOfAvailableCamera[0] = 0;
		break;
	case 1:
	{
		if (cameracontroller->GetCameraStatusConnection())
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

void SetPower(int value, float power)
{
	int value1 = pmlaser->ReadWaveLength();
	int value2 = pmlaser2->ReadWaveLength();

	if (value == value1)
		pmlaser->SetPower(power);
	else if (value == value2)
		pmlaser2->SetPower(power);
	else
		cout << "The entry value has not been recognized, please enter a good value." << endl;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void SetExposureTime(int camera, int exposureTime, int timeBase)
{
	if (camera == 1)
	{
		DWORD dwExpousreTime = DWORD(exposureTime);
		DWORD dwTimebase = DWORD(timeBase);
		//cameracontroller->SetExposureTime(camHandle, dwExpousreTime, dwTimebase);
		flimCameracontroller->SetExposureTime(camHandle, dwExpousreTime, dwTimebase);

	}
}

int  GetExposureTime(int camera, int* exposureTime, int* expousretimeBase)
{
	if (camera == 1)
	{
		WORD dwexposureTime, dw_exposureTimebase;
		cameracontroller->GetExposureAndDelayTime(camHandle, &dwexposureTime, &dw_exposureTimebase);

		*exposureTime = dwexposureTime;
		*expousretimeBase = dw_exposureTimebase;

		int test = *exposureTime;

		return test;
	}
}

void GetAllpositionsName()
{

	char filterPosition[400];
	filterController->GetAllPositionName(filterPosition);
	
}

void MoveFilterToPosition()
{
	int position = filterController->ActualFIlterPosition();
	filterController->MoveToSelectedPosition(2);
	position = filterController->ActualFIlterPosition();
	filterController->MoveToSelectedPosition(1);
	position = filterController->ActualFIlterPosition();
}




int main()
{
	
	srand((unsigned int)time(NULL));


	
	Connect();

	SetExposureTime(1, 1000, 2);
	char filenemame[100];
	sprintf_s(filenemame, "test1.tif");
	//cameracontroller->SingleImageAquisition(2, camRecorderHandle, camHandle, NULL, NULL, NULL, filenemame);
	flimCameracontroller->SingleImageAquisition(2, camRecorderHandle, camHandle, PhaseNumber::Four, PhaseSymmetry::twice, TapSelect::both, filenemame);


	//int listofConnectedControllers[4];

	//ConnectedCamera(listofConnectedCamera);
	
	//GetConnectedMotions(listofConnectedControllers);
	

	//GetAllpositionsName();

	//MoveFilterToPosition();
	



	


	
	//Live_View();

	/*
	SetExposureTime(1, 100, 2);
	char filenemame [100];
	sprintf_s(filenemame, "test1.tif");
	cameracontroller->SingleImageAquisition(1, camRecorderHandle, camHandle, NULL, NULL, NULL, filenemame);
	SetExposureTime(1, 1000, 2);
	char filenemame1[100];
	sprintf_s(filenemame1, "test2.tif");
	cameracontroller->SingleImageAquisition(1, camRecorderHandle, camHandle, NULL, NULL, NULL, filenemame1);

	**/
	

	Disconnect();

	




	
		

	printf("Ende");



	return 0;
}