#pragma once


#include<iostream>
#include<String>
#include <windows.h>
#include <time.h>
#include "Component.h"
#include "IMotionController.h"
#include "..\TreiberImplementierung\include\PI_GCS2_DLL.h"


// Enumeration to define the Stage of each Controller 
enum  Stage
{
	xRichtung = 0,
	yRichtung = 1,
	zRichtung = 2,
	focus = 3
};

class LSFM_Motion_PI : public Component, public IMotionController
{
private:
	int id = -1;
	double x0 = 0;
	double x1 = 0;
	double home = 0;
	double actPosition = 0;
	char* identification;
	const char* szAxis = "1";
	const BOOL flag = true;
	int stage = xRichtung;
	bool isPIC413 = false;


	double go_left();

	double go_right();

	void setReferencing();

	void CloseConnectionWithComment(const char* comment);

	void ReportError(int iD);

	bool ControllerReady();

	bool ControllerDone();



public:
	//constructor of the class. For each instantiation of an object, the identifier and stage of the controller is recommended.
	LSFM_Motion_PI(char* cidentification, int st);

	void add(Component* component) override;

	void remove(Component* component) override;

	double getActualPosition() override;

	void Connect() override;

	bool MoveRelativXYZ(int stage, double position) override;

	bool MoveAbsolutXYZ(int stage, double position) override;

	char* getIdentification();

	void Disconnect() override;


};

