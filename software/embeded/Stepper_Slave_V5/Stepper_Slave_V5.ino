#include <stdint.h>

#define PROTOCOL_NO_WAIT_JSON_AKNOWLEDGE 1
#define ARDUINOJSON_DECODE_UNICODE 1
#define PROTOCOL_ITEM_NUMBER 6
#define PROTOCOL_DEBUG_MODE 0


#include "ArduPPPJS.h"
//#include <avr/wdt.h>

#include "DriverClass.h"

#define INTERNAL_TMC_UART Serial5  // UART serial port used to communicate with TMC driver
#define EXTERNAL_DATA_UART Serial4
#define EXTERNAL_COMPUTER_UART Serial

#define INITIALIZATION_WAIT_PERIOD 6000

const int Pin_GreenLED = 12;
const int Pin_RedLED = 11;

constexpr auto glb_Pin_MotorEnable = 7;
constexpr auto glb_Pin_MS1 = 5;
constexpr auto glb_Pin_MS2 = 6;

constexpr auto glb_Pin_Step = 3;
constexpr auto glb_Pin_Dir = 2;

constexpr auto DRIVER_ADDRESS = 0b00;

constexpr float R_SENSE = 0.11;
//#define R_SENSE 0.11f // Match to your driver

int RDOfreq = 100;
bool RDOflag = false;

IntervalTimer RDO_timer;

MORJStepperDriver StepMotor(INTERNAL_TMC_UART, R_SENSE, DRIVER_ADDRESS, glb_Pin_Step, glb_Pin_Dir, glb_Pin_MS1, glb_Pin_MS2);

BJ_ProtocolHandler<PROTOCOL_ITEM_NUMBER> IOs;



#define com_type int32_t
#define com_id 10
#define mpos_type int32_t
#define mpos_id 20
#define abscom_type int32_t
#define abscom_id 30
//#define virtual_type float
//#define virtual_id 230
//#define virtual_type_2 uint64_t
//#define virtual_id_2 158

void PayloadSetup() {
	IOs.append<com_type>(com_id, "command");
	IOs.append<mpos_type>(mpos_id, "motor_pos");
	IOs.append<abscom_type>(abscom_id, "abs_motor_command");
	//IOs.append<virtual_type>(virtual_id, "virtual_float");
	//IOs.append<virtual_type_2>(virtual_id_2, "virtual_uint64");
}

