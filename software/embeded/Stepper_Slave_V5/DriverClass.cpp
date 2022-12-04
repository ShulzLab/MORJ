#include "DriverClass.h"
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <ArduinoJson.h>
//#include <TMCStepper.h>

MORJStepperDriver::MORJStepperDriver(Stream& _TMC_UART, float r_sense, uint8_t address, int StepPin, int DirPin, int MS1Pin, int MS2Pin) : TMC2209Stepper(&_TMC_UART, r_sense, address), AccelStepper(AccelStepper::DRIVER, StepPin, DirPin)
{
	pinMode(StepPin, OUTPUT);
	pinMode(DirPin, OUTPUT);

	pinMode(MS1Pin, OUTPUT);
	pinMode(MS2Pin, OUTPUT);
	digitalWrite(MS1Pin, LOW);
	digitalWrite(MS2Pin, LOW);

	_address = address;
	//AccelStepper(AccelStepper::DRIVER, StepPin , DirPin);
	//TMC2209Stepper(&TMC_UART, r_sense, address);
}

void MORJStepperDriver::begin(int GearReductionRatio, int StepsPerRevolution, int microstepping, int EnablePin, Stream& DATAport) {

	_EnablePin = EnablePin;
	pinMode(_EnablePin, OUTPUT);
	digitalWrite(_EnablePin, LOW);
	_mot_enabled = true;

	TMC2209Stepper::begin();
	_curr_command = 0;

	TMC2209Stepper::microsteps(microstepping);

	_GearRed = GearReductionRatio;
	_StperRev = StepsPerRevolution;
	_microsteps = microstepping;

	//_DATAport = &DATAport;
	//_DATAport = DATAport;

	CalculateIncperRev();

	TMC2209Stepper::en_spreadCycle(true);
}

void MORJStepperDriver::SetLedMode(bool mode) {
	_led_active = mode;
	if (!_led_active) {
		digitalWrite(_Pin_LED, LOW);
		digitalWrite(LED_BUILTIN, LOW);
	}
	else {
		digitalWrite(_Pin_LED, HIGH);
		digitalWrite(LED_BUILTIN, HIGH);
	}
}

void MORJStepperDriver::SetMotorPrecision(int scale) {
	TMC2209Stepper::microsteps(scale);
	_microsteps = scale;
	CalculateIncperRev();
}

void MORJStepperDriver::SetMotorAcceleration(long accel) {
	_accel_rate = accel;
	AccelStepper::setAcceleration(_accel_rate);
}

void MORJStepperDriver::SetMotorCurrent(long current) {
	TMC2209Stepper::rms_current(current);
}

void MORJStepperDriver::SetMotorEnable(bool enb) {
	digitalWrite(_EnablePin, !enb);
	_mot_enabled = enb;
}

void MORJStepperDriver::SetZero() {
  AccelStepper::stop();
	//if (!AccelStepper::isRunning()) {
	AccelStepper::setCurrentPosition(0);
  _curr_command = 0;
		//AddKeyValue_JSON(F("zero"), F("M")); //SEND A MESSAGE TO SAY THAT THE MOTOR JUST SET IT'S ZERO
		//Send_JSON();
	//}
}

void MORJStepperDriver::StartRDO(long freq) { // delay in microseconds
	//AddKeyValue_JSON(F("RDO_delay"), (String)((long)(1000000 / freq))); //SEND A MESSAGE TO SAY THAT THE MOTOR IS UNLOCKED
	_RDOfreq = freq;
	_RDO_status = true;
}

void MORJStepperDriver::StopRDO() { // delay in microseconds
	_RDO_status = false;
}

void MORJStepperDriver::StopMotor() {
	AccelStepper::stop();
	_curr_command = AccelStepper::targetPosition();
}


//long id MORJStepperDriver::EstimateTurns(){
//  long pos = AccelStepper::currentPosition();
//  long turns = 0;
//  int supress;
//  
//  if (pos > 0) {
//    supress = -1;
//  }
//  else if (pos < 0) {
//    supress = 1;
//  }
//  else (){
//    supress = 0;
//  }
//
//  while (true) {
//    if (abs(pos) >= _IncperRev) {
//      pos = pos + (supress * _IncperRev);
//      turns = turns + 1;
//    }
//    else {
//      break;
//    }
//  }
//}

long MORJStepperDriver::Turns_Calc() {
  long temp_position = _curr_command;
  long temp_turns = 0;
  while (temp_position > _IncperRev || temp_position < 0){
    if (temp_position > _IncperRev){
      temp_turns = temp_turns + 1;
      temp_position = temp_position - _IncperRev;
    }
    else{
      temp_turns = temp_turns - 1;
      temp_position = temp_position + _IncperRev;
    }
  }
  return temp_turns;
};

