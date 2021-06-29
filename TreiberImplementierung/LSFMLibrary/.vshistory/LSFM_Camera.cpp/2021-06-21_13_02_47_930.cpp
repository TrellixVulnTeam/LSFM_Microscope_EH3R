#include "pch.h"
#include "LSFM_Camera.h"
#include "include/file12.h"
#include "include/file12.cpp"
#include <math.h>






PCO_Description LSFM_Camera::GetCameraDescription()
{
	strDescription.wSize = sizeof(PCO_Description);
	this->iRet = PCO_GetCameraDescription(this->cam, &this->strDescription);//Sensor and camera specific description;

	return this->strDescription;
}

void LSFM_Camera::GetCameraHealthStatus()
{
	this->iRet = PCO_GetCameraHealthStatus(this->cam, &this->cameraWarning, &this->cameraError, &this->cameraStatus);
}

void LSFM_Camera::CloseConnectionWithComment(DWORD dwerr, const char* comment)
{

	DWORD dwlen = 400;
	char pbuf[400];
	PCO_GetErrorText(dwerr, pbuf, dwlen);
	printf("%s\n", comment);
	printf("%s\n", pbuf);
	Disconnect();
}

bool LSFM_Camera::IsNoErrorOccured(int iRet, const char* message)
{
	if (!iRet)
		return true;
	else
	{
		CloseConnectionWithComment(iRet, message);
		return false;
	}
}

