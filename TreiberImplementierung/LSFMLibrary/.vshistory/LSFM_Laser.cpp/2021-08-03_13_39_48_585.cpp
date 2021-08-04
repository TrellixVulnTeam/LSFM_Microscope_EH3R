#include "pch.h"
#include "LSFM_Laser.h"

void LSFM_Laser::SendCommand(char* SerialBuffer)
{

	//Writing data to Serial Port
	Status = WriteFile(this->serialHandle,// Handle to the Serialport
		SerialBuffer,            // Data to be written to the port
		sizeof(SerialBuffer),   // No of bytes to write into the port
		&BytesWritten,  // No of bytes written to the port
		NULL);


	if (Status == FALSE)
	{
		printf_s("\nFail to Written");
		this->Disconnect();
	}


	//Setting Receive Mask
	Status = SetCommMask(this->serialHandle, EV_RXCHAR);
	if (Status == FALSE)
	{
		printf_s("\nError to in Setting CommMask\n\n");
		this->Disconnect();

	}
	//Setting WaitComm() Event
	Status = WaitCommEvent(this->serialHandle, &dwEventMask, NULL); //Wait for the character to be received
	if (Status == FALSE)
	{
		printf_s("\nError! in Setting WaitCommEvent()\n\n");
		this->Disconnect();
	}
	//Read data and store in a buffer
	unsigned char loop = 0;

	do
	{
		Status = ReadFile(this->serialHandle, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
		SerialBuffer[loop] = ReadData;

		if (ReadData == '\r')
			NoBytesRead = 0;

		++loop;
	} while (NoBytesRead > 0);


}

const  char* LSFM_Laser::hex_char_to_bin(char c)
{
	// convert a hexadecimal character into a binary in string form 
	switch (toupper(c))
	{
	case '0': return "0000";
	case '1': return "0001";
	case '2': return "0010";
	case '3': return "0011";
	case '4': return "0100";
	case '5': return "0101";
	case '6': return "0110";
	case '7': return "0111";
	case '8': return "1000";
	case '9': return "1001";
	case 'A': return "1010";
	case 'B': return "1011";
	case 'C': return "1100";
	case 'D': return "1101";
	case 'E': return "1110";
	case 'F': return "1111";
	}
}

string LSFM_Laser::hex_str_to_bin_str(char* responseCommand)
{

	/*
	* Method to convert a Hexadecimal Value into a binary value
	* input-> array of character 
	* output -> binary value in string form 
	*/

	string response(responseCommand);

	if (!response.empty())
	{
		char hexValue[4];

		int j = 0;
		for (int i = 4; i <= 7; i++)
		{
			hexValue[j] = responseCommand[i];
			j++;
		}
		string bin;

		for (unsigned i = 0; i != 4; ++i)
			bin += hex_char_to_bin(hexValue[i]);

		return bin;
	}

}

void LSFM_Laser::ErrorHandling()
{
	/*
	 Method for managing and handling errors. 
	
	*/
	if (this->laserConnected == true)
	{
		stringstream sstream;

		sstream << "?GFB" << "\r"; // Ask for the actual error status of the device

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer); // Send command 

		sstream << "?GLF" << "\r"; // Ask for the last error byte

		command = sstream.str();

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);

		string binaryValue = hex_str_to_bin_str(SerialBuffer); // convert hex to binary 

		for (int i = 0; i < 16; i++)
		{
			if (binaryValue.at(i) == '1')
			{
				switch (i)
				{
				case '0':
					cout << " Diode Power  error" << endl;
					break;
				case '1':
					cout << " internal error" << endl;
					break;
				case '2':
					cout << " test error" << endl;
					break;
				case '3':
					cout << " diode temperature" << endl;
					break;
				case '4':
					cout << " ambient temperature" << endl;
					break;
				case '5':
					cout << " diode current" << endl;
					break;
				case '6':
					cout << " external interlock" << endl;
					break;
				case '7':
					cout << " under/over voltage" << endl;
					break;
				case '8':
					cout << " High Power Controller needed" << endl;
					break;
				case '9':
					cout << " K1 relay error:" << endl;
					break;
				case '10':
					cout << " internal communication error" << endl;
					break;
				case '11':
					cout << " CDRH error:" << endl;
					break;

				default:
					break;
				}
			}

		}

	}

	else
		cout << "No LSFM_Laser connected" << endl;

}

float LSFM_Laser::extractValueFromResponse(char* response, string delimiter)
{
	/*
	Method to extract value from the response of the Device
	response -> Response of the Device 
	delimiter -> Delimiter of the extracted value 

	return -> extracted value 
	
	*/

	string response1(response); // convert the array of character into a string 
	 
	vector<string> parseString1 = Stringdelimiter(response1, delimiter); // split the response with the delimiter to extract the Wavelength value 

	if (this->specPower == 0)
	{
		vector<string> parseString2 = Stringdelimiter(parseString1.at(1), "\r"); // split the bext response to extract the value of the Power specification 
		this->specPower = stoi(parseString2.at(0)); // convert and pass the string value to the variable

	}

	string str3 = parseString1.at(0).erase(0, 4);

	float value = stof(str3); // convert the string value into float value 


	return value;
}

