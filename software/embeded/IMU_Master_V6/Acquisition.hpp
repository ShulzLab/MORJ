#pragma once
#include "Modulex.hpp"
#include "IMU_DMP.hpp"

class Acquisition_selection {
public : 
	bool quaternions = true;
	bool euler_quat = false;

	bool acceleration = false;
	bool gyroscope = false;
	bool magnetometer = false;

	bool yaw_quat = true;
	bool yaw_coupling = false;
	bool summed_yaw_coupling = true;

	bool command = true;
	bool summed_command = true;
	bool turns = true;
	
	bool is_empty() {
		if (quaternions || euler_quat || acceleration || gyroscope || magnetometer || yaw_quat || yaw_coupling || summed_yaw_coupling || command || summed_command || turns) {
			return true;
		}
		return false;
	}
  
	void answer_json_request(JsonObject json_request) {
		if (json_request.containsKey("quat")) {
			quaternions = (bool)json_request["quat"];
		}
		if (json_request.containsKey("accel")) {
			acceleration = (bool)json_request["accel"];
		}
		if (json_request.containsKey("gyro")) {
			gyroscope = (bool)json_request["gyro"];
		}
		if (json_request.containsKey("mag")) {
			magnetometer = (bool)json_request["mag"];
		}
		if (json_request.containsKey("yaw_quat")) {//yaw obtained from quaternion processing
			yaw_quat = (bool)json_request["yaw_quat"];
		}
		if (json_request.containsKey("summed_yaw_coupling")) {//yaw obtained after offseting for coupling
			summed_yaw_coupling = (bool)json_request["summed_yaw_coupling"];
		}
   if (json_request.containsKey("yaw_coupling")) {//yaw obtained after offseting for coupling
     yaw_coupling = (bool)json_request["yaw_coupling"];
    }
		if (json_request.containsKey("command")) {//last motor command
			command = (bool)json_request["command"];
		}
		if (json_request.containsKey("summed_command")) {//last summed motor command
			summed_command = (bool)json_request["summed_command"];
		}
		if (json_request.containsKey("turns")) {
			turns = (bool)json_request["turns"];
		}
		if (json_request.containsKey("euler_quat")) {//all yaw, pitch and roll, from quaternion processing
			euler_quat = (bool)json_request["euler_quat"];
		}
    }

	void show_current_selection(JsonDocument& l_output_doc) {
		if (quaternions) {
			l_output_doc["quat"] = true;
		}
		if (acceleration) {
			l_output_doc["accel"] = true;
		}
		if (gyroscope) {
			l_output_doc["gyro"] = true;
		}
		if (magnetometer) {
			l_output_doc["mag"] = true;
		}
		if (yaw_coupling) {//yaw obtained after offseting for coupling
			l_output_doc["yaw_coupling"] = true;
		}
		if (command) {//last motor command
			l_output_doc["command"] = true;
		}
		if (summed_command) {//last summed motor command
			l_output_doc["summed_command"] = true;
		}
		if (turns) {
			l_output_doc["turns"] = true;
		}
		if (euler_quat) {//all yaw, pitch and roll, from quaternion processing
			l_output_doc["euler_quat"] = true;
		}
		else if (yaw_quat) {//yaw obtained from quaternion processing
			l_output_doc["yaw_quat"] = true;
		}
	}
};


class Acquisitions : public Modulex {
public:

	enum e_record_status : uint8_t {
		stopped = 0,
		starting = 2,
		stopping = 3,
		started = 1
	};

	e_record_status session_status = stopped;
	e_record_status trial_status = stopped;

	uint32_t session_time_memo;
	uint16_t trial_number = 0;

	IMU_DMP* IMU_object;
	IMU_Coupling* Coupling_object;

	Quaternion quat_reference;

	JsonDocument* output_document;

	Acquisition_selection data_selection;

	Acquisitions(char* l_name, IMU_DMP* l_IMU_object, IMU_Coupling* l_Command_object);
	Acquisitions(char* l_name);

	void set_output_document(JsonDocument* l_output_document);

	void aggregate_due_data();
	void append_data_selections(JsonObject& data_object);
  
	void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
	void show_help(JsonDocument& outputDoc);

	void start_session();
	void end_session();

	void start_trial(uint32_t trial_start_time);
	void start_trial();
	void end_trial();

	Waiter emission_timer;
	Waiter trial_duration_timer = Waiter(1500,Waiter::millisec);//default, trial duration = 1500 millisec
};

Acquisitions::Acquisitions(char* l_name, IMU_DMP* l_IMU_object, IMU_Coupling* l_Command_object) : Modulex(l_name) {
	quat_reference = Quaternion();
	IMU_object = l_IMU_object;
	Coupling_object = l_Command_object;
	emission_timer = Waiter(50, Waiter::millisec);
	emission_timer.wait();//by default, we start acquisition right away at program start
}

Acquisitions::Acquisitions(char* l_name) : Modulex(l_name) {
}

void Acquisitions::set_output_document(JsonDocument* l_output_document) {
	output_document = l_output_document;
}

void Acquisitions::start_session() {
	session_status = starting;
	session_time_memo = millis();
	trial_number = 0;
}

