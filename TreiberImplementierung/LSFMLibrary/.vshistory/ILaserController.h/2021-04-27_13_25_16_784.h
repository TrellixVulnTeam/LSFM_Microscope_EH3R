#pragma once
class ILaserController
{
public:
	virtual ~ILaserController() {}
	virtual void SetPower(float power) = 0;
	virtual void LaserOn() = 0;
	virtual void LaserOff() = 0;
	virtual void SetModulation(int modulation) = 0;
	virtual int ReadWaveLength() = 0;
	virtual void ReadStatus() = 0;
	virtual float ReadMaxPower() = 0;
	virtual float GetPowerInpercentage() = 0;
	virtual void GetOperatingMode() = 0;
	virtual void SetOperatingMode(int OperatingMode) = 0;
	virtual int  GetModulation() = 0;
	virtual int GetActualModulation() = 0;
	virtual bool IsHardwareConnected() = 0;


};
