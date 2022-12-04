#pragma once

class LockInput : public PollingInput {
public:
	
	void lock_update();

	JsonProtocol* com_motor;

	void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);

	LockInput(const char* l_name, uint8_t l_pin, uint8_t l_pinmode, JsonProtocol* l_motor_com);

	//TODO : hold (force permanent lock with software, whatever the BNC state) 
	//TODO : freerun (force permanent unlock with software, whatever the BNC state) 
	//TODO : triggered (let the lock be defined by the bnc state) 
};

LockInput::LockInput(const char* l_name, uint8_t l_pin, uint8_t l_pinmode, JsonProtocol* l_motor_com) : PollingInput(l_name,l_pin, l_pinmode) {
	com_motor = l_motor_com;
}

void LockInput::lock_update() {
	if (rising()) {
		com_motor->send_outputs();
		com_motor->outputDoc["lock"] = 1;
		com_motor->send_outputs();
	}
	if (falling()) {
		com_motor->send_outputs();
		com_motor->outputDoc["lock"] = 0;
		com_motor->send_outputs();
	}
}

void LockInput::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey("set_status")) {
		if ((bool)json_request["set_status"]) {
			com_motor->send_outputs();
			com_motor->outputDoc["lock"] = 1;
			com_motor->send_outputs();
		}
		else {
			com_motor->send_outputs();
			com_motor->outputDoc["lock"] = 0;
			com_motor->send_outputs();
		}
	}
	Modulex::answer_json_request(json_request, outputDoc);
}