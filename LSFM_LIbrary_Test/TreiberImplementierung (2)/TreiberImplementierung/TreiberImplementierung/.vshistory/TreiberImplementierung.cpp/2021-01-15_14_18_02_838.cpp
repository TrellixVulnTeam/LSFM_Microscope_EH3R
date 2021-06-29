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
#include "Composite.h"

using namespace std;
int main()
{
	
	srand((unsigned int)time(NULL));
	double dVal = -1;
	const BOOL flag = true;
	char szUSB[300];

	Composite microscope;


	vector<LSFM_Motion_PI> controllers = controllerDetection(szUSB, 299);

	if (controllers.size() != 0)
	{
		for (int i = 0; i < controllers.size(); i++)
		{
			Component* p1 = static_cast<Component*> (&controllers.at(i));
			microscope.add(p1);
			printf("%s was added to the Composite. \n ", controllers.at(i).getIdentification());

		}
	}
	
	
	LSFM_Camera camera;
	Component* p2 = static_cast<Component*>(&camera);
	microscope.add(p2);
	printf("Camera was succesfful added to the Composite \n ");

	microscope.Connect();

	microscope.MoveAbsolutXYZ(3, 2.0);

	

	microscope.Disconnect();

	printf("Ende");




	return 0;
}