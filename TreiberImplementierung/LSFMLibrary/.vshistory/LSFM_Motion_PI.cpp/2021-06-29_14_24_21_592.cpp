#include "pch.h"
#include "LSFM_Motion_PI.h"



double LSFM_Motion_PI::go_left()
	{
		double PI_positivLimit = 0;
		bool bFlag;

		/*The PI c413 controller does not have the FPL and FNL methode which allows to navigate to the smallest and largest position of the controller,
			but it is possible to navigate there knowing already the values of these positions.*/

		if (isPIC413)
		{
			double dPos[3];

			// move the axis the corresponding position in 'dPos'
			dPos[0] = -5.0; // mov first axis in the string 'axes' to position 0.0

			// call the MOV command (for closed servo loop).
			if (ControllerReady())
			{

				if (!PI_MOV(this->id, this->szAxis, dPos))
				{
					//CloseConnectionWithComment("Could not collect the positiv Value\n");
				}
				else
				{
					if (ControllerDone())
					{

						if (PI_qPOS(this->id, this->szAxis, dPos))
						{
							PI_positivLimit = dPos[0];
						}


					}

				}
			}

		}

		if (ControllerReady())
		{
			if (PI_FPL(this->id, this->szAxis))
			{
				if (ControllerDone())
				{
					if (PI_qPOS(this->id, this->szAxis, &PI_positivLimit))
					{

						printf("Current Value Succesfull save\n");
					}
					else
					{
						//CloseConnectionWithComment("Could not collect the positiv Value\n");

					}
				}

			}
		}


		return PI_positivLimit;
	}

	double  LSFM_Motion_PI::go_right()
	{
		double PI_negativLimit = 0;
		bool bFlag;
		bool controllerReady = false;

		if (isPIC413)
		{
			double dPos[3];

			// move the axis the corresponding position in 'dPos'
			dPos[0] = 5.0; // mov first axis in the string 'axes' to position 2.0

			// call the MOV command (for closed servo loop).

			if (ControllerReady())
			{
				if (!PI_MOV(this->id, szAxis, dPos))
				{
					//CloseConnectionWithComment("Could not collect the positiv Value\n");
				}
				else
				{
					if (ControllerDone())
					{
						if (PI_qPOS(this->id, szAxis, dPos))
						{
							PI_negativLimit = dPos[0];

						}
					}

				}
			}

		}

		
			if (ControllerReady())
			{
				if (PI_FNL(this->id, szAxis))
				{
					if (ControllerDone())
					{
						if (PI_qPOS(this->id, szAxis, &PI_negativLimit))            // method to get the current  position from the controller 
						{
						}
						else
						{
							//CloseConnectionWithComment("Could not collect the negativ Value\n");

						}
					}

				}
			}

		

		return PI_negativLimit;
	}

	void  LSFM_Motion_PI::setReferencing()
	{
		bool enableReferencing = 1;
		bool bFlag = false;
		bool run = true;
		//this->home = 0.5 * (round(this->x0) + round(this->x1));
		this->home = 37.5;

		if (isPIC413)
			this->home = 0.0;


		

			if (ControllerReady())
			{
				if (!PI_MOV(this->id, szAxis, &this->home))
					//CloseConnectionWithComment(" An error has occurred\n");
					printf("Error was occured");
				else
				{
					if (ControllerDone())
					{

					}

				}

				
			}
		
		

	}

	void  LSFM_Motion_PI:: CloseConnectionWithComment(const char* comment)
	{
		printf(comment);
		ReportError(this->id);
		PI_CloseConnection(this->id);

	}

	void  LSFM_Motion_PI::ReportError(int iD)
	{
		int err = PI_GetError(iD);
		char szErrMsg[300];
		if (PI_TranslateError(err, szErrMsg, 299))
		{
			printf("Error %d occured: %s\n", err, szErrMsg);
		}
	}

	bool  LSFM_Motion_PI::ControllerReady()
	{
		/*method to find out if the controller is available for a specific operation */
		BOOL bFlag = false;
		while (bFlag != TRUE)
		{
			!PI_IsControllerReady(this->id, &bFlag);
			
		}

		return true;
	}

	bool LSFM_Motion_PI::ControllerDone()
	{
		/*a method of determining whether the controller has completed its task.*/
		bool result = false;
		BOOL bFlag = true;
		while (bFlag != FALSE)
		{
			printf("#");
			!PI_IsMoving(this->id, szAxis, &bFlag);
			
		}


		return true;
	}


	//constructor of the class. For each instantiation of an object, the identifier and stage of the controller is recommended.
	LSFM_Motion_PI::LSFM_Motion_PI(char* cidentification, int st) {
		identification = new (char[100]);
		strcpy_s(identification, 100, cidentification);

		// check if the controller is  the PI C-413
		if (std::string(identification).compare(std::string("PI C-413 PIMag Motion Controller SN 0118024559 ")) == 0)
			isPIC413 = true;

		switch (st)
		{
		case 0:
			this->stage = 0;
			break;
		case 1:
			this->stage = 1;
			break;
		case 2:
			this->stage = 2;
			break;
		case 3:
			this->stage = 3;
			break;
		default:
			printf("Not defined Axis choose \n");
			break;

		}

	}

	void LSFM_Motion_PI::add(Component* component)
	{
		// Ausnahme werfen
		std::cout << "Ein Blatt kann keine Kinder haben" << std::endl;
	}

	void  LSFM_Motion_PI::remove(Component* component) 
	{
		// Ausnahme werfen
		std::cout << "Ein Blatt kann keine Kinder entfernen" << std::endl;

	}

	double  LSFM_Motion_PI::getActualPosition()
	{
		double dpos[3];
		double currrentPosition = 0.0;
		char szAxes[17];
		strcpy_s(szAxes, "1");
		

		
			if (ControllerReady())
			{
				
				
					
				if (PI_qPOS(this->id, szAxes, dpos))
				{
					this->actPosition = dpos[0];

				}
				else
				{
					ReportError(this->id);
				}


				

					   
						
					    
					

			}


		return this->actPosition;
	}


	void  LSFM_Motion_PI:: Connect() 
	{
		/*   there are two different types of controllers and the initialisation of the two different controllers is done in different ways:
			 1st controller -> PIC413
			 2nd controller -> PI 863
			 the initialisation of the isPIC413 controller requires, apart from the different steps of the other controllers, other methods, therefore the condition isPIC413*/
		system("CLS");
		printf("connection to controller %s in progress\n", identification);


		//method for connecting to a controller through its identifier
		this->id = PI_ConnectUSB(identification);

		if (this->id == -1)
		{
			printf("Could not connect to controller.\n");
		}
		else
		{
			if (isPIC413) // check if the device is a PIC413 controller 
			{
				/////////////////////////////////////////
				// Get the name of the connected axis. //
				 /////////////////////////////////////////
				char szAxes[17];
				if (!PI_qSAI(this->id, szAxes, 16))
				{
					CloseConnectionWithComment("SAI?: Error");
				}

				// Use only the first axis.
				strcpy_s(szAxes, "1");
				this->szAxis = szAxes;


				//////////////////////////////////////////
				// change the control mode to position. //
				//////////////////////////////////////////
				int bControlMode[3];

				// Switch on the Servo for all axes
				bControlMode[0] = 7; // servo on for the axis in the string 'axes'.

				// call the SerVO mode command.
				if (!PI_CMO(this->id, this->szAxis, bControlMode))
				{

					CloseConnectionWithComment("CMO: ERROR \n");

				}


				//////////////////////////
				// Reference the stage. //
				//////////////////////////

				// call the SerVO mode command.
				if (!PI_FRF(this->id, this->szAxis))
				{

					CloseConnectionWithComment("FRF: ERROR \n");

				}


				///////////////////////////////////////////
				// Wait until the end of reference move. //
				///////////////////////////////////////////
				if (ControllerDone())
				{

				}

				//////////////////////////
				// Perform a Auto Zero  //
				//////////////////////////
				BOOL bUseDefaultVoltageArray[3];
				bUseDefaultVoltageArray[0] = TRUE;

				// call the SerVO mode command.
				if (!PI_ATZ(this->id, this->szAxis, NULL, bUseDefaultVoltageArray))
				{
					CloseConnectionWithComment("ATZ: ERROR");

				}

				///////////////////////////////////////////
				// Wait until the end of auto zero.      //
				///////////////////////////////////////////
				BOOL bFlag = false;
				while (bFlag != TRUE)
				{
					if (!PI_IsControllerReady(this->id, &bFlag))
					{
						CloseConnectionWithComment("An error has occurred");
						break;

					}
					
				}

			}

			if (PI_SVO(this->id, this->szAxis, &flag) == FALSE)   // Set servo-control "on" or "off" (closed-loop/open-loop mode)
			{
				CloseConnectionWithComment("qIDN failed. Exiting.\n");

			}

			else
			{
				///////////////////////////////////////////
				// Wait until the end of auto zero.      //
				///////////////////////////////////////////

				printf("Connection with the Controller %s Successfull \n", identification);

				   // Sleep(2000);
					this->x0 = go_left();
					this->x1 = go_right();
					setReferencing();
                

			}

			this->isControllerConnected = true;
			printf("Controller connected");
		}


	}

	bool LSFM_Motion_PI:: GetMotionStatusConnection()
	{
		if (this->isControllerConnected)
			return true;
		else
			return false;
	}

	bool  LSFM_Motion_PI::MoveRelativXYZ(int stage, double position,bool acquisitionRunning)
	{

		bool result = FALSE;

		
		double actPosition = 0;
		double newPosition = 0;
		double dPos[3];
		char szAxes[17];
		strcpy_s(szAxes, "1");


		if (isPIC413)
		{


			if (ControllerReady())
			{
				if (PI_qPOS(this->id, szAxes, dPos))
				{
					actPosition = dPos[0];
					if (position == 0)
					{
						return true;
					}

					else
					{
						dPos[0] = position;
						bool request = PI_MVR(this->id, szAxes, dPos);

						while (true)
						{
							if (ControllerDone())
							{
								if (request == FALSE)
								{
									//CloseConnectionWithComment("qIDN failed. Exiting.\n");

									return false;
								}
								else
								{
									if (!acquisitionRunning)
									{
										Sleep(2000);

										PI_qPOS(this->id, szAxes, dPos);
										newPosition = dPos[0];
										this->actPosition = newPosition;
										return true;
									}
									


								}
							}
						}
						


					}
				}
				
				

			}



		}

		if (ControllerReady())
		{
			if (PI_qPOS(this->id, szAxis, &actPosition))
			{
				if (position ==  0)
				{

					result = true;
				}
				else
				{
					bool request = PI_MVR(this->id, szAxis, &position);
					bool bFlag = true;
					if (ControllerDone())
					{
						if (request == FALSE)
						{
							//CloseConnectionWithComment("qIDN failed. Exiting.\n");
						}
						else
						{
							if (!acquisitionRunning)
							{
								PI_qPOS(this->id, szAxis, &newPosition);
								this->actPosition = newPosition;

								result = true;
							}

						}
					}


				}

			}

		}

		//else
		//	CloseConnectionWithComment("Could not collect the  Value\n");


		return result;
	}

	bool  LSFM_Motion_PI::MoveAbsolutXYZ(int stage, double position, bool acquisitionRunning)
	{
		bool result = false;

		char szAxes[17];
		strcpy_s(szAxes, "1");
		double dPos[3];

		double actPosition = 0;
		double newPosition = 0;

		if (isPIC413)
		{

			if (position > 5 || position < -5)
			{
				printf("Position Out of range, Please select a value between 0 and 5 \n");

				result = FALSE;
			}
			else
			{
				while (true)
				{
					if (ControllerReady())
					{
						if (PI_qPOS(this->id, szAxes, dPos) == false)
						{
							CloseConnectionWithComment("qIDN failed. Exiting.\n");
						}
						else
						{
							actPosition = dPos[0];
							if (actPosition == position)
							{
								this->actPosition = actPosition;

								printf("\nController Already in the requested position \n ");
								result = true;


							}

							else
							{

								if (ControllerReady())
								{
									dPos[0] = position;
									PI_MOV(this->id, szAxes, dPos);
									
										//CloseConnectionWithComment("Could not collect the positiv Value\n");
									
									if (ControllerDone())
									{
										if (!acquisitionRunning)
										{
											Sleep(2000);

											PI_qPOS(this->id, szAxes, dPos);
											newPosition = dPos[0];
											this->actPosition = newPosition;
											printf("\nController successful Move to the Position: \n oldPosition = %f \n new position = %f \n ", actPosition, newPosition);

											result = true;
										}
									}
									
									/*
									else
									{
										
									}*/

								}


							}
						}

						break;
					}
					
				}
				





			}

		}

		else
		{

			if (position > 75.0 || position < 0.00)
			{
				printf("Position Out of range, Please select a value between 0 and 75 \n");

			}
			else
			{

				if (ControllerReady())
				{
					if (PI_qPOS(this->id, szAxis, &actPosition))
					{
						if (position == actPosition)
						{
							printf("the controller is already in the requested position. \n");

							result = true;
						}
						else
						{
							//Sleep(2000);
							bool request = PI_MOV(this->id, szAxis, &position);

							if (ControllerDone())
							{

								if (!acquisitionRunning)
								{
									//Sleep(2000);
									PI_qPOS(this->id, szAxis, &newPosition);
									this->actPosition = newPosition;
									printf("\nController successful Move to the Position: \n oldPosition = %f \n new position = %f \n ", actPosition, newPosition);

									result = true;
								}
								/*
								if (request == FALSE)
								{
									CloseConnectionWithComment("qIDN failed. Exiting.\n");
								}
								*/

								/*
								else
								{
									//Sleep(2000);
									PI_qPOS(this->id, szAxis, &newPosition);
									this->actPosition = newPosition;
									printf("\nController successful Move to the Position: \n oldPosition = %f \n new position = %f \n ", actPosition, newPosition);

									result = true;

								}
								*/
							}


						}

					}

				}


			}

		}
		return result;

	}

	void LSFM_Motion_PI::getIdentification(char* identificationValue)
	{
		identificationValue = identification;
	}

	int  LSFM_Motion_PI::GetStage()
	{
		printf(" Test \n ");

		return this->stage;
	}


	void  LSFM_Motion_PI::Disconnect()
	{

		PI_CloseConnection(this->id);
		this->id = -1;
	}

	bool LSFM_Motion_PI::IsHardwareConnected()
	{
		return this->isControllerConnected;
	}
