#pragma once
#include "Modulex.hpp"

class InterruptInput : public Modulex {
public:

	InterruptInput(const char* l_name,uint8_t interrupt_pin, uint8_t l_pinmode, uint8_t transition);

	uint8_t _object_id;

	void attach_interrupt(uint8_t interrupt_pin, uint8_t transition);
	uint8_t get_interrupt_pin() { return _interrupt_pins[_object_id]; } ;
	uint32_t get_activation_time() { return _activation_time[_object_id]; };

	virtual bool is_activated();

	void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
	void show_help(JsonDocument& outputDoc);

protected://up to 5 interrupts with current implementation

	//static functions, hence, that can be attached to interrupts. 
	//They internally can access ONLY static members of the class (and global variables)
	static void ISR_0();
	static void ISR_1();
	static void ISR_2();
	static void ISR_3();
	static void ISR_4();

	//static members, accessible and identical for all instances. They are used inside the above static functions.
	static uint8_t _object_count;
	
	static uint8_t _interrupt_pins[5];
	static bool _activation_flags[5]; 
	static uint32_t _activation_time[5];
};

//initialization of the values of the static members :
uint8_t InterruptInput::_object_count = 0;

uint8_t InterruptInput::_interrupt_pins[5] = {0};
bool InterruptInput::_activation_flags[5] = { false };
uint32_t InterruptInput::_activation_time[5] = { 0 };
//

InterruptInput::InterruptInput(const char* l_name, uint8_t interrupt_pin, uint8_t l_pinmode, uint8_t transition) : Modulex(l_name) {
	_object_id = _object_count;
	_object_count++ ;
  pinMode(interrupt_pin, l_pinmode);
	attach_interrupt(interrupt_pin, transition);
}

bool InterruptInput::is_activated() {
	if (_activation_flags[_object_id]) {
		_activation_flags[_object_id] = false;
		_activation_time[_object_id] = micros();
		return true;
	}
	return false;
}

void InterruptInput::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey("trigger")) {
		_activation_flags[_object_id] = true;
	}
	if (json_request.containsKey("info")) {
		InterruptInput::show_help(outputDoc);
	}
}

void InterruptInput::show_help(JsonDocument& outputDoc) {
	JsonObject info_object;
	if (outputDoc.containsKey("info")) {
		info_object = outputDoc["info"];
	}
	else {
		info_object = outputDoc.createNestedObject("info");
	}
	info_object["trigger_rising"] = "Force the object to respond to a rising signal event";
	info_object["trigger_falling"] = "Force the object to respond to a falling signal event";
}

void InterruptInput::attach_interrupt(uint8_t interrupt_pin, uint8_t transition) {
	switch (_object_id) {
		case 0 :
			attachInterrupt(digitalPinToInterrupt(interrupt_pin), ISR_0, transition);
			_interrupt_pins[0] = interrupt_pin;
			break;
		case 1:
			attachInterrupt(digitalPinToInterrupt(interrupt_pin), ISR_1, transition);
			_interrupt_pins[1] = interrupt_pin;
			break;
		case 2:
			attachInterrupt(digitalPinToInterrupt(interrupt_pin), ISR_2, transition);
			_interrupt_pins[2] = interrupt_pin;
			break;
		case 3:
			attachInterrupt(digitalPinToInterrupt(interrupt_pin), ISR_3, transition);
			_interrupt_pins[3] = interrupt_pin;
			break;
		case 4:
			attachInterrupt(digitalPinToInterrupt(interrupt_pin), ISR_4, transition);
			_interrupt_pins[4] = interrupt_pin;
			break;
	}
}

void InterruptInput::ISR_0() {
	_activation_flags[0] = true;
	_activation_time[0] = micros();
}

void InterruptInput::ISR_1() {
	_activation_flags[1] = true;
	_activation_time[1] = micros();
}

void InterruptInput::ISR_2() {
	_activation_flags[2] = true;
	_activation_time[2] = micros();
}

void InterruptInput::ISR_3() {
	_activation_flags[3] = true;
	_activation_time[3] = micros();
}

void InterruptInput::ISR_4() {
	_activation_flags[4] = true;
	_activation_time[4] = micros();
}

