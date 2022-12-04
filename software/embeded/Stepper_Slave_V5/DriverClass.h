#ifndef DRIVER_CUSTOM_H
#define DRIVER_CUSTOM_Hs

#include <stdint.h>
#include <Arduino.h>

#include <TMCStepper.h>
//#include <TMCStepper_UTILITY.h>

#include <AccelStepper.h>

class MORJStepperDriver : public TMC2209Stepper, public AccelStepper {

public:

	bool _RDO_status = false;
	long _RDOfreq = 100;
	int _IncperRev;
	bool _led_active = false;

protected:

	Stream* _DATAport;
	uint8_t _address;

	int _EnablePin;

	int _GearRed;
	int _StperRev;
	int _microsteps;

	long _accel_rate = 10000;

	long _curr_command;

	bool _mot_locked;
	bool _mot_enabled;

	bool _initlock;

	int _Pin_LED;



public:

	MORJStepperDriver(Stream& _TMC_UART, float r_sense, uint8_t address, int StepPin, int DirPin, int MS1Pin, int MS2Pin);

	void begin(int GearReductionRatio, int StepsPerRevolution, int microstepping, int EnablePin, Stream& DATAport);

	void SetLedMode(bool mode);

	void SetMotorPrecision(int scale);

	void SetMotorAcceleration(long accel);

	void SetMotorCurrent(long current);

	void SetMotorEnable(bool enb);

	void SetZero();

	void StartRDO(long freq);

	void StopRDO();

	void StopMotor();

 long MORJStepperDriver::Turns_Calc();

	void LockToNearestZero();

	void Unlock();

	void NewCommand(int16_t variation);

	bool AbsCommand(int32_t command);

	//void AddKeyValue_JSON(String key, String value);

	//void Send_JSON();

	void connectLED(int Pin_LED);

	void EnableSpread(bool spread);

	bool EvaluateMovement();

	char* GetMotorScale(char* results);

private:

	//StaticJsonDocument<200> _buffer_stepDATA;

	void CalculateIncperRev();

	void WriteHEX_position(long data);

	//TMC2209Stepper::microsteps(scale);
	//TMC2209Stepper::en_spreadCycle(_mode);
	//AccelStepper::setMaxSpeed(_maxspeed);
	//AccelStepper::setAcceleration(_accel_rate);

	void AltLED();
};
#endif 
