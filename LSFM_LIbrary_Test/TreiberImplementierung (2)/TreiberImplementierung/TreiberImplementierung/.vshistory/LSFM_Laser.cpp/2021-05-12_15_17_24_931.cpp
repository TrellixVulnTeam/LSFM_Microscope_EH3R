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
		// TODO handle default / error
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
			// TODO use a loop from <algorithm> or smth
			string bin;

			for (unsigned i = 0; i != 4; ++i)
				bin += hex_char_to_bin(hexValue[i]);

			return bin;
		}
		
	}

	void LSFM_Laser::ErrorHandling()
	{
		if (this->laserConnected == true)
		{
			stringstream sstream;

			sstream << "?GFB" << "\r";

			string command = sstream.str();
			char SerialBuffer[400];  //Buffer to send and receive data

			strcpy_s(SerialBuffer, command.c_str());

			 this->SendCommand(SerialBuffer);

			sstream << "?GLF" << "\r";

			command = sstream.str();

			strcpy_s(SerialBuffer, command.c_str());

			this->SendCommand(SerialBuffer);

			string binaryValue = hex_str_to_bin_str(SerialBuffer);

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
		string response1(response);
		vector<string> parseString1 = Stringdelimiter(response1, delimiter);

		string str3 = parseString1.at(0).erase(0, 4);

		float value = stof(str3);

		return value;
	}

	void LSFM_Laser::Power_On()
	{

		stringstream sstream;

		sstream << "?POn" << "\r";

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		 this->SendCommand(SerialBuffer);
	}

	




	LSFM_Laser::LSFM_Laser(string serialPortInput)
	{
		serialPort = new (char[100]);
		strcpy(serialPort, serialPortInput.c_str());
	}


	void LSFM_Laser::Connect()
	{
		// Open serial port

		this->serialHandle = CreateFile(serialPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


		cout << serialPort << endl;
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

			this->Status = GetCommState(this->serialHandle, &serialParams);

			if (this->Status == FALSE)
			{
				printf("\n Connection with the Controller failed\n\n");

				printf("\nError to Get the Com state\n\n");
			}

			else
			{
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
					this->ReadStatus();
					this->GetOperatingMode();
				//	this->SetOperatingMode(0x0000);
				//	this->SetModulation(1);
					this->Power_On();
					this->LaserOn();
					cout << this->ReadWaveLength() << endl;
					cout << this->GetPowerInpercentage() << endl;
					this->LaserOff();
					
					printf("\n Connection with the Controller succesfull\n\n");
				}



			}



		}



	}

	void LSFM_Laser::Disconnect()
	{
		if (this->laserConnected == true)
		{
			CloseHandle(this->serialHandle);
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
		if (this->laserConnected == true)
		{
			stringstream sstream;

			sstream << "?LOn" << "\r";

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
		if (this->laserConnected == true)
		{
			stringstream sstream;

			sstream << "?LOf" << "\r";

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
		*/

		if (this->laserConnected == true)
		{
			if (modulation == 0 || modulation == 1 || modulation == 2 || modulation == 3)
			{
				stringstream sstream;

				sstream << "?ROM" << modulation << "\r";

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
				value = 1;
				break;
			case 488:
				value = 2;
				break;
			default:
				break;

			}

			return value;
		}

		else
			cout << "No LSFM_Laser connected" << endl;

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

		return list1;

	}