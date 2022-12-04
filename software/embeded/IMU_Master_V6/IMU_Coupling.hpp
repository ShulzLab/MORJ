#pragma once

#include <ArduinoJson.h>
#include "Modulex.hpp"

/////////////////////////
///   PositionCycler   ///
/////////////////////////

class PositionCycler : public Modulex {
public : // keeps degrees between a 360� range and keep tracks of turns as well as summed position.
         // keeps track of the offset (for the coupling, not the measure saved for analysis)
         // all outputs use a position between -180 and 180 (0 is the middle) wich is  the convention for the rest of the program and for figures 
         // (0 when going straight and -10 or 10 when going sideways makes sense more sense for the article reader than 0 being straight and 350 or 10 being the same angle sideways but opposite directions.)

  float position = 0; // position angle, always between -180 and +180 deg. 0 is going straight
  float offset =0; //offset for the imu coupling to the actual maze orientation
  int32_t turns = 0; // full 360 turn count (can be positive (CW) or negative (CCW))

  PositionCycler(const char* l_name);//module constructor

  //the function to actually call in the main loop
  void update_coupling_orientation(float angle); //takes as input angles between -180 and +180. (internally converts it into 0 to +360 for simplicity sake) 
  float get_summed_position(); //accumulated position value taking turns into account, in degrees. Unlimited range
  float get_position(); //position corrected by the offset. -180/+180 range

  void set_current_zero();// set offset equal to current degrees angle
  void add_turn(int32_t turn_count) { turns = turns + turn_count; };// add a given number of turns (neg or positive) to the turn count

protected :
  //called by update_orientation
  float constrain_degrees(float deg); //ensures degrees are in the range -180 to +180. Returns angle data in this range
  int32_t detect_turn(float value, float previousvalue); // returns 0, -1 or +1 turns if the position is detected to have rotated a full turn.

  virtual void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
};

PositionCycler::PositionCycler(const char* l_name) : Modulex(l_name) {}

void PositionCycler::update_coupling_orientation(float new_position) {
    float previous_position = get_position();// 
    position = new_position;//new_position is data coming from quaternion to euler conversion, in -180/180 range.
    turns = turns + detect_turn(get_position(), previous_position);
}

float PositionCycler::get_position() {
    return constrain_degrees(position - offset);// We susbtract offset so the range may be wrong. Constrain_degrees is meant to correct this.
}

float PositionCycler::get_summed_position() {
    return (turns * 360) - get_position();
}

int32_t PositionCycler::detect_turn(float angle, float prev_angle) {
    // if a large difference between previous and current angle (200 degrees)
    if (abs(angle - prev_angle) > 200){
      if (angle < 0 && prev_angle > 0) {
          return -1;//CCW turn : -1 turn
      }
      else if (angle > 0 && prev_angle < 0) {
          return 1;//CW turn : +1 turn
      }
    }
    return 0;//else, 0 turn detected
}

float PositionCycler::constrain_degrees(float deg) {
    float l_deg = deg;
    if (l_deg < -180) {
        l_deg = l_deg + 360;
    }
    else if (l_deg > 180) {
        l_deg = l_deg - 360;
    }
    return l_deg;
}

void PositionCycler::set_current_zero() {
    offset = position;
}

void PositionCycler::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
    if (json_request.containsKey("set_offset")) {
        offset = ((float)json_request["set_offset"]);
    }
    if (json_request.containsKey("set_zero")) {
        set_current_zero();
    }
    if (json_request.containsKey("get_offset")) {
        outputDoc[name]["offset"] = offset;
    }
    if (json_request.containsKey("get_pos")) {
        outputDoc[name]["angle"] = get_position();
    }
    if (json_request.containsKey("get_summed_pos")) {
        outputDoc[name]["angle"] = get_summed_position();
    }
    if (json_request.containsKey("add_turn")) {
        int32_t turn_offset = (int32_t)json_request["add_turn"];//cast as int32 
        if (turn_offset > 0){
          add_turn(1);
        }
        else if (turn_offset < 0){
          add_turn(-1);
        }
    }
    if (json_request.containsKey("get_turns")) {
        outputDoc[name]["turns"] = turns;
    }

    Modulex::answer_json_request(json_request, outputDoc);
}

