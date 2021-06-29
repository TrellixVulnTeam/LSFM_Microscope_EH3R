#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<String>
#include <windows.h>
#include <vector>
#include <sstream>
#include "Component.h"
#include "ILaserController.h"
#include "GlobalFunctions.h"


using namespace std;

class LSFM_Laser : public Component, public ILaserController
{
private:
	bool laserStatus = false;
	char* serialPort;
	int actualModulation = 0;
	int modulation = 0; // variable to retireve the modulation of the laser 
	bool laserConnected = false;//variable to check if the laser is connected 
	BOOL   Status; // Status
	HANDLE serialHandle;
	DCB dcbSerialParams = { 0 };  // Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure

	DWORD BytesWritten = 0;          // No of bytes written to the port
	DWORD dwEventMask;     // Event mask to trigger
	char  ReadData;        //temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()



	char* SendCommand(char* SerialBuffer);

	const  char* hex_char_to_bin(char c);

	std::string hex_str_to_bin_str(char* responseCommand);

	void ErrorHandling();

	float extractValueFromResponse(char* response, std::string delimiter);

	void Power_On();

public:
	LSFM_Laser(std::string serialPortInput);
	void Connect() override;

	void Disconnect() override;

	void add(Component* component) override;

	void remove(Component* component) override;

	void LaserOn() override;

	void LaserOff() override;

	int GetActualModulation() override;


	void SetModulation(int modulation) override;


	int  GetModulation() override;

	int ReadWaveLength() override;


	void ReadStatus() override;

	float ReadMaxPower() override;

	void SetPower(float power) override;

	float GetPowerInpercentage() override;

	void GetOperatingMode() override;

	void SetOperatingMode(int OperatingMode) override;

	bool IsHardwareConnected() override;

};