#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Modulex.hpp"
#include <avr/wdt.h>

#ifndef PROTOCOL_INPUT_LINE_SIZE
#define PROTOCOL_INPUT_LINE_SIZE 140
#endif // !PROTOCOL_INPUT_LINE_SIZE

#ifndef PROTOCOL_OUTPUT_LINE_SIZE
#define PROTOCOL_OUTPUT_LINE_SIZE 1000
#endif // !PROTOCOL_OUTPUT_LINE_SIZE

#ifndef MAX_LISTENING_MODULES_NUMBER
#define MAX_LISTENING_MODULES_NUMBER 0x30 //48 max listening modules. Change if necessary
#endif // !MAX_LISTENING_MODULES_NUMBER

#ifndef SOFTWARE_TIMEOUT
#define SOFTWARE_TIMEOUT 100 //in milliseconds
#endif // !SOFTWARE_TIMEOUT

////////////////////////
///   JsonMessage    ///
////////////////////////

struct JsonMessage {
	enum e_status : uint8_t {
		idle = 0,
		capturing = 1,
		too_long = 2,
		message_ready = 3,
	};

	int8_t layer = 0;//counts the nesting state of the json message to get the end of it
	uint8_t indexer = 0;//counts the number of bytes recieved from start of the line
	e_status status = idle;
	char incomingLine[PROTOCOL_INPUT_LINE_SIZE];
	char* message;

	JsonMessage();
	void addByte(char input_bit);
	char* getMessage();
	void clear();
};

JsonMessage::JsonMessage() {
	clear();
}

void JsonMessage::addByte(char input_bit) {
	if (status == idle && input_bit == 0x7B) {//we aren't capturing anything yet and a { arrives.
		layer = 1;
		status = capturing;
		incomingLine[0] = input_bit;
		indexer = 1;
	}
	else if (status == capturing) {
		if (input_bit == 0x7B) {//{   we go deeper in the nesting
			layer = layer + 1;
		}
		else if (input_bit == 0x7D) {//}  we exit one way out in the nesting
			layer = layer - 1;
		}
		incomingLine[indexer] = input_bit;
		indexer = indexer + 1;
	}
	if (indexer >= PROTOCOL_INPUT_LINE_SIZE) { // was the message too long ?
		status = too_long;
	}
	if (status == capturing && layer <= 0) { // did we reach outermost nesting layer ? Then we show the massage is ready for collection
		status = message_ready;
	}
}

char* JsonMessage::getMessage() {
	delete message;
	message = new char[indexer + 1]; //Here, we convert from char* to const char* because otherwise
	// deserializeJson do zero-copy operation with char* (and not with const char*)
	memcpy(message, incomingLine, indexer);
	message[indexer + 1] = '\0';//add a trailing /0 (null character) is indispenseable in c char arrays to finish a valid string
	return message;
}

void JsonMessage::clear() {
	//for (uint8_t i = 0; i <= indexer; i++) {
	//	incomingLine[i] = 0xFF;
	//}
	memset(incomingLine,0xFF, PROTOCOL_INPUT_LINE_SIZE);
	layer = 0;
	indexer = 0;
	status = idle;
}

////////////////////////
///   JsonProtocol   ///
////////////////////////

class JsonProtocol {

public:
	DynamicJsonDocument inputDoc = DynamicJsonDocument(PROTOCOL_INPUT_LINE_SIZE);//dynamic = stored on the heap
	DynamicJsonDocument outputDoc = DynamicJsonDocument(PROTOCOL_OUTPUT_LINE_SIZE);//dynamic = stored on the heap
	
	Stream* SERIALPORT;//serial port this protocol object is attached to

	JsonMessage incoming_message = JsonMessage();//the object responsible to build a full message from individual bytes.

	bool do_aknowledges = false; // by default, jsonprotocol does not send info about recieved messages.
	//it is usefull mosty (if not only) for communication from embedded device (no graphical interface) to computer (human interface)
	//not usefull between two automated interfaces (between embedded from/to embedded, or from PC to embedded)

