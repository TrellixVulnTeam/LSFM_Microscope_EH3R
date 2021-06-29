#pragma once

// Interface für Motion Controller. here is stored all  controller-specific methods  that can be called in the main function  
class IMotionController
{

public:
	virtual ~IMotionController() {}
	virtual double getActualPosition() = 0;
	virtual char* getIdentification() = 0; // Method to get the Identification of A Controller 

};