////////////////////////
///   OrientKeeper   ///
////////////////////////

class IMU_Coupling : public PositionCycler {
public :

   int32_t last_summed_command =0; //These are only to send motor commands when they are issued, along with rest of DAQ_info, to the PC for saving data
   int32_t last_command = 0;
   Waiter last_command_memo = Waiter(60, Waiter::millisec);

   bool new_command_flag = false;

   IMU_Coupling(const char* l_name, JsonProtocol* l_com_motor);

   void update_motor();//call this in the main loop after updating the coupling orientation  

//protected :
    JsonProtocol* com_motor;

    //parameters relative to the mechanical design of the morj
    float scaler = 80.0 / 3.0;//ratio of angle to steps to keep a stable basis with the motor. (for reference, with 80/3 scaler, 9600 steps represent 360 degrees)

    //parameter relative to the tuning of the motor movements
    int32_t move_above_step_count = 600;//number of steps necessary to issue a new command update

    //called by update_motor : 
    int32_t compute_motor_command();
    int32_t get_summed_command();

    void reset_coupling();//an override of the parent method, to reset commands when we set the offset (to avoid any issues) 

    void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
};

IMU_Coupling::IMU_Coupling(const char* l_name, JsonProtocol* l_com_motor) : PositionCycler(l_name) {
    com_motor = l_com_motor;
    last_command_memo.wait();
}

void IMU_Coupling::update_motor() {

    int32_t command = compute_motor_command();//calculates the command to send
    if (command != 0) {
        new_command_flag = true; // a mean to report a new command has been calculated and sent.
        com_motor->outputDoc["go"] = command;
    }
}

int32_t IMU_Coupling::get_summed_command() {
    return (int32_t) (scaler * get_summed_position());
    
}

int32_t IMU_Coupling::compute_motor_command() {
    int32_t current_summed_command = get_summed_command();
    if (status == activated) {//if status deactivaed : means IMU and motor uncoupled. The orientation is still calculated, bu the command is not updated.
        if (abs(current_summed_command - last_summed_command) > move_above_step_count && last_command_memo.elapsed()) {// check the diff between last summed command and current summed command > step count sensiotivity and time elapsed > 60ms (Waiter value)
            last_command = current_summed_command - last_summed_command; // the difference between last summed and current summed. We send  this to the motor later. 
            //constrain output command to one turn (+/-9600) to avoid erroneous super large movements. (more than one turn in one command)
            //It's better than destroying the setup, during putative very rare bugs.
            
            //The summed command is just for us to not accumulate error when converting degrees (float) to motor steps (int) because the scaler is not an integer but a rational number (fraction of two ints)
            last_summed_command = current_summed_command; //save new current summed command
            last_command_memo.wait(); //update timer
            return constrain(last_command, -9600, 9600);; //returns the command
        }
    }
    //else:
    return 0;
}

void IMU_Coupling::reset_coupling(){
    turns = 0;
    offset = 0;
    new_command_flag = false;
    last_summed_command = 0;
    last_command = 0;
}

void IMU_Coupling::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
    if (json_request.containsKey("set_sensitivity")) {
        move_above_step_count = (int32_t)json_request["set_sensitivity"];
    }
    if (json_request.containsKey("get_last_cmd")) {
        outputDoc[name]["last_cmd"] = last_command;
    }
    if (json_request.containsKey("get_last_scmd")) {
        outputDoc[name]["last_scmd"] = last_summed_command;
    }
    if (json_request.containsKey("reset")) {//reset everything : turns, offsets, commands. 
        reset_coupling();
    }
    //to set the coupling as used with the keyword "coupling" before, call set_status 1 or 0. For more details on how it works, see in compute_motor_command at "status==active".

    PositionCycler::answer_json_request(json_request, outputDoc);
}
