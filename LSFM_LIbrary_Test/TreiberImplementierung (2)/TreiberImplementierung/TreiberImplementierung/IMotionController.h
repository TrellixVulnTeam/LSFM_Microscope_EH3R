#pragma once

// Interface für Motion Controller. here is stored all  controller-specific methods  that can be called in the main function  
class IMotionController
{

public:
	virtual ~IMotionController() {}
	virtual double getActualPosition() = 0;
	virtual char* getIdentification() = 0; // Method to get the Identification of A Controller 
	virtual bool MoveRelativXYZ(int stage, double position) = 0;
	virtual bool MoveAbsolutXYZ(int stage, double position) = 0;
	virtual int GetStage() = 0;
	virtual bool GetMotionStatusConnection() = 0;

};