	//used for calling back modules. The max number of modules that can be called back is tuneable in the definitions at the top of this file
	Modulex* modules_list[MAX_LISTENING_MODULES_NUMBER];//modules pointers list
	uint8_t modules_nb = 0;//an int counter to keep track of the size of the list that actually contains valueable info

	bool external_callback_set = false;//wether a callback should be called on reception or not
	void (*external_listener_callback)(JsonObject&);//a variable to store a pointer to a function that is of the form : void forwarder_callback(JsonObject&)
	//this forwarded function will be called when a message is recieved (can be usefull to do something external of the class at reception)

	uint32_t software_timeout = SOFTWARE_TIMEOUT;
	uint32_t byte_recied_timememory = 0;

	Waiter soft_timeout = Waiter(SOFTWARE_TIMEOUT);

	bool copy_on_reception = false;// wether a copy should be set on document below on reception 
	JsonDocument* reception_copy_document; // a pointer to the document we want to copy all inputs from this protocol to.

	bool copy_on_emission = false;// wether a copy should be set on document below on emission 
	JsonDocument* emission_copy_document;//a pointer to the document we want to copy all outputs from this protocol to.
  
	char* prog_file;//storage for metadata relative to the program. only defined if the protocol class is initialized with this metadata as input. See contrucor n�2 below
	char* prog_date;//same as above

public:

	//constructor n�1
	JsonProtocol(Stream* l_SerialPort);//create a json protocol handler using the port specified.
	//constructor n�2
	JsonProtocol(Stream* l_SerialPort, const char* l_prog_file, const char* l_prog_name, const char* l_prog_date);//overloading for traceability of code. Ask with "info"

	void set_copy_on_reception_document(JsonDocument* l_copy_output_doc);
	void remove_copy_on_reception();
	void reception_copy(JsonDocument* self_document);

	void set_copy_on_emission_document(JsonDocument* l_copy_output_doc);
	void remove_copy_on_emission();
	void emission_copy(JsonDocument* self_document);
  
	void add_listening_module(Modulex& l_module);//add a module that will be checked by message_interpreter on json message reception with the name of the module as key.
	void show_listening_modules();
	
	void add_extenal_listener(void (*l_func)(JsonObject&));

	void recieve_inputs();//recieve inputs. Internally calls deserialize and then message_interpreter on a valid json message reception.
	void send_outputs();//send and clears outputs.
	void clear_outputs();//clear without sending

	void set_aknowledging_mode(bool l_do_aknowledges);

protected :

	void set_program_data(const char* l_prog_file, const char* l_prog_name, const char* l_prog_date);
	void message_interpreter();//uses a deserialized message. Calls internally check_requested on each attached module.
};

JsonProtocol::JsonProtocol(Stream* l_SerialPort) {
	SERIALPORT = l_SerialPort;
}

JsonProtocol::JsonProtocol(Stream* l_SerialPort, const char* l_prog_file, const char* l_prog_date, const char* l_prog_time) {
	SERIALPORT = l_SerialPort;
	//set_program_data(l_prog_file, l_prog_date, l_prog_time);
}

void JsonProtocol::set_aknowledging_mode(bool l_do_aknowledges) {
	do_aknowledges = l_do_aknowledges;
}

void JsonProtocol::add_extenal_listener(void (*l_func)(JsonObject&)){
  external_listener_callback = l_func;
  external_callback_set = true;
}

void JsonProtocol::set_program_data(const char* l_prog_file, const char* l_prog_date, const char* l_prog_time) {
	prog_file = new char[strlen(l_prog_file) + 1];
	memcpy(prog_file, l_prog_file, strlen(l_prog_file) + 1);

	char* datetime = strcat((char*)l_prog_date, strcat(" ",(char*)l_prog_time));
	prog_date = new char[strlen(datetime) + 1];
	memcpy(prog_date, datetime, strlen(datetime) + 1);	
}

void JsonProtocol::add_listening_module(Modulex& l_module){
	modules_list[modules_nb] = &l_module;
	modules_nb += 1;
}

void JsonProtocol::show_listening_modules() {
	JsonArray l_modules = outputDoc.createNestedArray("modules");
	for (uint8_t i = 0; i < modules_nb; i++) {
		l_modules.add(modules_list[i]->name);
	}
}

