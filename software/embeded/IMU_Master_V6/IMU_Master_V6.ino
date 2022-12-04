/*
    Name:       IMU_Master_V6.ino
    Created:	20/10/2022 10:59:02
    Author:     XPS139370-1-DS\Timothe
*/
#include "JsonProtocol.hpp"
#include "IMU_Coupling.hpp"
#include "IMU_DMP.hpp"
#include "DigInputs.hpp"
#include "Acquisition.hpp"
#include "ShutterManager.hpp"
#include "LockManager.hpp"
#include "MotorControl.hpp"

//serial handlers
JsonProtocol ComPC = JsonProtocol(&Serial, __FILE__, __DATE__, __TIME__); // __FLAGS__ are Just some info to know which version of the code is compiled on the IMU -For traceability
JsonProtocol ComMotor = JsonProtocol(&Serial1);

//modules for the IMU and motor coupling
IMU_DMP IMU_sensor = IMU_DMP("imu");
IMU_Coupling Coupling_module = IMU_Coupling("coupling", &ComMotor);

//module that only forwards commands to motor
MotorForwarder forwarder_module = MotorForwarder("motor", &ComPC, &ComMotor);

//modules for the data acquisition
PollingInput Session_BNC = PollingInput("bnc_in_2", A10, INPUT_PULLUP);//Acq2_BNC is used for session trigger and is pin A10
RefractoryInterruptInput Sample_BNC = RefractoryInterruptInput("bnc_in_1",2,INPUT_PULLUP,RISING,1300000);//Acq1_BNC in used for precise sampling synchronization and is pin 2. We set the timing between two possible activations to 1.3 sec (in microseconds)
Acquisitions Acquisitions_manager = Acquisitions("records", &IMU_sensor, &Coupling_module);

//modules for the lock and shutter ios
LockInput Lock_BNC = LockInput("lock", A9, INPUT_PULLUP, &ComMotor);//lock in BNC is pin A9
ShutterManager Shutter_BNC = ShutterManager("shutter",31);//shutter out BNC is pin 31

//this tiny function must be defined before setup as we use it in setup. Il also uses global variables (Shutter_BNC etc) so we cannot define it in a class (or it will be too much complicated to make a modular thing for a situationnal need)
void interpret_motor_outputs(JsonObject& input_object){//all inputs from the jsonprotocol object that calls this will be forwarded through input_object
    //The jsonprotocol object that calls this function (defined in setup) is ComMotor. Hence, all inputs from motor are forwared to the PC outputs.
    if (input_object.containsKey("locked")) {
        if ((bool)input_object["locked"]) {
            Shutter_BNC.activate_shutter();
        }
        else {
            Shutter_BNC.deactivate_shutter();
        }
    }
}

void setup()
{
    Serial.begin(250000);
    Serial1.begin(115200);

    ComPC.set_aknowledging_mode(true);

    ComPC.outputDoc["MORJ_Controller"] = "started";
    ComPC.send_outputs();

    //DO STUFF AT RECEPTION OF MESSAGES :
    //MODULES THAT ARE RESPONSIBLE FOR THE MORJ OPERATION
    ComPC.add_listening_module(IMU_sensor);
    ComPC.add_listening_module(Coupling_module);
    ComPC.add_listening_module(Lock_BNC);
    ComPC.add_listening_module(Shutter_BNC);
    ComMotor.add_extenal_listener(&interpret_motor_outputs);// execute custom function on reception of motor outputs like shutter on locked.
    //MODULES THAT ARE RESPONSIBLe for DATA SYNCHRONIZATION AND ACQUISITION
    ComPC.add_listening_module(Acquisitions_manager);
    ComPC.add_listening_module(Sample_BNC);
    ComPC.add_listening_module(Session_BNC);

    //BIDIRECTIONNAL MOTOR ACCESS:
    //(MOTOR -> PC)
    ComMotor.set_copy_on_reception_document(&ComPC.outputDoc);// (MOTOR -> PC) 
    //(PC -> MOTOR)
    ComPC.add_listening_module(forwarder_module);// the module forwarder_module will allow you to write directly to the motor. (PC -> MOTOR)
    //(MORJ -> MOTOR  : COPY TO PC )
    //ComMotor.set_copy_on_emission_document(&ComPC.outputDoc);// usuefull only for debugging. Comment otherwise 
    
    //Set on wich output to send acquisition data and metadata
    Acquisitions_manager.set_output_document(&ComPC.outputDoc);

    delay(200);

    //SETTING UP THE IMU CONNEXION
    IMU_sensor.begin();
    IMU_sensor.connect();
    if (IMU_sensor.connected) {
        IMU_sensor.setup_DMP();
        if (IMU_sensor.dmp_setup) {
            ComPC.outputDoc["IMU_status"] = "connected";
        }
    }
    if (!IMU_sensor.connected || !IMU_sensor.dmp_setup) {
        ComPC.outputDoc["IMU_status"] = "connexion_failed";
    }
    //IMU_sensor.initialize_fake_data();//USEFULL ONLY FOR DEBUGGING. COMMENT OTHERWISE
}

void loop()
{
    ComPC.recieve_inputs();
    ComMotor.recieve_inputs();

//Data extraction

    //IMU_sensor.handle_connection_losses(ComPC.outputDoc); //check if it works by uncommenting this, seemed to do strange things. If you don't see disconnexions, i advise to better left this commented.
    IMU_sensor.get_data();
    //IMU_sensor.get_fake_data();//USEFULL ONLY FOR DEBUGGING. COMMENT OTHERWISE
    
//Coupling live data to the motor

    Coupling_module.update_coupling_orientation(IMU_sensor.quaternion_data.yaw());//updates yaw and turns for command calculation
    Coupling_module.update_motor();//sends command to motor if the movement threshold is passed, and if coupled

//Locking management (shutter management is done upwards, in the function "interpret_motor_outputs")

    Lock_BNC.poll();
    Lock_BNC.lock_update();

//Acquisition management

    Session_BNC.poll();
    if (Session_BNC.rising()) {
        Acquisitions_manager.start_session();
    }
    if (Session_BNC.falling()) {
        Acquisitions_manager.end_session(); 
    }

    if (Sample_BNC.is_activated()) {
       Acquisitions_manager.start_trial((Sample_BNC.get_activation_time()/1000)-1);//start new trial synchonized data, and give the start time of the trial, in milliseconds (hence /1000 because get_activation_time is in micros)
    }
    if (Acquisitions_manager.trial_duration_timer.elapsed()) {
        Acquisitions_manager.end_trial();
    }
    Acquisitions_manager.aggregate_due_data();// send data to pc on a regular basis. Deactivates automatically when we save data based on an external signal (500Hz BNC)

    ComMotor.send_outputs();
    ComPC.send_outputs();
}
