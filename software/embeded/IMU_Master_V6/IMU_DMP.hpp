#pragma once
#include "Modulex.hpp"
#include "Quaternions.h"
#include <ICM_20948.h>

class IMU_DMP : public Modulex{
public :
	#define WIRE_PORT Wire //desired I2C port (Wire is default on most MCUs)
	#define AD0_VAL 0 //change to 0 if the ICM adress is changed by external pin wiring.
    // on the sparkfun prototyping board, AD0_VAL needs to be set to 1
    // on the MORJ board it must be set to 0 (adress 0x68)

	IMU_DMP(const char* l_name);//constructor

	//CONNEXION TO THE IMU DMP
	ICM_20948_I2C ICM; //ICM_20948 access object
	void begin();//just setup the clock and the I2C port. must be calle once only in the setup
	void connect();//just connects to the IMU. Blocks up to 2sec so use during setup but not in loop. In loop, use handle_connection_losses to reconnect if necessary
	void handle_connection_losses(JsonDocument& outputDoc);//unused in the main loop. Seemed to do strange things. If you don't see disconnexions, i advise to better left this commented in main loop.
	void setup_DMP();//sets up the DMP. Can take a little moment (~hundreds of millieconds) while the code do nothing else, because we upload quite a bit of data to the dmp.
	
	bool connected = false;//set to true by connect or handle_connection_losses upon a sucessfull connexion throug i2C to imu
	bool dmp_setup = false;//set to true by setup_DMP or handle_connection_losses upon a sucessfull DMP setup. else, is false

	Waiter connection_checker = Waiter(1000, Waiter::millisec);//checks connexion status (if handle_connection_losses called) each 1 sec with this Waiter
	Waiter oldest_message_checker = Waiter(1000, Waiter::millisec);//once status is not ok, this Waiter counts for 1sec before declaring a connexion loss (in case IMU.status was bad only or one iteration)

	//ACCESS TO DATA
	Quaternion quaternion_data = Quaternion();//stores input quaternion data
	void get_data();
	void get_fake_data();//usefull for debugging far from the setup
	void initialize_fake_data();
	bool available = false; //is true if new data is available. consumed and set to false by a call to get_data

	void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
};

IMU_DMP::IMU_DMP(const char* l_name) : Modulex(l_name) {
}

void IMU_DMP::begin(){
	WIRE_PORT.begin();
	WIRE_PORT.setClock(400000);
}

void IMU_DMP::connect(){
	Waiter max_time = Waiter(2000);
	ICM.begin(WIRE_PORT, AD0_VAL);
	max_time.wait();
	while (!max_time.elapsed() && !connected) {//exit when time for connexion try elapsed or connected
		if (ICM.status == ICM_20948_Stat_Ok) {
			connected = true;
		}
	}
	connection_checker.wait();
}

void IMU_DMP::handle_connection_losses(JsonDocument& outputDoc) {
	if (connection_checker.elapsed()) {
		if (ICM.status == ICM_20948_Stat_Ok) {//if status string is fine
			oldest_message_checker.stop();//we stop waiting if we were about to declare a connexion loss
			if (!connected) {
				connected = true;//we are connected though I2C port. Now we try to set DMP.
				setup_DMP();
				if (dmp_setup) {
					outputDoc["IMU_status"] = "reconnected";//it worked.
				}
				else {//it failed. We set variables to retry from the begining soon(opening I2C port)
					ICM.begin(WIRE_PORT, AD0_VAL);
					outputDoc["IMU_status"] = "failed_reconnexion_attempt";
					connected = false;
					dmp_setup = false;
				}
			}
		}
		else if (ICM.status == ICM_20948_Stat_NoData) {//if status string is just saying no data is in store for us, don't try to reconnect
			//do nothing
		}
		else { //else, there must be something 
			outputDoc["IMU_error"] = (char*)ICM.statusString();// ICM.status;
			if (connected || dmp_setup) {//we were previously connected
				oldest_message_checker.wait_impatiently();//we start waiting (and not retriggering this) top see if connexion loss was just momentary and comes back to normal without intervention
				if (oldest_message_checker.elapsed()) {//if we waited long enough without the status being ok, we set variables to try a reconexion sequence
					outputDoc["IMU_status"] = "lost_connexion";
					connected = false;
					dmp_setup = false;
					oldest_message_checker.stop();
				}
			}
			else {// we had already lost connexion, just retry opening I2C port without setting DMP.
				ICM.begin(WIRE_PORT, AD0_VAL);
			}
		}
		connection_checker.wait(500);
	}
}

void IMU_DMP::setup_DMP() {
	if (connected) {
		bool success = true;
		success &= (ICM.status == ICM_20948_Stat_Ok);
		success &= (ICM.initializeDMP() == ICM_20948_Stat_Ok);
		success &= (ICM.enableDMPSensor(INV_ICM20948_SENSOR_ORIENTATION) == ICM_20948_Stat_Ok); // Enable the DMP in 9 axis mode (acc,gyt,mag) sending quaternions.
		success &= (ICM.setDMPODRrate(DMP_ODR_Reg_Quat9, 0) == ICM_20948_Stat_Ok); // Set DMP frequency to the maximum
		success &= (ICM.enableFIFO() == ICM_20948_Stat_Ok); // FIFO buffer necessary for DMP signal transfer
		success &= (ICM.enableDMP() == ICM_20948_Stat_Ok);
		success &= (ICM.resetDMP() == ICM_20948_Stat_Ok);
		success &= (ICM.resetFIFO() == ICM_20948_Stat_Ok);
		dmp_setup = success;
	}
}

void IMU_DMP::get_data() {
	if (dmp_setup) {
		icm_20948_DMP_data_t data;
		ICM.readDMPdataFromFIFO(&data);
		if ((ICM.status == ICM_20948_Stat_Ok) || (ICM.status == ICM_20948_Stat_FIFOMoreDataAvail)) { // Was valid data available?
			quaternion_data.load_measures(&data);
			available = true;
		}
	}
}

void IMU_DMP::get_fake_data() {
	quaternion_data.generate_drift();
	available = true;
}

void IMU_DMP::initialize_fake_data() {
	quaternion_data.generate();
	available = true;
}

void IMU_DMP::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey("get_error")) {
		outputDoc[name]["IMU_error"] = (char*)ICM.statusString();
	}	
	Modulex::answer_json_request(json_request, outputDoc);
}