void JsonProtocol::message_interpreter() {
	JsonObject documentRoot = inputDoc.as<JsonObject>();
	if (documentRoot.containsKey("info")) {
		show_listening_modules();
		outputDoc["prog_file"] = prog_file;
		outputDoc["prog_date"] = prog_date;
	}
	if (documentRoot.containsKey("handshake")) {
		outputDoc["accept_handshake"] = 1;
	}
	if (documentRoot.containsKey("reboot")) {
		wdt_enable(WDTO_15MS);
		while (1) {}
	}
	for (uint8_t i = 0; i < modules_nb; i++) {
		modules_list[i]->check_requested(documentRoot, outputDoc);
	}
	if (external_callback_set){
		external_listener_callback(documentRoot);
	}
}

void JsonProtocol::recieve_inputs() {
	while (SERIALPORT->available() > 0) {
		incoming_message.addByte(SERIALPORT->read());
		soft_timeout.wait();
		if (incoming_message.status == JsonMessage::message_ready) {
			const char* message = incoming_message.getMessage();
			auto deserialization_error = deserializeJson(inputDoc, message);//deserialize a complete message recieved, into a jsondocument
			if (!deserialization_error) {//use message only if we have no deserialization error
				if (do_aknowledges) {
					outputDoc["ak"]["t"] = "v"; //ack type : valid
				}
				reception_copy(&inputDoc);
				message_interpreter();
			}
			else if (do_aknowledges) {
				outputDoc["ak"]["t"] = "d"; //ack type : deserialization error
				outputDoc["ak"]["e"] = deserialization_error.c_str();
			}
			incoming_message.clear();
			return;
		}
		if (incoming_message.status == JsonMessage::too_long) {
			if (do_aknowledges) {
				outputDoc["ak"]["t"] = "l"; //ack type : length error
			}
			incoming_message.clear();
		}
	}
	if (incoming_message.status == JsonMessage::capturing && soft_timeout()){
		if (do_aknowledges) {
			outputDoc["ak"]["t"] = "t"; //ack type : timeout error
		}
		incoming_message.clear();
	}
}

void JsonProtocol::set_copy_on_emission_document(JsonDocument* l_document_to_copy_on){
	emission_copy_document = l_document_to_copy_on;
	copy_on_emission = true;
}

void JsonProtocol::remove_copy_on_emission() {
	copy_on_emission = false;
}

void JsonProtocol::emission_copy(JsonDocument* self_document) {
	if (copy_on_emission) {
		JsonObjectConst l_outputDoc = self_document->as<JsonObject>();
    if (l_outputDoc.size()==1 && (l_outputDoc.containsKey("ak") || l_outputDoc.containsKey("Ak"))){
      return;
    }
		JsonObject nested_copy = emission_copy_document->createNestedObject("output_copy");
		nested_copy.set(l_outputDoc);
    nested_copy.remove("ak");
    nested_copy.remove("Ak");
	}
}

void JsonProtocol::set_copy_on_reception_document(JsonDocument* l_document_to_copy_on) {
	reception_copy_document = l_document_to_copy_on;
	copy_on_reception = true;
}

void JsonProtocol::remove_copy_on_reception() {
	copy_on_reception = false;
}

void JsonProtocol::reception_copy(JsonDocument* self_document) {
	if (copy_on_reception) {
		JsonObjectConst l_outputDoc = self_document->as<JsonObject>();
    if (l_outputDoc.size()==1 && (l_outputDoc.containsKey("ak") || l_outputDoc.containsKey("Ak"))){
      return;
    }
		JsonObject nested_copy = reception_copy_document->createNestedObject("input_copy");
		nested_copy.set(l_outputDoc);
    nested_copy.remove("ak");
    nested_copy.remove("Ak");
	}
}

void JsonProtocol::send_outputs() {
	if (!outputDoc.isNull()) {
		emission_copy(&outputDoc);
		serializeJson(outputDoc, *SERIALPORT);
		SERIALPORT->println();
		outputDoc.clear();
	}
}

void JsonProtocol::clear_outputs() {
	if (!outputDoc.isNull()) {
		outputDoc.clear();
	}
}
