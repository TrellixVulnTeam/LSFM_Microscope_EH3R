#include "pch.h"
#include "XagylFilter.h"


void LSFM_Filter::SendCommand(char* SerialBuffer, int response)
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

	if (response)
	{
		do
		{
			Status = ReadFile(this->serialHandle, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
			SerialBuffer[loop] = ReadData;

			if (ReadData == '\r')
				NoBytesRead = 0;

			++loop;
		} while (NoBytesRead > 0);

	}


}

void LSFM_Filter::Connect()
{
	// Open serial port

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

		this->Status = GetCommState(this->serialHandle, &serialParams);

		if (this->Status == FALSE)
		{
			printf("\n Connection with the Controller failed\n\n");

			printf("\nError to Get the Com state\n\n");
		}

		else
		{
			serialParams.BaudRate = 9600;
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
				this->filterConnected = true;
				int position = GetFilterCurrentPosition();

				this->currentPosition = position;

				printf("\n Connection with the Filter succesfull\n\n");

			}



		}



	}



}

void LSFM_Filter::Disconnect()
{
	if (this->filterConnected == true)
	{
		CloseHandle(this->serialHandle);
		printf("\nConnection successfull closed\n\n");
	}
	else
		cout << "No Filter connected" << endl;



}

LSFM_Filter::LSFM_Filter()
{
	vector<string> list1;
	vector<string> list2;
	const char* filePath = "C:\\LSFM_Software\\TreiberImplementierung\\LSFMLibrary\\XagYl_Filter_Config.txt";
	LoadConfigFile(filePath, &list1, &list2);

	serialPort = new char[100];
	string s = GetElementFromConfigFIle("COM", list1, list2);
	string serialPortInput = "\\\\.\\COM" + s;
	strcpy(serialPort, serialPortInput.c_str());


	this->position1 = GetElementFromConfigFIle("position1", list1, list2);
	this->position2 = GetElementFromConfigFIle("position2", list1, list2);
	this->position3 = GetElementFromConfigFIle("position3", list1, list2);
	this->position4 = GetElementFromConfigFIle("position4", list1, list2);
	this->position5 = GetElementFromConfigFIle("position5", list1, list2);

}

string LSFM_Filter::GetElementFromConfigFIle(string element, vector<string>list1, vector<string>list2)
{
	std::vector<string>::iterator it = std::find(list1.begin(), list1.end(), element);
	if (it != list1.end())
	{

		int index = std::distance(list1.begin(), it);

		return list2.at(index);

	}
}


void LSFM_Filter::add(Component* component)
{
	// Ausnahme werfen
	cout << "Ein Blatt kann keine Kinder haben" << endl;
}

void LSFM_Filter::remove(Component* component)
{
	// Ausnahme werfen
	cout << "Ein Blatt kann keine Kinder entfernen" << endl;

}

void LSFM_Filter::LoadConfigFile(const char* filePath, vector<string>* list1, vector<string>* list2)
{
	string line;
	ifstream input;
	string delimiter = "->";
	size_t pos = 0;
	input.open(filePath);
	if (input.is_open())
	{
		while (getline(input, line))
		{
			size_t pos = 0;
			string s3 = line;
			while ((pos = s3.find(delimiter)) != string::npos) {
				list1->push_back(s3.substr(0, pos));
				list2->push_back(s3.substr(pos + 2, s3.back()));
				break;
			}

		}

	}


	else
	{
		throw std::runtime_error("The file to compare the Controller does not exist");
	}
}

bool LSFM_Filter::MoveToSelectedPosition(int position)
{

	if (this->filterConnected == true)
	{

		
			if (position < 1 && position > 5)
			{
				cout << "Argument Out of Range" << endl;

				return false;
			}

			else
			{
				stringstream sstream;

				sstream << "G" << position;
				this->currentPosition = position;




				string command = sstream.str();
				char SerialBuffer[400];  //Buffer to send and receive data

				strcpy_s(SerialBuffer, command.c_str());

				this->SendCommand(SerialBuffer, 0);

				Sleep(2000);

				int position = GetFilterCurrentPosition();

				if (position)
					this->currentPosition = position;

				return true;
			}

	}

	else
	{
		cout << "No FIlter connected" << endl;
		return false;

	}

}

int LSFM_Filter::GetFilterCurrentPosition()
{
	if (this->filterConnected == true)
	{
		stringstream sstream;

		sstream << "I" << 2;

		string command = sstream.str();
		char SerialBuffer[400];  //Buffer to send and receive data

		strcpy_s(SerialBuffer, command.c_str());

		this->SendCommand(SerialBuffer, 1);

		string response(SerialBuffer);

		response = response.substr(0, response.find("\r"));

		if (response.size() > 2)
		{
			cout << " Error with the Acquisition of the Value" << endl;

			return 0;
		}
		else
		{
			string s = response.substr(1);

			int position = stoi(s);

			return position;
		}

	}

	else
	{
		cout << "No FIlter connected" << endl;
		return false;

	}
}

int LSFM_Filter::ActualFIlterPosition()
{
	return this->currentPosition;
}

bool LSFM_Filter::GetAllPositionName(char* positionsList)
{
	string positionslist = this->position1 + "//" + this->position2 + "//" + this->position3 + "//" + this->position4 + "//" + this->position5;
	strcpy(positionsList, positionslist.c_str());

	return true;
}

bool LSFM_Filter::IsHardwareConnected()
{
	return this->filterConnected;
}