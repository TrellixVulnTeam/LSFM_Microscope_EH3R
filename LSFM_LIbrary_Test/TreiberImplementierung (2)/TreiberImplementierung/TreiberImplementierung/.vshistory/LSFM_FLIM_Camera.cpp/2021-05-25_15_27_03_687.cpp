
#include "LSFM_FLIM_Camera.h"


using namespace std;


bool LSFM_FLIM_Camera::SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect)
{
	WORD phaseNumberparameter = phaseNumber;
	WORD phaseSymmetryparameter = phasesymmetry;
	WORD phaseOrderparameter = phaseOrder;
	WORD tapSelectparameter =  tapSelect;

	int iRet = PCO_SetFlimPhaseSequenceParameter(this->flimCam, phaseNumberparameter, phaseSymmetryparameter, phaseOrderparameter, tapSelectparameter, 0, 0);

	if (IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		return true;
	else
		return false;
}

long int LSFM_FLIM_Camera::Get_Camera_Modulation_Frequency()
{
	WORD wSourceSelect = 0;
	WORD wOutputWaveform = 0;
	int iRet = PCO_GetFlimModulationParameter(this->flimCam, &wSourceSelect, &wOutputWaveform, NULL, NULL);
	if (IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		return wOutputWaveform;
	else
		return 0 ;
}
bool LSFM_FLIM_Camera::Set_Camera_Modulation_Frequency(DWORD waveform)
{
	int iRet = PCO_SetFlimModulationParameter(this->flimCam, 0, waveform, NULL, NULL);

	if (IsNoErrorOccured(iRet, "Problem with the Modulation  Frequency\n"))
		return true;
	else
		return false;
}

bool LSFM_FLIM_Camera::Set_Camera_Master_modulation_frequency(DWORD frequency)
{
	int iRet = PCO_SetFlimMasterModulationFrequency(this->flimCam, frequency);

	if (IsNoErrorOccured(iRet, "Problem with the Modulation  Frequency\n"))
		return true;
	else
		return false;
}
bool LSFM_FLIM_Camera::GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect)
{
	WORD phaseNumberparameter = *phaseNumber;
	WORD phaseSymmetryparameter = *phasesymmetry;
	WORD phaseOrderparameter = *phaseOrder;
	WORD tapSelectparameter = *tapSelect;
	

	int iRet = PCO_GetFlimPhaseSequenceParameter(this->flimCam, &phaseNumberparameter, &phaseSymmetryparameter, &phaseOrderparameter, &tapSelectparameter, NULL, NULL);

	if (LSFM_FLIM_Camera::IsNoErrorOccured( iRet, "Problem with the Recording State\n"))
	{
		*phaseNumber = static_cast<PhaseNumber>(phaseNumberparameter);
		*phasesymmetry = static_cast<PhaseSymmetry>(phaseSymmetryparameter);
		*phaseOrder = static_cast<PhaseOrder>(phaseOrderparameter);
		*tapSelect = static_cast<TapSelect>(tapSelectparameter);

		return true;
	}
	else
		return false;
}

void LSFM_FLIM_Camera::Connect(){

	 if (this->isNoFlimCameraConnected)
	{
		printf("Get Handle to connected camera\n");
		this->iRet = PCO_OpenCamera(&this->flimCam, 0);
		if (this->iRet != PCO_NOERROR)
		{
			this->CloseConnectionWithComment(this->iRet, "No Camera found\n");
		}
		else
		{
			printf("Connection with the Camera successful\n");

			PCO_CameraType strCamType;

			iRet = PCO_GetCameraType(this->flimCam, &strCamType);
			if (IsNoErrorOccured(iRet, "Error with the Type of Camera "))


			        // Die Kamera auf Softtrigger stellen 
					LSFM_Camera::CheckAndSetTriggerMode(this->flimCam, 1);

					//  First step to the Initialisation of the :  Recorder Typ: FIFO
					this->cameraMode = PCO_RECORDER_MEMORY_FIFO;

					//Set the mode to  Memory,
					LSFM_Camera::Prepare_Recording(this->flimCam, &this->flimHrec, PCO_RECORDER_MODE_MEMORY);

					// Define the number of images for the sequences: 10 sequences * 32(maximal nimber of images for one sequence)

					reqImgCountArr[0] = 320;

					//Init Recorder
					iRet = PCO_RecorderInit(this->flimHrec, reqImgCountArr, 1, this->cameraMode, 0, NULL, NULL);

					if (IsNoErrorOccured(iRet, "Error with the Initialisation of the Recorder"))
					{

						//Start camera
						iRet = PCO_RecorderStartRecord(this->flimHrec, NULL);


						if (IsNoErrorOccured(iRet, "Error with the Start of  the Recorder"))
						{
							//Get image size
							iRet = PCO_RecorderGetSettings(this->flimHrec, hCamArr[0], NULL, NULL, NULL, &this->imgWidth, &this->imgHeight, NULL);


							// Set the flim Phase sequence parameter 
							bool result = SetFilmPhaseSequenceParameter(PhaseNumber::Four, PhaseSymmetry::twice, PhaseOrder::opposite, TapSelect::both);

							// Set the modulation waveForm: Sinusoidal 
							result = Set_Camera_Modulation_Frequency(1);


							// Set the modulation frequency by default to 30MHZ 
							result = Set_Camera_Master_modulation_frequency(3000000);


							// Set The Asymmetry correction to ON 
							iRet = PCO_SetFlimImageProcessingFlow(this->flimCam, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);

							if (IsNoErrorOccured(iRet, "Error during the Image processing Flow"))
							{



							}

							this->isNoFlimCameraConnected = false;

						}


						

							



					}

				}
			}
};

void LSFM_FLIM_Camera::Disconnect()
{
	printf("Stop camera and close connection\n");

	//Stop Recorder
	iRet = PCO_RecorderStopRecord(this->flimHrec, this->flimCam);

	//Delete Recorder
	iRet = PCO_RecorderDelete(this->flimHrec);

	iRet = PCO_CloseCamera(this->flimCam);

	printf("Connection Successful closed \n");
}




void LSFM_FLIM_Camera::GetCameraAndRecorderHandle(HANDLE* camera, HANDLE* recorder)
{
	*camera = this->flimCam;
	*recorder = this->flimHrec;
}

void LSFM_FLIM_Camera::CloseConnectionWithComment(DWORD dwerr, const char* comment)
{

	DWORD dwlen = 400;
	char pbuf[400];
	PCO_GetErrorText(dwerr, pbuf, dwlen);
	printf("%s\n", comment);
	printf("%s\n", pbuf);
	Disconnect();
}

bool LSFM_FLIM_Camera::IsNoErrorOccured(int iRet, const char* message)
{
	if (!iRet)
		return true;
	else
	{
		CloseConnectionWithComment(iRet, message);
		return false;
	}
}

bool LSFM_FLIM_Camera::GetCameraStatusConnection()
{
	if (!this->isNoFlimCameraConnected)
		return true;
	else
		return false;
}







