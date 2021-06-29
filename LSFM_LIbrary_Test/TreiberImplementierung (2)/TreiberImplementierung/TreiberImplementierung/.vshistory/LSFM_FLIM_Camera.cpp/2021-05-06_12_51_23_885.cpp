
#include "LSFM_FLIM_Camera.h"


using namespace std;


bool LSFM_FLIM_Camera::SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect)
{
	int iRet = PCO_SetFlimPhaseSequenceParameter(this->flimCam, this->phasenumber, this->phasesymmetry, this->phaseOrder, this->tapSelect, NULL, NULL);

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

			
					LSFM_Camera::CheckAndSetTriggerMode(this->flimCam, 0);

					this->cameraMode = PCO_RECORDER_MEMORY_RINGBUF;

					LSFM_Camera::Prepare_Recording(this->flimCam, &this->flimHrec, PCO_RECORDER_MODE_MEMORY);

					

					if (IsNoErrorOccured(iRet, "Error with the Initialisation of the Recorder"))
					{


						bool result = SetFilmPhaseSequenceParameter(PhaseNumber::eight, PhaseSymmetry::twice, PhaseOrder::opposite, TapSelect::both);

						 result = Set_Camera_Modulation_Frequency(1);

						 result = Set_Camera_Master_modulation_frequency(2000000);
						//int camera_Modulation_frequency = Get_Camera_Modulation_Frequency();

						this->isNoFlimCameraConnected = false;

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







