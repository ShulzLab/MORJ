#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Waiter.hpp"

////////////////////////
///     Modulex      ///
////////////////////////

class ModulexKey {
public:
  char* name;
  char* documentation;

  ModulexKey(const char* l_name, const char* l_documentaion);

  void set_char_value(const char* l_name, char* l_permanent_var);

  void get_info(JsonDocument& l_outputdoc);

  template<typename templated_type>
  templated_type get_content(JsonObject json_request);

  JsonObject get_jsonobject(JsonObject json_request);

  bool is_requested(JsonObject json_request);
};


ModulexKey::ModulexKey(const char* l_name, const char* l_documentaion) {
  set_char_value(l_name, name);
  set_char_value(l_documentaion, documentation);
}

void ModulexKey::set_char_value(const char* l_name, char* l_permanent_var) {
  l_permanent_var = new char[strlen(l_name) + 1];
  memcpy(l_permanent_var, l_name, strlen(l_name) + 1);
}

bool ModulexKey::is_requested(JsonObject json_request) {
  if (json_request.containsKey(name)) {
    return true;
  }
  return false;
}

template<typename templated_type>
templated_type ModulexKey::get_content(JsonObject json_request) {
  return (templated_type)json_request[name];
}

JsonObject ModulexKey::get_jsonobject(JsonObject json_request) {
  return json_request[name];
}


void ModulexKey::get_info(JsonDocument& l_outputdoc) {
  JsonObject info_object;
  if (l_outputdoc.containsKey("info")) {
    info_object = l_outputdoc["info"];
  }
  else {
    info_object = l_outputdoc.createNestedObject("info");
  }

  info_object[name] = documentation;
}


class Modulex {
public:
	enum e_status : bool {
		activated = true,
		deactivated = false,
	};

	//properties
	char* name;
	bool status;

	//methods
	void setName(const char* _name);
	void setState(e_status _state);

	virtual void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);
	void check_requested(JsonObject json_request, JsonDocument& outputDoc);

	//contructor
	Modulex(const char* name);

	static ModulexKey rqst_get_status;
  static ModulexKey rqst_set_status;
};

ModulexKey Modulex::rqst_set_status = ModulexKey("set_status","set if the module is activated (true) or deactivated. Results may vary depending on the type of the module.");
ModulexKey Modulex::rqst_get_status = ModulexKey("get_status", "returns if the module is activated (true) or deactivated");

Modulex::Modulex(const char* l_name) {
	setName(l_name);
	status = activated;
}

void Modulex::setState(e_status _state) {
	status = (bool)_state;
}

void Modulex::setName(const char* l_name) {
	name = new char[strlen(l_name) + 1];
	memcpy(name, l_name, strlen(l_name) + 1);
}

void Modulex::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey("set_status")) {
		e_status value = static_cast<e_status>((bool)json_request["set_status"]);
		setState(value);
	}
	if (json_request.containsKey("get_status")) {
		outputDoc[name]["status"] = status;
	}
}

void Modulex::check_requested(JsonObject json_request, JsonDocument& outputDoc) {
	if (json_request.containsKey(name)) {
		answer_json_request(json_request[name], outputDoc);
	}
}