void MORJStepperDriver::LockToNearestZero() {

	//long pos = _curr_command;
	long pos = AccelStepper::currentPosition();
	long turns = 0;
	int supress;

	_initlock = 1;
	_mot_locked = 1;

	if (pos > 0) {
		supress = -1;
	}
	else if (pos < 0) {
		supress = 1;
	}
	else {
		AccelStepper::moveTo(0);
		//AddKeyValue_JSON(F("lock"), F("F"));
		return; /// !! \\\ possible particular case, to handle later
	}
	//_DATAport->println(supress);

	while (true) {
		if (abs(pos) >= _IncperRev) {
			pos = pos + (supress * _IncperRev);
			turns = turns + 1;
		}
		else {
			break;
		}
	}
	//_DATAport->println(pos);

	if (abs(pos) > (long)(0.5 * _IncperRev)) {
		turns = turns + 1;
	}

	//AddKeyValue_JSON(F("lock"), F("S")); //SEND A MESSAGE TO SAY THAT THE MOTOR STARTS LOCKING
	//Send_JSON();
	//_DATAport->println(- supress * turns * _IncperRev);
	AccelStepper::moveTo(-supress * turns * _IncperRev);
}


void MORJStepperDriver::Unlock() {
	if (_mot_locked) {
		TMC2209Stepper::microsteps(_microsteps);
		TMC2209Stepper::en_spreadCycle(true);
		//AddKeyValue_JSON(F("lock"), F("U")); //SEND A MESSAGE TO SAY THAT THE MOTOR IS UNLOCKED
		//Send_JSON();
		_mot_locked = 0;
		AccelStepper::moveTo(_curr_command);
	}
}

void MORJStepperDriver::NewCommand(int16_t variation) {
	if (variation < _IncperRev * 3) {
		_curr_command = _curr_command + variation;
		if (_mot_locked == false) {
			//_DATAport->write(0x23);
			//_DATAport->write((uint8_t*)&variation, sizeof(variation));
			//_DATAport->println();
			AccelStepper::moveTo(_curr_command);
		}
	}
	AltLED();
}

bool MORJStepperDriver::AbsCommand(int32_t command) {
	if (abs(command - _curr_command < _IncperRev * 2)) {
		_curr_command = command;
		if (_mot_locked == false) {
			//_DATAport->write(0x23);
			//_DATAport->write((uint8_t*)&variation, sizeof(variation));
			//_DATAport->println();
			AccelStepper::moveTo(_curr_command);
      return 1;
		}
   return 0;
	}
 return 0;
	AltLED();
}

//void MORJStepperDriver::AddKeyValue_JSON(String key, String value) {
//    _buffer_stepDATA[key] = value;
//}

//void MORJStepperDriver::Send_JSON() {
//    serializeJson(_buffer_stepDATA, *_DATAport);
//    _DATAport->println();
//    _buffer_stepDATA.clear();
//}

void MORJStepperDriver::connectLED(int Pin_LED) {
	_Pin_LED = Pin_LED;
	pinMode(_Pin_LED, OUTPUT);
}

void MORJStepperDriver::EnableSpread(bool spread) {
	TMC2209Stepper::en_spreadCycle(spread);
}

bool MORJStepperDriver::EvaluateMovement() {
	if (_mot_locked) {
		if (_initlock) {
			if (AccelStepper::isRunning()) {
				AccelStepper::run();
			}
			else {
				TMC2209Stepper::en_spreadCycle(false);
				_initlock = 0;
        return true; //SEND A MESSAGE TO SAY THAT THE MOTOR FINISHED LOCKING
			}
		}
	}
	else {
		AccelStepper::run();
	}
  return false;
}

char* MORJStepperDriver::GetMotorScale(char* result) {
	dtostrf(_IncperRev, 12, 0, result);
	return result;
	//AddKeyValue_JSON(F("m_scale"), String(_IncperRev)); //SEND A MESSAGE TO SAY THAT THE MOTOR FINISHED LOCKING
	//Send_JSON();
}

void MORJStepperDriver::CalculateIncperRev() {
	_IncperRev = (int)_GearRed * _StperRev * _microsteps;
	//_DATAport->println(_IncperRev);
}

void MORJStepperDriver::WriteHEX_position(long data) { // 23(HEX) or # or 35(DEC): start byte , and then length of 4 bytes
	return;
	//_DATAport->write(0x23);
	//_DATAport->write((uint8_t*)&data, sizeof(data));
}

//TMC2209Stepper::microsteps(scale);
//TMC2209Stepper::en_spreadCycle(_mode);
//AccelStepper::setMaxSpeed(_maxspeed);
//AccelStepper::setAcceleration(_accel_rate);
void MORJStepperDriver::AltLED() {
	if (_led_active) {
		static bool led = 1;
		if (led == 0) {
			led = 1;
			analogWrite(_Pin_LED, 255);
		}
		else {
			led = 0;
			analogWrite(_Pin_LED, 20);
		}
	}
}