void LSFM_Laser::Power_On()
{

	stringstream sstream; // variable to write hexadecimal value 

	sstream << "?POn" << "\r"; // Power on Command 

	string command = sstream.str();
	char SerialBuffer[400];  //Buffer to send and receive data

	strcpy_s(SerialBuffer, command.c_str()); // copy command to the serial Buffer 

	this->SendCommand(SerialBuffer); // Send command to the Device 
}

LSFM_Laser::LSFM_Laser(string serialPortInput)
{
	/*
	* Constructor of the Class 
	* parameter: serialPortInput -> COM port of the Laser 
	*/
	serialPort = new (char[100]);
	strcpy(serialPort, serialPortInput.c_str());
}

void LSFM_Laser::Connect()
{

	/*
	Method of connecting a serial port connected laser with a computer:
	 Important parameter of the connection: CoMPort,BaudRate, ByteSize, StopBits, Parity
	
	*/

	// Open serial port and retrieve handler to communicate with the device 
	this->serialHandle = CreateFile(serialPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


	if (this->serialHandle == INVALID_HANDLE_VALUE)
	{
		printf("\n Connection with the Controller failed\n\n");

		printf("\n Port can't be opened\n\n");
	}

	else
	{
		// Do some basic settings
		DCB serialParams = { 0 };
		serialParams.DCBlength = sizeof(serialParams);

		// Check if the Communication with the Device was etablished 
		this->Status = GetCommState(this->serialHandle, &serialParams);

		if (this->Status == FALSE)
		{
			printf("\n Connection with the Controller failed\n\n");

			printf("\nError to Get the Com state\n\n");
		}

		else
		{
            // Set Baudrate, ByteSize, StopBits and Parity 
			serialParams.BaudRate = 500000;
			serialParams.ByteSize = 8;
			serialParams.StopBits = ONESTOPBIT;
			serialParams.Parity = NOPARITY;

			this->Status = SetCommState(this->serialHandle, &serialParams);

			if (Status == FALSE)
			{
				printf("\n Connection with the Controller failed\n\n");

				printf("\nError to Setting DCB Structure\n\n");
			}

			else
			{
				// Set timeouts
				COMMTIMEOUTS timeout = { 0 };
				timeout.ReadIntervalTimeout = 50;
				timeout.ReadTotalTimeoutConstant = 50;
				timeout.ReadTotalTimeoutMultiplier = 50;
				timeout.WriteTotalTimeoutConstant = 50;
				timeout.WriteTotalTimeoutMultiplier = 10;

				SetCommTimeouts(this->serialHandle, &timeout);


				this->laserConnected = true;

				// Read status to check if
				this->ReadStatus();

				// Get the actual operating mode of the Device 
				this->GetOperatingMode();

				// Set default operating Mode 
				this->SetOperatingMode(0x0000);

				// Set default Modulation 
				this->SetModulation(1);

				//Set Analog Impedanz
				this->SetAnalogImpedanz(1);

				//laser Power ON
				this->Power_On();

				// Get Wavelength and Spec. Power 
				this->ReadWaveLength();

				printf("\n Connection with the Laser  succesfull\n\n");
			}

		}



	}



}

void LSFM_Laser::SetAnalogImpedanz(int impedanz)
{
	/*   method to Set the Impedance of the Analog Input.

	valid value for the paramter modulation are:
		 0: impedance OFF
		 1:impedance ON
	*/

	if (this->laserConnected == true)
	{
		if (impedanz == 0 || impedanz == 1)
		{
			stringstream sstream;

			sstream << "?SIA" << impedanz << "\r"; // Set Impedance of the Analog Input

			string command = sstream.str();

			char SerialBuffer[400];  //Buffer to send and receive data

			strcpy_s(SerialBuffer, command.c_str());

			this->SendCommand(SerialBuffer);

		}

		else
			cout << "the input value is not acceptable" << endl;
	}

	else
		cout << "No laser connected" << endl;
}

void LSFM_Laser::Disconnect()
{
	if (this->laserConnected == true)
	{
		CloseHandle(this->serialHandle);
		this->laserConnected = false;
		this->specPower = 0;
		printf("\nConnection successfull closed\n\n");
	}
	else
		cout << "No laser connected" << endl;



}



void LSFM_Laser::add(Component* component)
{
	// Ausnahme werfen
	cout << "Ein Blatt kann keine Kinder haben" << endl;
}

void LSFM_Laser::remove(Component* component)
{
	// Ausnahme werfen
	cout << "Ein Blatt kann keine Kinder entfernen" << endl;

}

void LSFM_Laser::LaserOn()
{
	/*  Method to turn On the Laser*/

	if (this->laserConnected == true)
	{
		cout << "abena" << endl;
		stringstream sstream;

		sstream << "?LOn" << "\r";   // Command to turn ON the laser 

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);
	}

	else
		cout << "No laser connected" << endl;

}