void Acquisitions::end_session() {
	session_status = stopping;
}

void Acquisitions::start_trial(uint32_t trial_start_time) {//trial_start_time in millisec.
	trial_status = starting;
	emission_timer.elapse_now();
	trial_duration_timer.wait();
	trial_duration_timer.memory = trial_start_time;//set retrospectively the start of the wait period to trial_start_time instead of the time we called wait, just before.
}

void Acquisitions::start_trial() {//start trial with time = now
	start_trial(millis());
}

void Acquisitions::end_trial() {
	trial_status = stopping;
	trial_number = trial_number + 1;
	trial_duration_timer.stop();
}

void Acquisitions::aggregate_due_data() {

	if (status && emission_timer.elapsed() ){//&& !data_selection.is_empty()) {
		if (!IMU_object->available) {
			//do nomething if no imu data is available.
		}
		JsonObject data_object = output_document->createNestedObject("d");// d stores all "data" fields. Non nested infos are either metadata or live state indicators
		if (session_status == starting) {
			data_object["sstart"] = true;
			session_status = started;
		}
		if (trial_status == starting) {
			data_object["tstart"] = trial_number;
			trial_status = started;
		}
		if (trial_status == started || trial_status == stopping){
			data_object["tnb"] = trial_number;
			data_object["tt"] = trial_duration_timer.get_elapsed_time();//time relative to trial start (trial time : tt)
		}
		if (trial_status == stopping) {
			data_object["tstop"] = trial_number;
			trial_status = stopped;
		}
		if (session_status == started || session_status == stopping) {
			data_object["t"] = millis() - session_time_memo;//time relative to session start
		}
		if (session_status == stopping) {
			data_object["sstop"] = true;
			session_status = stopped;
		}
		append_data_selections(data_object);
		emission_timer.wait();
	}
}

void Acquisitions::append_data_selections(JsonObject& data_object) {
	if (data_selection.quaternions) {
		IMU_object->quaternion_data.into_json_object("q", (data_object));
	}

	if (data_selection.euler_quat) {
		data_object["y"] = IMU_object->quaternion_data.yaw();
		data_object["p"] = IMU_object->quaternion_data.pitch();
		data_object["r"] = IMU_object->quaternion_data.roll();
	}
	else if (data_selection.yaw_quat) {
		data_object["y"] = IMU_object->quaternion_data.yaw();
	}

	if (data_selection.summed_yaw_coupling) {
		data_object["ys"] = Coupling_object->get_summed_position();
	}
  
	if (data_selection.acceleration) {
		//todo data_object["a"] = 
	}
	if (data_selection.gyroscope) {
		//todo data_object["g"] = 
	}
	if (data_selection.magnetometer) {
		//tododata_object["m"] = 
	}

	if (data_selection.yaw_coupling) {
		data_object["cy"] = Coupling_object->get_position();
	}

	if (Coupling_object->new_command_flag){
		if (data_selection.command) {
			data_object["c"] = Coupling_object->last_command;
		}
		if (data_selection.summed_command) {
			data_object["lsc"] = Coupling_object->last_summed_command;
			data_object["csc"] = Coupling_object->get_summed_command();
		}
		Coupling_object->new_command_flag = false;
	}

	if (data_selection.turns) {
		data_object["tu"] = Coupling_object->turns;
	}
}

void Acquisitions::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey("set_select")) {
		data_selection.answer_json_request(json_request["set_select"]);
	}
	if (json_request.containsKey("get_select")) {
		data_selection.show_current_selection(outputDoc);
	}
	if (json_request.containsKey("start")) {
		emission_timer.elapse_now();
	}
	if (json_request.containsKey("stop")) {
		emission_timer.stop();
	}
	if (json_request.containsKey("set_interval")) {
		emission_timer.interval = (uint32_t)json_request["set_interval"];
	}
	if (json_request.containsKey("set_trial_duration")) {
		trial_duration_timer.interval = (uint32_t)json_request["set_trial_duration"];
	}
	if (json_request.containsKey("set_quat_ref")) {
		quat_reference.take_value_of(&IMU_object->quaternion_data);
	}
	if (json_request.containsKey("get_quat_ref")) {
		quat_reference.into_json_document("quat_ref", outputDoc);
	}
	if (json_request.containsKey("info")) {
		Acquisitions::show_help(outputDoc);
	}
	Modulex::answer_json_request(json_request, outputDoc);
}

void Acquisitions::show_help(JsonDocument& outputDoc) {
	JsonObject info_object;
	if (outputDoc.containsKey("info")) {
		info_object = outputDoc["info"];
	}
	else {
		info_object = outputDoc.createNestedObject("info");
	}
	info_object["set_select"] = "select the data that will be sent";
	info_object["get_select"] = "see wich data is selected";
	info_object["start"] = "start acquisition timer";
	info_object["stop"] = "stop acquisition timer";
	info_object["set_interval"] = "set acquisition timer interval (in millis)";
	info_object["set_trial_duration"] = "set expected trial duration (in millis)";
	info_object["set_quat_ref"] = "set and store 3D orientation reference from current position";
	info_object["get_quat_ref"] = "get 3D orientation reference";
}