class RefractoryInterruptInput : public InterruptInput {
public:

	RefractoryInterruptInput(const char* l_name, uint8_t interrupt_pin, uint8_t l_pinmode, uint8_t transition, uint32_t l_refractory_interval);

	bool is_activated();
	uint32_t get_activation_time();
	bool _is_in_refractory_period = false;
	Waiter refractory_waiter;

	uint32_t activation_time = 0;
};

RefractoryInterruptInput::RefractoryInterruptInput(const char* l_name, uint8_t interrupt_pin, uint8_t l_pinmode, uint8_t transition, uint32_t l_refractory_interval) : InterruptInput(l_name, interrupt_pin, l_pinmode, transition) {
	refractory_waiter = Waiter(l_refractory_interval, Waiter::microsec);
}


bool RefractoryInterruptInput::is_activated() {
	if (InterruptInput::is_activated()) {
		if (_is_in_refractory_period) {
			//do nothing
		}
		else {
			activation_time = InterruptInput::get_activation_time();
			_is_in_refractory_period = true;
			refractory_waiter.wait();
			return true;
		}
	}
	if (_is_in_refractory_period) {
		if (refractory_waiter.elapsed()) {
			_is_in_refractory_period = false;
		}
	}
	return false;
}

uint32_t RefractoryInterruptInput::get_activation_time() {
	return activation_time;
}

//TODO need to finish this alternative class in case it is usefull later
class PartialInterruptInput : public InterruptInput {
public:
	bool is_activated();
};

bool PartialInterruptInput::is_activated() {

}


class PollingInput : public Modulex{
public :

	enum e_detection : uint8_t {
		low = 0,
		high = 1,
		falled = 3,
		rised = 4,
		idle = 5,
	};

	PollingInput(const char* l_name, uint8_t l_pin, uint8_t l_pinmode);
	uint8_t pin;
	e_detection signal;
	e_detection virtual_signal = idle;

	void poll();

	bool rising();
	bool falling();
	bool is_high();
	bool is_low();

	//void connect(bool (PollingInput::*detection_func)(), void (*action_func)(Modulex*), Modulex* l_connected_module);

	bool memory_state = 0;

	void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
	void show_help(JsonDocument& outputDoc);
};

PollingInput::PollingInput(const char* l_name, uint8_t l_pin, uint8_t l_pinmode) : Modulex(l_name) {
	pin = l_pin;
	pinMode(pin, l_pinmode);
}

void PollingInput::poll() {//internally called by rising falling and is_high is_low.
	if (digitalRead(pin)) {//if pin HIGH 
		if (!memory_state) {//and memory state was LOW
			signal = rised;
		}
		else {
			signal = high;
		}
		memory_state = HIGH;
	}
	else {//if pin LOW 
		if (memory_state) {//and memory state was HIGH
			signal = falled;
		}
		else {
			signal = low;
		}
		memory_state = LOW;
	}
}

bool PollingInput::rising() {
	if (signal == rised || virtual_signal == rised) {
		virtual_signal = idle;
		return true;
	}
	return false;
}

bool PollingInput::falling() {
	if (signal == falled || virtual_signal == falled) {
		virtual_signal = idle;
		return true;
	}
	return false;
}

bool PollingInput::is_high() {
	if (signal == rised || signal == high) {
		return true;
	}
	return false;
}

bool PollingInput::is_low() {
	if (signal == falled || signal == low) {
		signal = idle;
		return true;
	}
	return false;
}

void PollingInput::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey("trigger_rising")) {
		virtual_signal = rised;
	}
	if (json_request.containsKey("trigger_falling")) {
		virtual_signal = falled;
	}
	if (json_request.containsKey("info")) {
		PollingInput::show_help(outputDoc);
	}
}

void PollingInput::show_help(JsonDocument& outputDoc) {
	JsonObject info_object;
	if (outputDoc.containsKey("info")) {
		info_object = outputDoc["info"];
	}
	else {
		info_object = outputDoc.createNestedObject("info");
	}
	info_object["trigger_rising"] = "Force the object to respond to a rising signal event";
	info_object["trigger_falling"] = "Force the object to respond to a falling signal event";
}
