#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<String>
#include <windows.h>
#include <vector>
#include <sstream>
#include "Component.h"
#include "ILaserController.h"


using namespace std;

class LSFM_Laser : public Component, public ILaserController
{
private:
	bool laserStatus = false;
	char* serialPort;  // COM port of the devices 
	int actualModulation = 0;
	int specPower = 0; // Power specification for the laser
	int modulation = 0; // variable to retireve the modulation of the laser 
	bool laserConnected = false;//variable to check if the laser is connected 
	BOOL   Status; // Status
	HANDLE serialHandle;  // Handle for the connection with the devices 
	DCB dcbSerialParams = { 0 };  // Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure

	DWORD BytesWritten = 0;          // No of bytes written to the port
	DWORD dwEventMask;     // Event mask to trigger
	char  ReadData;        //temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()



	void SendCommand(char* SerialBuffer);  // Method to send a command to the Device

	const  char* hex_char_to_bin(char c); // Method to convert hexadecimal value into binary value 

	std::string hex_str_to_bin_str(char* responseCommand);  // Method to convert a Hexademical respnse to  String 

	void ErrorHandling();

	float extractValueFromResponse(char* response, std::string delimiter);  // Method to Extract a value from a response of a Device 

	void Power_On();  // Set the power of the Laser of ON 

	vector<string> Stringdelimiter(string s1, string delimiter); // Method to split a String


public:
	LSFM_Laser(std::string serialPortInput);  // Constructor of the Class 

	void Connect() override;   // connect the Device with the Computer 

	void Disconnect() override;  // Disconnect the Device with the Computer 

	void add(Component* component) override;  // Method to add the Leafcomponent laser to a Composite Microscope 

	void remove(Component* component) override; // Method to remove the Leafcomponent laser to a Composite Microscope

	void LaserOn() override;  // Set the laser to ON 

	void LaserOff() override; // Set the laser to OFF

	int GetActualModulation() override; // Retrieve the actual Modulation of the Laser 


	void SetModulation(int modulation) override; // Set the a Modulation to he the Laser


	int  GetModulation() override;

	int ReadWaveLength() override;  // Method to determine the wavelength of the laser 


	void ReadStatus() override;  // Read a status of the laser 

	float ReadMaxPower() override;  // Read the maximal power intensity of the laser 

	void SetPower(float power) override; // Set the Power intensity of the laser 

	float GetPowerInpercentage() override;  // Retrieve the power intensity of the laser in percenetage 

	void GetOperatingMode() override;  // Retrieve the operating mode of the Laser 

	void SetOperatingMode(int OperatingMode) override; // Set the Operating mode of the Device 

	bool IsHardwareConnected() override; // Method to check if the connection with laser was succesfull 

	void SetAnalogImpedanz(int impedanz) override;  // Set analog impedanz to a laser 

	int ReadSpecPower() override; // Read the Specification Power; 


};