void LSFM_Laser::LaserOff()
{
	/*  Method to turn OFF the Laser*/

	if (this->laserConnected == true)
	{
		stringstream sstream;

		sstream << "?LOf" << "\r"; // command to turn OFF the Laser 

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);
	}

	else
		cout << "No laser connected" << endl;



}

int LSFM_Laser::GetActualModulation()
{
	// Method to retrieve the actual Modualtion of the Laser
	return this->actualModulation;
}


void LSFM_Laser::SetModulation(int modulation)
{
	/*   method to change the signal mdulation.

	valid value for the paramter modulation are:
		 0: emission standby (the diode current source is set to 0)
		 1: ACC mode – no modulation active
		 2: APC mode – no modulation active
		 3: ACC mode – analog modulated
		 4: Digital and Analog modulation active
	*/

	if (this->laserConnected == true)
	{
		if (modulation == 0 || modulation == 1 || modulation == 2 || modulation == 3 || modulation == 4)
		{
			stringstream sstream;

			sstream << "?ROM" << modulation << "\r";  // command to ret

			string command = sstream.str();

			char SerialBuffer[400];  //Buffer to send and receive data

			strcpy_s(SerialBuffer, command.c_str());

			this->SendCommand(SerialBuffer);

			this->actualModulation = modulation;
		}

		else
			cout << "the input value is not acceptable" << endl;
	}

	else
		cout << "No laser connected" << endl;


}


int  LSFM_Laser::GetModulation()
{

	return this->modulation;

}

int LSFM_Laser::ReadWaveLength()
{
	cout << this->laserConnected << endl;
	if (this->laserConnected == true)
	{
		stringstream sstream;

		sstream << "?GSI" << "\r";

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);

		int waveLength = extractValueFromResponse(SerialBuffer, "§");



		int value = 0;

		switch (waveLength)
		{
		case 638:
			value = 2;
			break;
		case 488:
			value = 1;
			break;
		case 445:
			value = 3;
			break;
		default:
			break;
		}
		return value;
	}

	else
		cout << "No LSFM_Laser connected" << endl;

}


int LSFM_Laser::ReadSpecPower()
{
	return this->specPower;
}

void LSFM_Laser::ReadStatus()
{
	if (this->laserConnected == true)
	{
		char SerialBuffer[400] = { 0x3F,0X47,0X41,0X53,0X0D }; //Buffer to send and receive data

		this->SendCommand(SerialBuffer);

		string binaryValue = hex_str_to_bin_str(SerialBuffer);

		if (binaryValue.at(15) == '1')
		{
			this->ErrorHandling();
			this->Disconnect();

		}

		if (binaryValue.at(14) == '1')
			this->laserStatus = true;

	}

	else
		cout << "No laser connected" << endl;

}


float LSFM_Laser::ReadMaxPower()
{
	if (this->laserConnected == true)
	{
		stringstream sstream;

		sstream << "?GMP" << "\r";

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);

		float maxPower = extractValueFromResponse(SerialBuffer, "\r");

		return maxPower;
	}

	else
		cout << "No LSFM_Laser connected" << endl;


}

void LSFM_Laser::SetPower(float power)
{

	if (this->laserConnected == true)
	{
		stringstream sstream;

		sstream << "?SPP" << power << "\r";

		string command = sstream.str();

		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);
	}

	else
		cout << "No laser connected" << endl;


}

float LSFM_Laser::GetPowerInpercentage()
{
	if (this->laserConnected == true)
	{

		stringstream sstream;

		sstream << "?GPP" << "\r";

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());
		

		this->SendCommand(SerialBuffer);

		float intensityValue = extractValueFromResponse(SerialBuffer, "\r");

		return intensityValue;
	}

	else
		cout << "No laser connected" << endl;


}

void LSFM_Laser::GetOperatingMode()
{
	if (this->laserConnected == true)
	{
		char SerialBuffer[400] = { 0x3F,0X47,0X4f,0X4d,0X0D }; //Buffer to send and receive data

		this->SendCommand(SerialBuffer);

		string binaryValue = hex_str_to_bin_str(SerialBuffer);


		if (binaryValue.at(7) == '1')
			this->modulation = 1;

	}
	else
		cout << "No laser connected" << endl;
}



void LSFM_Laser::SetOperatingMode(int OperatingMode)
{
	if (this->laserConnected == true)
	{

		stringstream sstream;

		sstream << "?SOM" << std::uppercase << std::hex << OperatingMode << "\r" << std::ends;

		string command = sstream.str();

		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer);

	}

	else
		cout << "No laser connected" << endl;

}


vector<string> LSFM_Laser::Stringdelimiter(string s1, string delimiter) {

	/* method to split a string with help from a delimiter*/

	size_t pos = 0;
	vector<string> list1;
	while ((pos = s1.find(delimiter)) != string::npos) {
		list1.push_back(s1.substr(0, pos));
		s1.erase(0, pos + delimiter.length());
	}

	list1.push_back(s1);
	return list1;

}


bool LSFM_Laser::IsHardwareConnected()
{
	return this->laserConnected;
}