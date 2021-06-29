#pragma once

class IFilterController
{
public:
	virtual ~IFilterController() {}

	virtual bool MoveToSelectedPosition(int position) = 0;
	virtual int ActualFIlterPosition() = 0;
	virtual bool GetAllPositionName(char* positionsList) = 0;

};