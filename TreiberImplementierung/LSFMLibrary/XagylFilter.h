#pragma once
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<String>
#include <windows.h>
#include <vector>
#include <sstream>
#include "Component.h"
#include "GlobalFunctions.h"
#include "IFilterController.h"

using namespace std;


class LSFM_Filter : public Component, public IFilterController
{
private:
	string position1, position2, position3, position4, position5;
	int currentPosition;
	char* serialPort;
	bool filterConnected = false;//variable to check if the laser is connected 
	BOOL   Status; // Status
	HANDLE serialHandle;
	DCB dcbSerialParams = { 0 };  // Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure

	DWORD BytesWritten = 0;          // No of bytes written to the port
	DWORD dwEventMask;     // Event mask to trigger
	char  ReadData;        //temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()



	void SendCommand(char* SerialBuffer, int response);
	string GetElementFromConfigFIle(string element, vector<string>list1, vector<string>list2);
	int GetFilterCurrentPosition();
	void LoadConfigFile(const char* filePath, vector<string>* list1, vector<string>* list2);







public:
	LSFM_Filter();
	void Connect() override;

	void Disconnect() override;

	void add(Component* component) override;

	void remove(Component* component) override;

	bool MoveToSelectedPosition(int position) override;
	
	int ActualFIlterPosition() override;

	bool GetAllPositionName(char* positionsList) override;

	bool IsHardwareConnected() override;
};