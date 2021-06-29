#include "LSFM_FLIM_Camera.h"
#include "pch.h"
#include "include/file12.h"
#include "include/file12.cpp"

BOOL LSFM_FLIM_Camera::SetFilmPhaseSequenceParameter(PhaseNumber phaseNumber, PhaseSymmetry phasesymmetry, PhaseOrder phaseOrder, TapSelect tapSelect)
{
	int iRet = PCO_SetFlimPhaseSequenceParameter(this->flimCam, this->phasenumber, this->phasesymmetry, this->phaseOrder, this->tapSelect, NULL, NULL);

	if (LSFM_Camera::IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		return true;
	else
		return false;
}

long int LSFM_FLIM_Camera::Get_Camera_Modulation_Frequency()
{
	WORD wSourceSelect = 0;
	WORD wOutputWaveform = 0;
	int iRet = PCO_GetFlimModulationParameter(this->flimCam, &wSourceSelect, &wOutputWaveform, NULL, NULL);
	if (LSFM_Camera::IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		return wOutputWaveform;
	else
		return 0 ;
}
BOOL LSFM_FLIM_Camera::Set_Camera_Modulation_Frequency(DWORD waveform)
{
	int iRet = PCO_SetFlimModulationParameter(this->flimCam, 0, waveform, NULL, NULL);

	if (LSFM_Camera::IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
		return true;
	else
		return false;
}
BOOL LSFM_FLIM_Camera::GetFilmPhaseSequenceParameter(PhaseNumber* phaseNumber, PhaseSymmetry* phasesymmetry, PhaseOrder* phaseOrder, TapSelect* tapSelect)
{
	WORD phaseNumberparameter = *phaseNumber;
	WORD phaseSymmetryparameter = *phasesymmetry;
	WORD phaseOrderparameter = *phaseOrder;
	WORD tapSelectparameter = *tapSelect;
	

	int iRet = PCO_GetFlimPhaseSequenceParameter(this->flimCam, &phaseNumberparameter, &phaseSymmetryparameter, &phaseOrderparameter, &tapSelectparameter, NULL, NULL);

	if (LSFM_Camera::IsNoErrorOccured(iRet, "Problem with the Recording State\n"))
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

	while (isNoCameraConnected)
	{
		printf("Get Handle to connected camera\n");
		this->iRet = PCO_OpenCamera(&this->flimCam, 0);
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
			iRet = PCO_GetCameraSetup(this->flimCam, &m_wType, &m_dwSetup[0], &m_wLen);
			if (IsNoErrorOccured(iRet, "Error with the retribution of the Camera Setup"))
			{
				if (m_dwSetup[0] == PCO_EDGE_SETUP_GLOBAL_RESET)
				{
					m_dwSetup[0] == PCO_EDGE_SETUP_ROLLING_SHUTTER;

					PCO_SetCameraSetup(this->flimCam, m_wType, &m_dwSetup[0], m_wLen);
					PCO_RebootCamera(this->flimCam);

					this->Disconnect();
					break;
				}
				else
				{

					CheckAndSetTriggerMode(0);

					this->cameraMode = PCO_RECORDER_MEMORY_RINGBUF;

					Prepare_Recording(PCO_RECORDER_MODE_MEMORY);

					//Init Recorder
					iRet = PCO_RecorderInit(this->flimHrec, reqImgCountArr, 1, this->cameraMode, 0, NULL, NULL);

					if (IsNoErrorOccured(iRet, "Error with the Initialisation of the Recorder"))
					{

						//Get image size
						iRet = PCO_RecorderGetSettings(this->flimHrec, hCamArr[1], NULL, NULL, NULL, &this->imgWidth, &this->imgHeight, NULL);


						//Start camera
						iRet = PCO_RecorderStartRecord(this->flimHrec, NULL);

						isNoCameraConnected = false;

						break;
					}

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


