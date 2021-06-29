#include "pch.h"
#include "LSFM_Camera.h"
#include "include/file12.h"
#include "include/file12.cpp"

	PCO_Description LSFM_Camera:: GetCameraDescription(HANDLE camera)
	{
		strDescription.wSize = sizeof(PCO_Description);
		this->iRet = PCO_GetCameraDescription(camera, &this->strDescription);//Sensor and camera specific description;

		return this->strDescription;
	}

	void LSFM_Camera::GetCameraHealthStatus(HANDLE camera)
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

	bool LSFM_Camera::GetRecodingstate(HANDLE camera)
	{
		int iRet = PCO_GetRecordingState(camera, &this->RecordingState);

		if (IsNoErrorOccured( iRet, "Problem with the Recording State\n"))
			return true;
		else
			return false;

	}

	bool LSFM_Camera::SetRecordingstate(HANDLE camera, WORD recordingstate)
	{
		if (this->GetRecodingstate(camera))
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
				int iRet = PCO_SetRecordingState(camera, recordingstate);
				if (IsNoErrorOccured(iRet, "Problem with the Recording State"))
					return true;
				else
					return false;
			}
		}

	}

	void LSFM_Camera::RecordedStateOn(HANDLE camera) { this->SetRecordingstate(camera, 1); }

	void LSFM_Camera::RecordedStateOff(HANDLE camera) { this->SetRecordingstate(camera, 0); }

	bool LSFM_Camera::ResetToDefaultParameters(HANDLE camera)
	{
		int iRet = PCO_ResetSettingsToDefault(camera);
		if (IsNoErrorOccured(iRet, "Problem with the Reset to Default parameter\n"))
			return true;
		else
			return false;
	}

	bool LSFM_Camera::CreateRecorder(HANDLE camera, HANDLE recorder, DWORD mode, char localDisk, DWORD* maxImgCountArr)
	{
		recorder = NULL;

		hCamArr[0] = camera;

		DWORD imgDistributionArr[1] = { 1 }; //Array defining the memory distribution between the used cameras(can be set to NULL for equal distribution)
											 // //Set image distribution to 1 since only one camera is used



		int iRet = 0;

		switch (mode)
		{
		case PCO_RECORDER_MODE_MEMORY:
			iRet = PCO_RecorderCreate(&recorder, hCamArr, imgDistributionArr, 1, mode, NULL, maxImgCountArr);
			break;
		case PCO_RECORDER_MODE_FILE:
			iRet = PCO_RecorderCreate(&recorder, hCamArr, imgDistributionArr, 1, mode, localDisk, maxImgCountArr);
			break;
		default:
			break;
		}

		if (IsNoErrorOccured(iRet, "Unable to create a recording session"))
			return true;
		else
			return false;


	}

	bool LSFM_Camera::GetAndSave_Image(HANDLE camera, char* file_name)
	{
		WORD XResAct, YResAct, XResMax, YResMax;
		DWORD bufsize;

		iRet = PCO_GetSizes(camera, &XResAct, &YResAct, &XResMax, &YResMax);

		bufsize = XResAct * YResAct * sizeof(WORD);


		HANDLE BufEvent = NULL;
		short BufNum = -1;
		WORD* BufAdr = NULL;

		WORD* imgBuffer = NULL;

		iRet = PCO_AllocateBuffer(camera, &BufNum, bufsize, &BufAdr, &BufEvent);

		iRet = PCO_SetImageParameters(camera, XResAct, YResAct, IMAGEPARAMETERS_READ_WHILE_RECORDING, NULL, 0);

		// Start Image Acquisition 
		iRet = PCO_GetImageEx(camera, 1, 0, 0, BufNum, XResAct, YResAct, 16);
		jetraw_tiff_init();
		if (iRet != PCO_NOERROR)
		{
			CloseConnectionWithComment(iRet, "Image Acquisitiion failed \n");
		}
		//Ende "Single_Image_Acquisition"
		else
		{
			printf("done ");
			//Funktion "Save_Single_Image"
			store_tiff(file_name, XResAct, YResAct, 0, BufAdr);




			dp_tiff* tiff = nullptr;
			

			/*
			iRet = PCO_GetBuffer(cam, BufNum, &imgBuffer, &BufEvent);

			auto status = jetraw_tiff_open(L"C:\\Users\\admin\\Documents\\Visual Studio 2019\\Bilder\\jetraw.tif", XResAct, YResAct, "Test file", &tiff);
			printf("jetraw_tiff_open(): %d\n", status);
			status = jetraw_tiff_append(tiff, imgBuffer);
			printf("jetraw_tiff_append(): %d\n", status);
			status = jetraw_tiff_close(&tiff);
			printf("jetraw_tiff_close(): %d\n", status);
			printf("and stored to %s", file_name);
			//Ende "Save_Single_Image"
			printf("\n");
			*/


		}


		iRet = PCO_FreeBuffer(camera, BufNum);
		//Ende "End_recording"

		return true;
	}





	void  LSFM_Camera::Prepare_Recording(HANDLE camera, HANDLE recorder, DWORD mode)
	{
		int iRet;




		//Some frequently used parameters for the camera
		//DWORD numberOfImages = 10;
		DWORD expTime = 100;
		WORD expBase = TIMEBASE_MS;
		WORD metaSize = 0, metaVersion = 0;


		//set camera to default state, recording off
		this->RecordedStateOff(camera);


		//set noise filter off
		iRet = PCO_SetNoiseFilterMode(camera, 0);
		//iRet = PCO_ResetSettingsToDefault(hCamArr[0]);
		//Do some settings
		iRet = PCO_SetTimestampMode(camera, TIMESTAMP_MODE_OFF);
		iRet = PCO_SetMetaDataMode(camera, METADATA_MODE_ON, &metaSize, &metaVersion);
		//Set Exposure time
		iRet = PCO_SetDelayExposureTime(camera, 0, expTime, 2, expBase);

		iRet = PCO_ArmCamera(camera);


		//Reset Recorder to make sure a no previous instance is running
		iRet = PCO_RecorderResetLib(false);

		//Create Recorder (mode: memory sequence)
		//WORD mode = PCO_RECORDER_MODE_MEMORY | PCO_RECORDER_MODE_USE_DPCORE;

		if (CreateRecorder(camera, recorder, mode, NULL, maxImgCountArr))
		{
			//Set required images
			if (reqImgCountArr[0] > maxImgCountArr[0])
				reqImgCountArr[0] = maxImgCountArr[0];
		}
	}

	void LSFM_Camera::CheckAndSetTriggerMode(HANDLE camera, WORD requiredMode)
	{
		WORD  triggermode;
		iRet = PCO_GetTriggerMode(camera, &triggermode);

		if (IsNoErrorOccured( iRet, "Error during Trigger mode  acquisition "))
		{
			if (triggermode != requiredMode)
			{
				iRet = PCO_SetTriggerMode(camera, requiredMode);
				if (IsNoErrorOccured(iRet, "Error during Trigger mode Selection"))
				{
				}
			}

		}

	}


	

	void LSFM_Camera::Connect()
	{
		while (isNoCameraConnected)
		{
			printf("Get Handle to connected camera\n");
			this->iRet = PCO_OpenCamera(&this->cam, 0);
			if (this->iRet != PCO_NOERROR)
			{
				this->CloseConnectionWithComment(this->iRet, "No Camera found\n");
				break;
			}
			else
			{
				printf("Connection with the Camera successful\n");

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
						break;
					}
					else
					{

						CheckAndSetTriggerMode(this->cam, 0);

						this->cameraMode = PCO_RECORDER_MEMORY_RINGBUF;

						Prepare_Recording(this->cam,this->hRec,  PCO_RECORDER_MODE_MEMORY);

						//Init Recorder
						iRet = PCO_RecorderInit(hRec, reqImgCountArr, 1, this->cameraMode, 0, NULL, NULL);

						if (IsNoErrorOccured(iRet, "Error with the Initialisation of the Recorder"))
						{

							//Get image size
							iRet = PCO_RecorderGetSettings(hRec, hCamArr[0], NULL, NULL, NULL, &this->imgWidth, &this->imgHeight, NULL);


							//Start camera
							iRet = PCO_RecorderStartRecord(hRec, NULL);

							isNoCameraConnected = false;

							break;
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

	WORD LSFM_Camera::GetExposureAndDelayTime(HANDLE camera)
	{

		WORD wTimeBaseDelay;
		WORD wTimeBaseExposure;

		PCO_GetDelayExposureTime(camera, &this->dwDelay, &this->dwExposure, &wTimeBaseDelay, &wTimeBaseExposure);

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

		return this->dwExposure;
	}

	void LSFM_Camera::SetExposureTime(HANDLE camera, DWORD value, DWORD unit)
	{
		this->dwExposure = value;
		this->dwDelay = 0;
		WORD wTimeBaseDelay = 0;
		WORD wTimeBaseExposure = unit;
		int idx = PCO_SetDelayExposureTime(camera, this->dwDelay, this->dwExposure, wTimeBaseDelay, wTimeBaseExposure);
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

	void LSFM_Camera::add(Component* component)
	{
		// Ausnahme werfen
		cout << "Ein Blatt kann keine Kinder haben" << endl;
	}

	void LSFM_Camera::remove(Component* component) {
		// Ausnahme werfen
		cout << "Ein Blatt kann keine Kinder entfernen" << endl;

	}

	void LSFM_Camera::SingleImageAquisition(HANDLE camera, char* file_name)
	{
		CheckAndSetTriggerMode(camera, 1); // Trigger Mode to "Software_trigger"

		WORD wCameraBusyState;
		iRet = PCO_GetCameraBusyStatus(camera, &wCameraBusyState);
		if (IsNoErrorOccured(iRet, "Eror during the Camera Busy Status acquisiton"))
		{
			if (wCameraBusyState == 1)
				printf(" Camera is Busy, not ready for a new trigger command");
			else
			{
				WORD forceTrigger = 1;
				iRet = PCO_ForceTrigger(camera, &forceTrigger);

				if (IsNoErrorOccured(iRet, "Error during the Trigger Command"))
				{

					GetAndSave_Image(camera, file_name);


				}

			}
		}
	}

	void LSFM_Camera::Start_Sequence(HANDLE camera)

	{

		PCO_SetTriggerMode(camera, 1); // Trigger mode to "Software-Trigger"

	}

	DataPack* LSFM_Camera::Live_View(HANDLE camera, HANDLE recorder)
	{
		CheckAndSetTriggerMode(camera, 0); // Trigger mode to "Auto-sequence"

				//Wait until acquisition is finished (all other parameters are ignored)
		bool acquisitionRunning = true;

		//Get the images currently available
		DWORD procImgCount = 0;
		PCO_METADATA_STRUCT metadata;
		metadata.wSize = sizeof(PCO_METADATA_STRUCT);
		bool imageSaved = false;
		DWORD imgNumber = 0;
		bool isRunning = true;
		DataPack *datapack = new DataPack() ;

		while (acquisitionRunning)
		{
			iRet = PCO_RecorderGetStatus(recorder, camera, &acquisitionRunning, NULL, NULL, &procImgCount, NULL, NULL, NULL, NULL, NULL);


			DWORD warn = 0, err = 0, status = 0;

			iRet = PCO_GetCameraHealthStatus(camera, &warn, &err, &status);
			if (err != PCO_NOERROR)
				PCO_RecorderStopRecord(recorder, camera); //Stop record on health error

			Sleep(10);

			if (procImgCount == reqImgCountArr[0])
			{
				acquisitionRunning = true;
				break;
			}
		}


		//Allocate memory for one image
		WORD* imgBuffer = NULL;
		imgBuffer = new WORD[(__int64)this->imgWidth * (__int64)this->imgHeight];


		iRet = PCO_RecorderCopyImage(recorder, hCamArr[0], PCO_RECORDER_LATEST_IMAGE, 1, 1, this->imgWidth, this->imgHeight, imgBuffer, NULL, &metadata, NULL);

		if (IsNoErrorOccured(iRet, "Error occured"))
		{
			datapack->height = this->imgHeight;
			datapack->width= this->imgWidth;
			datapack->buffersize = (__int64)this->imgWidth * (__int64)this->imgHeight;
			datapack->bufferptr = imgBuffer;
			// Buffer als return zurückgeben
			return datapack;
		}

	}

	void LSFM_Camera::GetCameraAndRecorderHandle(HANDLE *camera, HANDLE *recorder)
	{
		*camera =  this->cam;
		*recorder = this->hRec;

	}

	bool  LSFM_Camera::SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect)
	{
		printf("Metohd are not allowed, this camera is not a Flim Camera\n");

		return false;
	}

	long int  LSFM_Camera::Get_Camera_Modulation_Frequency()
	{
		printf("Metohd are not allowed, this camera is not a Flim Camera\n");

		return false;

	}

	bool  LSFM_Camera::Set_Camera_Modulation_Frequency(DWORD waveform)
	{
		printf("Metohd are not allowed, this camera is not a Flim Camera\n");

		return false;
	}

	bool  LSFM_Camera::GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect)
	{
		printf("Metohd are not allowed, this camera is not a Flim Camera\n");

		return false;

	}


	
	