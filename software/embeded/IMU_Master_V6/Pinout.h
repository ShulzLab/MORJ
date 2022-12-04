#pragma once

//PINS DECLARATION


//LEDS FOR FRONT DISPLAY
const int LEDB =6; 
const int LEDR =7;
const int LEDG =5;


const int Plus = 33;
const int Minus = 37;
const int SetupPin = 35;
const int Led = 3;

const int LockBNC = A9; // APPEAR AS LOCK ON MORJ FACE
const int SessionBNC = A10; // APPEAR AS SHUT.IN ON MORJ FACE  //// UNUSED
const int SutterOUTPUT_BNC = 31;//A8; // APPEAR AS STROB.IN ON MORJ FACE
const int SequenceBNC = 2; // APPEAR AS SHUT.OUT ON MORJ FACE

//DAQ GLOBAL VARS 

bool DAQ_real_mpos = 0;
bool DAQ_3ax = 0;
bool DAQ_9ax = 0;
bool DAQ_raw = 0;
bool DAQ_info = 1;

////WORK VARIABLES

bool IMU_connection_status = 0;

unsigned long SEQSTARTTIME = 0;
unsigned long SESSIONSTARTTIME = 0;
unsigned long SEQENDTIME = 0;

long LASTCOMMAND_MSG = 0;

bool SequenceStartFlag = 0;

bool Locked_state = 0;

long TRIALS = 0;

void setupPins() {
	pinMode(LEDG, OUTPUT);
	pinMode(LEDR, OUTPUT);
	pinMode(LEDB, OUTPUT);
	digitalWrite(LEDG , LOW);
	digitalWrite(LEDR , LOW);
	digitalWrite(LEDB , LOW);
      
	pinMode(Plus, INPUT_PULLUP);
	pinMode(SetupPin, INPUT_PULLUP);
	pinMode(Minus, INPUT_PULLUP);
	pinMode(Led, OUTPUT);

	//pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LockBNC, INPUT_PULLUP);// POTENTIALLY DISCONNECTED INPUTS SET WITH PULLUPS TO AVOID NOISE JITTER ISSUES WHILE DISCONNECTED
	pinMode(SequenceBNC, INPUT_PULLUP);
	pinMode(SutterOUTPUT_BNC, OUTPUT);
	pinMode(SessionBNC, INPUT_PULLUP);
  
	digitalWrite(SutterOUTPUT_BNC , LOW);
}