bool LSFM_Camera::GetRecodingstate()
{
	int iRet = PCO_GetRecordingState(this->cam, &this->RecordingState);

	if (IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		return true;
	else
		return false;

}

bool LSFM_Camera::SetRecordingstate(WORD recordingstate)
{
	if (this->GetRecodingstate())
	{
		if (this->RecordingState == recordingstate)
		{
			switch (recordingstate)
			{
			case 0:
				printf("The Recording state is already in Stop modus\n");
				break;
			case 1:
				printf("The Recording state is already in Start modus\n");
				break;
			}

			return true;
		}
		else
		{
			int iRet = PCO_SetRecordingState(this->cam, recordingstate);
			if (IsNoErrorOccured(iRet, "Problem with the Recording State"))
				return true;
			else
				return false;
		}
	}

}

void LSFM_Camera::RecordedStateOn() { this->SetRecordingstate(1); }

void LSFM_Camera::RecordedStateOff() { this->SetRecordingstate(0); }

bool LSFM_Camera::ResetToDefaultParameters()
{
	int iRet = PCO_ResetSettingsToDefault(this->cam);
	if (IsNoErrorOccured(iRet, "Problem with the Reset to Default parameter\n"))
		return true;
	else
		return false;
}

bool LSFM_Camera::CreateRecorder(DWORD mode, char localDisk, DWORD* maxImgCountArr)
{


	this->hCamArr[0] = this->cam;

	DWORD imgDistributionArr[1] = { 1 }; //Array defining the memory distribution between the used cameras(can be set to NULL for equal distribution)
										 // //Set image distribution to 1 since only one camera is used



	int iRet = 0;

	switch (mode)
	{
	case PCO_RECORDER_MODE_MEMORY:
		iRet = PCO_RecorderCreate(&this->hRec, hCamArr, imgDistributionArr, 1, mode, NULL, maxImgCountArr);
		break;
	case PCO_RECORDER_MODE_FILE:
		iRet = PCO_RecorderCreate(&this->hRec, hCamArr, imgDistributionArr, 1, mode, localDisk, maxImgCountArr);
		break;
	default:
		break;
	}

	if (IsNoErrorOccured(iRet, "Unable to create a recording session"))
		return true;
	else
		return false;


}


bool LSFM_Camera::GetAndSave_Image(char* file_name)
{
	int cameratyp = this->GetCameraIdentification();


	if (cameratyp == 1)
	{

		bool acquisitionRunning = true;

		//Get image size
		WORD imgWidth = 0, imgHeight = 0;
		iRet = PCO_RecorderGetSettings(this->hRec, hCamArr[0], NULL, NULL, NULL, &imgWidth, &imgHeight, NULL);
		if (IsNoErrorOccured(iRet, "Error with the acquisition of Parameters"))
		{
			//Get the images currently available
			DWORD procImgCount = 0;
			PCO_METADATA_STRUCT metadata;
			metadata.wSize = sizeof(PCO_METADATA_STRUCT);



			//Start camera
			iRet = PCO_RecorderStartRecord(this->hRec, NULL);

			while (acquisitionRunning)
			{
				// get the current available images 
				iRet = PCO_RecorderGetStatus(this->hRec, this->cam, &acquisitionRunning, NULL, NULL, &procImgCount, NULL, NULL, NULL, NULL, NULL);

				DWORD warn = 0, err = 0, status = 0;

				iRet = PCO_GetCameraHealthStatus(this->cam, &warn, &err, &status);
				if (err != PCO_NOERROR)
					PCO_RecorderStopRecord(this->hRec, this->cam); //Stop record on health error

				Sleep(10);

				// check if the camera has took some pictures
				if (procImgCount >= 1)
				{
					acquisitionRunning = false;
					break;
				}


			}

			//Allocate memory for one image
			WORD* imgBuffer = NULL;
			imgBuffer = new WORD[(__int64)imgWidth * (__int64)imgHeight];



			iRet = PCO_RecorderCopyImage(this->hRec, hCamArr[0], PCO_RECORDER_LATEST_IMAGE, 1, 1, imgWidth, imgHeight, imgBuffer, NULL, &metadata, NULL);

			if (IsNoErrorOccured(iRet, "Error with the Acquisition of the Image"))

			{

				iRet = PCO_RecorderSaveImage(imgBuffer, imgWidth, imgHeight, FILESAVE_IMAGE_BW_16, false, file_name, true, nullptr);

				if (IsNoErrorOccured(iRet, " Error during image storage  "))
				{
					printf("Image succesfull saved \n");
					delete[] imgBuffer;
				}

			}



		}
		return true;
	}

	else if (cameratyp == 2)

	{
		// Initialise and define the number of images of 1 
		int L = 1;
		WORD phasenumber;
		WORD phaseSymmetry;
		WORD phaseOrder;
		WORD tapSelect;


		iRet = PCO_GetFlimPhaseSequenceParameter(this->cam, &phasenumber, &phaseSymmetry, &phaseOrder, &tapSelect, NULL, NULL);
		PhaseNumber phase = static_cast<PhaseNumber>(phasenumber);

		if (IsNoErrorOccured(iRet, "Error with the Acquisition of the flim Parameters"))
		{
			/*
			// Calculate the total of image for one sequence
			if (phaseSymmetry == PhaseSymmetry::twice)
				L = 2 * pow(2, int(phase));
			else
				L = int(phase);

			int bothTapselect = TapSelect::both;

			if (tapSelect == bothTapselect)
				L = L * 2;

			*/

			L = 2*pow(2, int(phase));

			bool acquisitionRunning = true;
			DWORD procImgCount = 0;
			PCO_METADATA_STRUCT metadata;
			metadata.wSize = sizeof(PCO_METADATA_STRUCT);
			cout << "Acquisition of Pictures in Process" << endl;

			while (acquisitionRunning)
			{
				// get the current available images 
				iRet = PCO_RecorderGetStatus(this->hRec, this->cam, &acquisitionRunning, NULL, NULL, &procImgCount, NULL, NULL, NULL, NULL, NULL);


				if (IsNoErrorOccured(iRet, "Error with the Recorder Status"))
				{
					DWORD warn = 0, err = 0, status = 0;

					iRet = PCO_GetCameraHealthStatus(this->cam, &warn, &err, &status);
					if (err != PCO_NOERROR)
						PCO_RecorderStopRecord(this->hRec, this->cam); //Stop record on health error

					Sleep(10);

					// check if the camera has took some pictures
					if (procImgCount >= L)
					{
						acquisitionRunning = false;
						break;
					}
				}


			}

			//Save L images
			for (int i = 0; i < L; i++)
			{

				string s(file_name);

				

				string s1 = "_0" + to_string(i) + ".tif";

				s = s + s1;

				char* fileName = new char[s.length() + 1];
				strcpy(fileName, s.c_str());
			   

				//Allocate memory for one image
				WORD* imgBuffer = new WORD[(__int64)imgWidth * (__int64)imgHeight];
				Sleep(10);


				iRet = PCO_RecorderCopyImage(this->hRec, hCamArr[0], i, 1, 1, imgWidth, imgHeight, imgBuffer, NULL, &metadata, NULL);


				if (IsNoErrorOccured(iRet, "Error with the Acquisition of the Image"))

				{
					iRet = PCO_RecorderSaveImage(imgBuffer, imgWidth, imgHeight, FILESAVE_IMAGE_BW_16, false, fileName, true, nullptr);

					if (IsNoErrorOccured(iRet, " Error during image storage  "))
					{
						printf("Image succesfull saved \n");
						delete[] imgBuffer;
					}

				}

				delete[] fileName;
			}

		}




		return true;
	}

	else
	{
		printf("The Selected Camera-Typ doesn't exist \n");
		return false;
	}

}


void  LSFM_Camera::Prepare_Recording(DWORD mode)
{
	int iRet;

	//Some frequently used parameters for the camera
	//DWORD numberOfImages = 10;
	DWORD expTime = 100;
	WORD expBase = TIMEBASE_MS;
	WORD metaSize = 0, metaVersion = 0;


	//set camera to default state, recording off
	this->RecordedStateOff();


	//set noise filter off
	//iRet = PCO_SetNoiseFilterMode(camera, 0);
	//iRet = PCO_ResetSettingsToDefault(hCamArr[0]);
	//Do some settings
	iRet = PCO_SetTimestampMode(this->cam, TIMESTAMP_MODE_OFF);
	iRet = PCO_SetMetaDataMode(this->cam, METADATA_MODE_ON, &metaSize, &metaVersion);
	//Set Exposure time
	iRet = PCO_SetDelayExposureTime(this->cam, 0, expTime, 2, expBase);

	iRet = PCO_ArmCamera(this->cam);


	//Reset Recorder to make sure a no previous instance is running
	//iRet = PCO_RecorderResetLib(false);       Here we have 2 Cameras and then no need the reset 

	//Create Recorder (mode: memory sequence)
	//WORD mode = PCO_RECORDER_MODE_MEMORY | PCO_RECORDER_MODE_USE_DPCORE;

	if (CreateRecorder(mode, NULL, maxImgCountArr))
	{
		//Set required images
		if (reqImgCountArr[0] > maxImgCountArr[0])
			reqImgCountArr[0] = maxImgCountArr[0];
	}
}

void LSFM_Camera::CheckAndSetTriggerMode(WORD requiredMode)
{
	WORD  triggermode;
	iRet = PCO_GetTriggerMode(this->cam, &triggermode);

	if (IsNoErrorOccured(iRet, "Error during Trigger mode  acquisition "))
	{
		if (triggermode != requiredMode)
		{
			iRet = PCO_SetTriggerMode(this->cam, requiredMode);
			if (IsNoErrorOccured(iRet, "Error during Trigger mode Selection"))
			{
			}
		}

	}

}

void LSFM_Camera::Connect()
{
	if (this->isNoCameraConnected)
	{
		printf("Get Handle to connected camera\n");

		this->iRet = PCO_OpenCamera(&this->cam, 0);
		if (this->iRet != PCO_NOERROR)
		{
			this->CloseConnectionWithComment(this->iRet, "No Camera found\n");
		}
		else
		{
			printf("Connection with the Camera successful\n");

			if (this->GetCameraIdentification() == 1)

			{
				DWORD m_dwSetup[4];
				WORD m_wLen = sizeof(m_dwSetup) / sizeof(DWORD);
				WORD m_wType = 0; // Set to zero initially
				iRet = PCO_GetCameraSetup(this->cam, &m_wType, &m_dwSetup[0], &m_wLen);
				if (IsNoErrorOccured(iRet, "Error with the retribution of the Camera Setup"))
				{
					if (m_dwSetup[0] == PCO_EDGE_SETUP_GLOBAL_RESET)
					{
						m_dwSetup[0] == PCO_EDGE_SETUP_ROLLING_SHUTTER;

						PCO_SetCameraSetup(this->cam, m_wType, &m_dwSetup[0], m_wLen);
						PCO_RebootCamera(this->cam);

						this->Disconnect();
					}
					else
					{

						CheckAndSetTriggerMode(0);

						this->cameraMode = PCO_RECORDER_MEMORY_RINGBUF;

						Prepare_Recording(PCO_RECORDER_MODE_MEMORY);

						//Init Recorder
						iRet = PCO_RecorderInit(hRec, reqImgCountArr, 1, this->cameraMode, 0, NULL, NULL);

						if (IsNoErrorOccured(iRet, "Error with the Initialisation of the Recorder"))
						{

							//Get image size
							iRet = PCO_RecorderGetSettings(hRec, hCamArr[0], NULL, NULL, NULL, &this->imgWidth, &this->imgHeight, NULL);


							//Start camera
							iRet = PCO_RecorderStartRecord(hRec, NULL);

							CheckAndSetTriggerMode(0); // Trigger Mode to "Auto Sequence"

							this->isNoCameraConnected = false;

						}

					}
				}
			}

			else if (this->GetCameraIdentification() == 2)
			{

				// Die Kamera auf Softtrigger einstellen 
				LSFM_Camera::CheckAndSetTriggerMode(1);

				//  First step to the Initialisation of the :  Recorder Typ: FIFO
				this->cameraMode = PCO_RECORDER_MEMORY_FIFO;

				//Set the mode to  Memory,
				LSFM_Camera::Prepare_Recording(PCO_RECORDER_MODE_MEMORY);

				// Define the number of images for the sequences: 10 sequences * 32(maximal nimber of images for one sequence)

				reqImgCountArr[0] = 320;

				//Init Recorder
				iRet = PCO_RecorderInit(this->hRec, reqImgCountArr, 1, this->cameraMode, 0, NULL, NULL);

				if (IsNoErrorOccured(iRet, "Error with the Initialisation of the Recorder"))
				{




					if (IsNoErrorOccured(iRet, "Error with the Start of  the Recorder"))
					{
						//Get image size
						iRet = PCO_RecorderGetSettings(this->hRec, hCamArr[0], NULL, NULL, NULL, &this->imgWidth, &this->imgHeight, NULL);


						// Set the flim Phase sequence parameter 
						bool result = SetFlimPhaseSequenceParameter(PhaseNumber::Four, PhaseSymmetry::twice, PhaseOrder::opposite, TapSelect::both, 30000000);

						// Set the modulation waveForm: Sinusoidal 
						result = Set_Camera_Modulation_Frequency(1);

						//Start camera
						iRet = PCO_RecorderStartRecord(this->hRec, NULL);

						this->isNoCameraConnected = false;

						}

					}

				}
			}
		}
	}



void LSFM_Camera::Disconnect()
{
	printf("Stop camera and close connection\n");

	//Stop Recorder
	iRet = PCO_RecorderStopRecord(hRec, this->cam);

	//Delete Recorder
	iRet = PCO_RecorderDelete(hRec);

	iRet = PCO_CloseCamera(cam);

	printf("Connection Successful closed \n");
}

void LSFM_Camera::GetExposureAndDelayTime(WORD* expousreTime, WORD* exposuretimeBase)
{
	if (this->cam != NULL)
	{
		WORD wTimeBaseDelay;
		WORD wTimeBaseExposure;

		PCO_GetDelayExposureTime(this->cam, &this->dwDelay, &this->dwExposure, &wTimeBaseDelay, &wTimeBaseExposure);

		switch (wTimeBaseExposure)
		{
		case 0:
			printf(" Exposure Time in nanoSecond\n");
			printf("Exposure = %ld \n", this->dwExposure);
			break;
		case 1:
			printf(" Exposure Time in microSecond\n");
			printf("Exposure = %ld \n", this->dwExposure);
			break;
		case 2:
			printf(" Exposure Time in milisecond\n");
			printf("Exposure = %ld \n", this->dwExposure);
			break;
		}

		*expousreTime = this->dwExposure;
		*exposuretimeBase = wTimeBaseExposure;
	}

	else
		cout << "Invalid Handler for the Camera" << endl;


}

void LSFM_Camera::SetExposureTime(DWORD value, DWORD unit)
{

	if (this->cam != NULL)
	{
		this->dwExposure = value;
		this->dwDelay = 100;
		WORD wTimeBaseDelay = 2;
		WORD wTimeBaseExposure = 2;
		int idx = PCO_SetDelayExposureTime(this->cam, this->dwDelay, this->dwExposure, wTimeBaseDelay, wTimeBaseExposure);
		if (idx != PCO_NOERROR)
		{
			CloseConnectionWithComment(idx, "Error with the Set of the Delay Value\n");
		}
		else
		{
			printf("Delay Successful Set\n");
			switch (wTimeBaseExposure)
			{
			case 0:
				printf(" Exposure Time in nanoSecond\n");
				printf("Exposure = %ld \n", this->dwExposure);
				break;
			case 1:
				printf(" Exposure Time in microSecond\n");
				printf("Exposure = %ld \n", this->dwExposure);
				break;
			case 2:
				printf(" Exposure Time in milisecond\n");
				printf("Exposure = %ld \n", this->dwExposure);
				break;
			}
		}
	}

	else
		cout << "Invalid Handler for the Camera" << endl;

}

void LSFM_Camera::add(Component* component)
{
	// Ausnahme werfen
	cout << "Ein Blatt kann keine Kinder haben" << endl;
}

void LSFM_Camera::remove(Component* component) {
	// Ausnahme werfen
	cout << "Ein Blatt kann keine Kinder entfernen" << endl;

}

bool LSFM_Camera::SingleImageAquisition(char* file_name)
{

	if (this->hRec == NULL || this->cam == NULL)
	{
		cout << "Invalid Handler for the Camera" << endl;
		return 0;
	}

	else
	{
		WORD wCameraBusyState;
		iRet = PCO_GetCameraBusyStatus(this->cam, &wCameraBusyState);
		if (IsNoErrorOccured(iRet, "Eror during the Camera Busy Status acquisiton"))
		{
			if (wCameraBusyState == 1)
				printf(" Camera is Busy, not ready for a new trigger command");
			else
			{
				PCO_SetTriggerMode(this->cam, 1); // Trigger mode to "Software-Trigger"
				WORD forceTrigger = 1;
				iRet = PCO_ForceTrigger(this->cam, &forceTrigger);

				if (IsNoErrorOccured(iRet, "Error during the Trigger Command"))
				{
					

					bool result = GetAndSave_Image(file_name);

					return result;
				}

			}
		}
	}
	
}

void LSFM_Camera::Start_Sequence()

{

	if (this->cam == NULL)
	{
		cout << " HAndle to the Camera is invalid" << endl;
	}

	else
		PCO_SetTriggerMode(this->cam, 1); // Trigger mode to "Software-Trigger"

}

DataPack* LSFM_Camera::Live_View()
{
	

	int cameraTyp = this->GetCameraIdentification();

	if (this->hRec == NULL || this->cam == NULL)
	{
		cout << "camera Handler or Recorder Handle  is Invalid" << endl;
		return NULL;
	}

	else
	{
		cout << "Test" << endl;

		hCamArr[0] = this->cam;
		WORD wCameraBusyState;
		//vector<WORD*>bufferList;
		WORD imgWidth = 0, imgHeight = 0;
		DataPack* datapack = new DataPack();

		iRet = PCO_RecorderGetSettings(this->hRec, hCamArr[0], NULL, NULL, NULL, &imgWidth, &imgHeight, NULL);
		if (IsNoErrorOccured(iRet, "Eror during the Acquisition of Images Parameters"))
		{
			iRet = PCO_GetCameraBusyStatus(this->cam, &wCameraBusyState);
			if (IsNoErrorOccured(iRet, "Eror during the Camera Busy Status acquisiton"))
			{

				if (wCameraBusyState == 1)
					printf(" Camera is Busy, not ready for a new trigger command");
				else
				{
					if (cameraTyp == 1)//PCO Edge
					{
						CheckAndSetTriggerMode(0); // Trigger mode to "Auto-sequence"

						//Wait until acquisition is finished (all other parameters are ignored)
						bool acquisitionRunning = true;

						//Get the images currently available
						DWORD procImgCount = 0;
						PCO_METADATA_STRUCT metadata;
						metadata.wSize = sizeof(PCO_METADATA_STRUCT);
						bool imageSaved = false;
						DWORD imgNumber = 0;
						bool isRunning = true;

						while (acquisitionRunning)
						{

							iRet = PCO_RecorderGetStatus(this->hRec, hCamArr[0], &acquisitionRunning, NULL, NULL, &procImgCount, NULL, NULL, NULL, NULL, NULL);
							if (IsNoErrorOccured(iRet, "Error during the Live View"))
							{
								DWORD warn = 0, err = 0, status = 0;

								iRet = PCO_GetCameraHealthStatus(this->cam, &warn, &err, &status);
								if (err != PCO_NOERROR)
									PCO_RecorderStopRecord(this->hRec, this->cam); //Stop record on health error

								Sleep(10);

								if (procImgCount >= 1)
								{
									acquisitionRunning = false;
									break;
								}
							}
						}

								//Allocate memory for one image
								WORD* imgBuffer = NULL;
								imgBuffer = new WORD[(__int64)this->imgWidth * (__int64)this->imgHeight];


								iRet = PCO_RecorderCopyImage(this->hRec,this->cam, PCO_RECORDER_LATEST_IMAGE, 1, 1, this->imgWidth, this->imgHeight, imgBuffer, NULL, &metadata, NULL);

								if (IsNoErrorOccured(iRet, "Error occured"))
								{
									datapack->height = this->imgHeight;
									datapack->width = this->imgWidth;
									datapack->buffersize = (_int64)imgWidth * (_int64)imgHeight;
									datapack->bufferptr = imgBuffer;

									// Buffer als return zurückgeben
									return datapack;
								}
			
					}

					else if (cameraTyp == 2)//PCO FLIM
					{

						CheckAndSetTriggerMode(1); // Trigger mode to "Soft-trigger"

						WORD forceTrigger = 1;
						iRet = PCO_ForceTrigger(this->cam, &forceTrigger);

						if (IsNoErrorOccured(iRet, "Error during the Trigger Command"))
						{
							int L = 2;
							WORD phasenumber;
							WORD phaseSymmetry;
							WORD phaseOrder;
							WORD tapSelect;

							iRet = PCO_GetFlimPhaseSequenceParameter(this->cam, &phasenumber, &phaseSymmetry, &phaseOrder, &tapSelect, NULL, NULL);
							PhaseNumber phase = static_cast<PhaseNumber>(phasenumber);

							if (IsNoErrorOccured(iRet, "Error while reading FLIM parameters"))
							{
								//number of phase images
								for (int i = 0; i < int(phase); i++)
								{
									L = L * 2;
								}

								bool acquisitionRunning = true;
								DWORD procImgCount = 0;
								PCO_METADATA_STRUCT metadata;
								metadata.wSize = sizeof(PCO_METADATA_STRUCT);

								bool imageSaved = false;
								DWORD imgNumber = 0;
								bool isRunning = true;

								while (acquisitionRunning)
								{
									// get the current available images 
									iRet = PCO_RecorderGetStatus(this->hRec, this->cam, &acquisitionRunning, NULL, NULL, &procImgCount, NULL, NULL, NULL, NULL, NULL);

									if (IsNoErrorOccured(iRet, "Error with the Recorder Status"))
									{
										DWORD warn = 0, err = 0, status = 0;

										iRet = PCO_GetCameraHealthStatus(this->cam, &warn, &err, &status);
										if (err != PCO_NOERROR)
											PCO_RecorderStopRecord(this->hRec, this->cam); //Stop record on health error

										Sleep(50);

										// check if the camera has took L images
										if (procImgCount >= L)
										{
											acquisitionRunning = false;
											break;
										}
									}
								}

								//Send L images to datapack
								//datapack->height = this->imgHeight;
								//datapack->width = this->imgWidth;
								//datapack->buffersize = (_int64)this->imgHeight * (_int64)this->imgWidth;
								//datapack->imgNumber = L;

								//char* file_name{ new char[200] };
								//file_name = strcpy(file_name, "C:\\\\LSFM_Software\\\\LSFM_Interface\\\\LSFM_Interface\\\\tmp");

								

									//char filename[200];

									//sprintf_s(filename, "\\image_live_%03d.tif", i);

									//char* TotalLine{ new char[strlen(file_name) + strlen(filename) + 1] };
									//TotalLine = strcpy(TotalLine, file_name);
									//TotalLine = strcat(TotalLine, filename);

									//Allocate memory for one image
									WORD* imgBuffer = new WORD[(__int64)imgWidth * (__int64)imgHeight];
									Sleep(10);

									iRet = PCO_RecorderCopyImage(this->hRec, this->cam, 1, 1, 1, imgWidth, imgHeight, imgBuffer, NULL, &metadata, NULL);


									//iRet = PCO_RecorderSaveImage(imgBuffer, imgWidth, imgHeight, FILESAVE_IMAGE_BW_16, false, TotalLine, true, &metadata);

									if (IsNoErrorOccured(iRet, " Error during image storage  "))
									{
										datapack->height = this->imgHeight;
										datapack->width = this->imgWidth;
										datapack->buffersize = (_int64)imgWidth * (_int64)imgHeight;
										datapack->bufferptr = imgBuffer;

										// Buffer als return zurückgeben
										return datapack;
									}

							}

						}

					}
				}
			}
		}
	}

}


bool LSFM_Camera::SetFlimPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect, DWORD frequency)
{
	bool value = false;

	int cameraType = this->GetCameraIdentification();

	if (cameraType == 1)

		cout << "Method just allowed  for FLIM Camera" << endl;

	else if (cameraType == 2)
	{
		WORD phaseNumberparameter = phaseNumber;
		WORD phaseSymmetryparameter = phasesymmetry;
		WORD phaseOrderparameter = phaseOrder;
		WORD tapSelectparameter = tapSelect;

		int iRet = PCO_RecorderStopRecord(this->hRec, this->cam);

		iRet = PCO_SetFlimPhaseSequenceParameter(this->cam, phaseNumberparameter, phaseSymmetryparameter, phaseOrderparameter, tapSelectparameter, 0, 0);

		if (IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		{

			// Set The Asymmetry correction to ON 
			iRet = PCO_SetFlimImageProcessingFlow(this->cam, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);

			if (IsNoErrorOccured(iRet, "Error during the Image processing Flow"))
			{

				iRet = PCO_SetFlimMasterModulationFrequency(this->cam, frequency);

				if (IsNoErrorOccured(iRet, "Problem with the Modulation  Frequency\n"))
				{

					iRet = PCO_ArmCamera(this->cam);

					if (IsNoErrorOccured(iRet, "Error with the Initialisation of Parameters"))
					{

						//Start camera
						iRet = PCO_RecorderStartRecord(this->hRec, NULL);


						WORD asymetricCorrection, param1, param2, param3, param4, param5, param6, param7, param8, param9;
						iRet = PCO_GetFlimImageProcessingFlow(this->cam, &asymetricCorrection, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9);

						if (IsNoErrorOccured(iRet, "Error with the Acquisition of Asymmetry Correction "))
						{

							cout << "Aymmetry Correction = " << asymetricCorrection << endl;

							WORD phaseNumberparameter, phaseSymmetryparameter, phaseOrderparameter, tapSelectparameter;

							iRet = PCO_GetFlimPhaseSequenceParameter(this->cam, &phaseNumberparameter, &phaseSymmetryparameter, &phaseOrderparameter, &tapSelectparameter, NULL, NULL);

							if (IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
							{
								cout << "phaseNumber = " << phaseNumberparameter << endl;
								cout << "phaseSymmetry = " << phaseSymmetryparameter << endl;
								cout << "phaseOrder = " << phaseOrderparameter << endl;
								cout << "tapSelect = " << tapSelectparameter << endl;


								DWORD frequency;

								iRet = PCO_GetFlimMasterModulationFrequency(this->cam, &frequency);

								if (IsNoErrorOccured(iRet, "Problem with the Acquisition of the Frequency\n"))
								{

									cout << "Frequency = " << frequency << endl;
									value = true;

								}

							}

						}
					}
				}

			}

		}

		else
			value = false;
	}

	return value;
}

bool LSFM_Camera::Set_Camera_Master_modulation_frequency(DWORD frequency)
{
	printf("Method is not allowed, this camera is not a FLIM Camera\n");

	return false;
}

long int  LSFM_Camera::Get_Camera_Modulation_Frequency()
{
	printf("Method is not allowed, this camera is not a FLIM Camera\n");

	return false;

}

bool  LSFM_Camera::Set_Camera_Modulation_Frequency(DWORD waveform)
{
	bool value = false;

	int cameraType = this->GetCameraIdentification();

	if (cameraType == 1)

		cout << "Method Just allowed for FLIM Camera" << endl;


	else if (cameraType == 2)
	{
		int iRet = PCO_SetFlimModulationParameter(this->cam, 0, waveform, NULL, NULL);

		if (IsNoErrorOccured(iRet, "Problem with the Modulation  Frequency\n"))
			value = true;
		else
			value = false;
	}

	return value;
}


bool LSFM_Camera::GetFlimPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect)
{
	bool value = false;

	int cameraType = this->GetCameraIdentification();

	if (cameraType == 1)

		cout << "Method not allowed just for FLIM Camera" << endl;

	else if (cameraType == 2)
	{
		WORD phaseNumberparameter = *phaseNumber;
		WORD phaseSymmetryparameter = *phasesymmetry;
		WORD phaseOrderparameter = *phaseOrder;
		WORD tapSelectparameter = *tapSelect;


		int iRet = PCO_GetFlimPhaseSequenceParameter(this->cam, &phaseNumberparameter, &phaseSymmetryparameter, &phaseOrderparameter, &tapSelectparameter, NULL, NULL);

		if (IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		{
			*phaseNumber = static_cast<PhaseNumber>(phaseNumberparameter);
			*phasesymmetry = static_cast<PhaseSymmetry>(phaseSymmetryparameter);
			*phaseOrder = static_cast<PhaseOrder>(phaseOrderparameter);
			*tapSelect = static_cast<TapSelect>(tapSelectparameter);

			value = true;
		}
		else
			value = false;
	}

	return value;

}

bool LSFM_Camera::GetCameraStatusConnection()
{
	if (!this->isNoCameraConnected)
		return true;
	else
		return false;
}

int LSFM_Camera::GetCameraIdentification()
{
	int cameraType = 0;
	PCO_CameraType strCamType;
	strCamType.wSize = sizeof(PCO_CameraType);
	iRet = PCO_GetCameraType(this->cam, &strCamType);


	if (IsNoErrorOccured(iRet, "Error with the Acquisition of the Camera Type"))
	{

		switch (strCamType.wCamType)
		{
		case 4896:
			cameraType = 1;
			break;
		case 5120:
			cameraType = 2;
			break;
		}

	}


	return  cameraType;



}

bool LSFM_Camera::IsHardwareConnected()
{
	return this->isNoCameraConnected;
}