void setup() {
	PayloadSetup();

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(Pin_GreenLED, OUTPUT);
	pinMode(Pin_RedLED, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	static bool COMPUTER_CONNECTED = false;
	EXTERNAL_COMPUTER_UART.begin(250000);
	while (millis() < INITIALIZATION_WAIT_PERIOD) {
		Wait_and_BlinkLEDs();
		if (EXTERNAL_COMPUTER_UART) {
			COMPUTER_CONNECTED = true;
			break;
		}
	}

	// put your setup code here, to run once:
	INTERNAL_TMC_UART.begin(115200);

	if (COMPUTER_CONNECTED) {
		IOs.begin(EXTERNAL_COMPUTER_UART, EXTERNAL_COMPUTER_UART);
		StepMotor.begin(3, 400, 8, glb_Pin_MotorEnable, EXTERNAL_COMPUTER_UART);
		StepMotor.connectLED(Pin_RedLED);
		StepMotor.SetLedMode(true);
		digitalWrite(Pin_GreenLED, LOW);
		EXTERNAL_COMPUTER_UART.println("{started_in_mode:'computer_usb'}");
	}
	else {
		EXTERNAL_DATA_UART.begin(115200);
		delay(250);
		IOs.begin(EXTERNAL_DATA_UART, EXTERNAL_COMPUTER_UART);
		StepMotor.begin(3, 400, 8, glb_Pin_MotorEnable, EXTERNAL_DATA_UART);
		StepMotor.connectLED(Pin_GreenLED);
		StepMotor.SetLedMode(false);
		digitalWrite(Pin_RedLED, LOW);
		EXTERNAL_COMPUTER_UART.println("{started_in_mode:'external_device'}");
	}

	RDO_timer.priority(129);
	RDO_timer.begin(RDO_IRQ, (long)(1000000 / RDOfreq));

	//StepMotor.toff(5);

	StepMotor.SetMotorCurrent(1600);

	StepMotor.AccelStepper::setMaxSpeed(10000);
	StepMotor.SetMotorAcceleration(20000);
	EXTERNAL_COMPUTER_UART.println("{setup_init:'finished'}");
};

void loop() {
	MessageHandlers();

	bool _temp_lock = StepMotor.EvaluateMovement();
  if (_temp_lock){
    IOs.AddKeyValue_JSON("locked", true);
    IOs.Send_JSON();
  }
	AltLEDS_builtin();
};

void MessageHandlers() {

	IOs.CommsHandler();
	if (IOs.jsonAvailable) {
		JsonInterpreter();
	}
	if (IOs[mpos_id].available == 1) {
		char result[16];
		IOs[mpos_id].getValue<mpos_type>();
		dtostrf(StepMotor.currentPosition(), 12, 0, result);
		IOs.AddKeyValue_JSON("motor_current_pos", result);
		IOs.Send_JSON();
	}
	if (IOs[com_id].available == 1) {
		/*char result[16];
		dtostrf(IOs[com_id].getValue<com_type>(), 12, 0, result);
		IOs.AddKeyValue_JSON("motor_command_recieved", result);
		IOs.Send_JSON();*/
		StepMotor.NewCommand(IOs[com_id].getValue<com_type>());
	}
	if (IOs[abscom_id].available == 1) {
	  bool statux = StepMotor.AbsCommand(IOs[com_id].getValue<com_type>());
    if (statux){
      IOs.AddKeyValue_JSON("abs_exec",true);
      IOs.Send_JSON();
    }
    else {
      IOs.AddKeyValue_JSON("abs_exec",false);
      IOs.Send_JSON();
    }
	}

	//if (IOs[virtual_id].available == 1) {
	//	char result[16];
	//	dtostrf(IOs[virtual_id].getValue<virtual_type>(), 12 , 5 ,result);
	//	IOs.AddKeyValue_JSON("printed_var_float", result);
	//	IOs.Send_JSON();
	//}
	//if (IOs[virtual_id_2].available == 1) {
	//	char result[16];
	//	dtostrf(IOs[virtual_id_2].getValue<virtual_type_2>(), 12, 0, result);
	//	IOs.AddKeyValue_JSON("printed_var_int64", result);
	//	IOs.Send_JSON();
	//}
};


void RDO_IRQ() {
	if (StepMotor._RDOfreq != RDOfreq) {
		RDO_timer.update((long)(1000000 / RDOfreq));
		RDOfreq = StepMotor._RDOfreq;
	}
	if (StepMotor._RDO_status == true) {
		RDOflag = true;
	}
};

void RDO_main_handler(){
	if (RDOflag){
		mpos_type data = StepMotor.currentPosition();
		IOs[mpos_id].setValue(data);
		IOs.SendHex(mpos_id);
		RDOflag = false;
	}
}

void Wait_and_BlinkLEDs() {
	static unsigned long _memo = 0;
	static bool _status = 0;
	static unsigned int _delay = 100;
	if (_delay == 100 && millis() > INITIALIZATION_WAIT_PERIOD - (INITIALIZATION_WAIT_PERIOD/4)) {
		_delay = 50;
	}
	if (millis() - _memo > _delay) {
		digitalWrite(Pin_GreenLED, _status);
		digitalWrite(Pin_RedLED, _status);
		_status = !_status;
		_memo = millis();
	}
};

void AltLEDS_builtin() {
	static bool led = 0;
	static unsigned long lasttime = 0;
	if (StepMotor._led_active) {
		if (millis() - lasttime > 50) {
			lasttime = millis();
			if (led == 0) {
				led = 1;
				digitalWrite(LED_BUILTIN, LOW);
			}
			else {
				led = 0;
				digitalWrite(LED_BUILTIN, HIGH);
			}
		}
	}
};

void JsonInterpreter() {
	bool ValidKey = 0;

	if (PROTOCOL_DEBUG_MODE){
		IOs._DirectSendInput();
	}	

	if (IOs.jsonInput.containsKey("set_led_mode")) {
		StepMotor.SetLedMode((bool)IOs.jsonInput["led_mode"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("handshake")){
		IOs.AddKeyValue_JSON("accept_handshake", "1");
		IOs.Send_JSON();
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("enb_rdout")) {
		if ((bool)IOs.jsonInput["enb_rdout"] == 0) {
			StepMotor.StopRDO();
		}
		else {
			StepMotor.StartRDO((long)IOs.jsonInput["enb_rdout"]);
		}
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("get_pos")) {
		mpos_type data = StepMotor.currentPosition();
		char result[16];
		dtostrf(data, 12, 0, result);
		IOs.AddKeyValue_JSON("current_position", result);
		IOs.Send_JSON();
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("set_motor_scale")) {
		StepMotor.SetMotorPrecision((uint16_t)IOs.jsonInput["set_motor_scale"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("get_inc_per_rev")) {
		char temp[16] = "";
		StepMotor.GetMotorScale(temp);
		IOs.AddKeyValue_JSON("inc_per_rev", temp);
		IOs.Send_JSON();
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("set_motor_max_speed")) {
		StepMotor.setMaxSpeed((long)IOs.jsonInput["set_motor_max_speed"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("set_motor_accel")) {
		StepMotor.SetMotorAcceleration((long)IOs.jsonInput["set_motor_accel"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("set_motor_enable")) {
		StepMotor.SetMotorEnable((bool)IOs.jsonInput["set_motor_enable"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("set_motor_spread_mode")) {
		StepMotor.en_spreadCycle((bool)IOs.jsonInput["set_motor_spread_mode"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("set_motor_current")) {
		StepMotor.SetMotorCurrent((long)IOs.jsonInput["set_motor_current"]);
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("go")) {
		StepMotor.NewCommand((long)IOs.jsonInput["go"]);
		ValidKey = 1;
	}

  if (IOs.jsonInput.containsKey("go_abs")) {
    bool statux = StepMotor.AbsCommand((long)IOs.jsonInput["go_abs"]);
    if (statux){
      IOs.AddKeyValue_JSON("abs_exec",true);
      IOs.Send_JSON();
    }
    else {
      IOs.AddKeyValue_JSON("abs_exec",false);
      IOs.Send_JSON();
    }
    ValidKey = 1;
  } 

	if (IOs.jsonInput.containsKey("zero")) {
		StepMotor.SetZero();
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("stop")) {
		StepMotor.StopMotor();
		ValidKey = 1;
	}

	if (IOs.jsonInput.containsKey("lock")) {
		if ((bool)IOs.jsonInput["lock"] == false) {
			StepMotor.Unlock();
      IOs.AddKeyValue_JSON("locked",false);
      IOs.Send_JSON();
		}
		else if ((bool)IOs.jsonInput["lock"] == true) {
			StepMotor.LockToNearestZero();
		}
		ValidKey = 1;
	}

  if (IOs.jsonInput.containsKey("get_turn_count")) {
   long temp_turns = StepMotor.Turns_Calc();
   IOs.AddKeyValue_JSON("turn_count", temp_turns);
   IOs.Send_JSON();
   ValidKey = 1;
  }

	if (IOs.jsonInput.containsKey("set_spread")) {
		StepMotor.EnableSpread((bool)IOs.jsonInput["set_spread"]);
		ValidKey = 1;
	}

  if (ValidKey == 0) {
    IOs.AddKeyValue_JSON("input_error", "unknown_command");
    IOs.Send_JSON();
  }

//	if (ValidKey == 0) {
//		IOs.Aknowledge_Json(false, "k");//valid key check failed
//	}
//	else {
//		IOs.Aknowledge_Json(true);
//	}
	IOs.jsonAvailable=0;
};
