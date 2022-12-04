#pragma once

class MotorForwarder : public Modulex  {
public :
    JsonProtocol* pc_port;
    JsonProtocol* motor_port;

    void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);

    MotorForwarder(char* l_name,  JsonProtocol* l_pc_port, JsonProtocol* l_motor_port );
};

MotorForwarder::MotorForwarder(char* l_name,JsonProtocol* l_pc_port , JsonProtocol* l_motor_port) : Modulex(l_name) {
  pc_port = l_pc_port;
  motor_port = l_motor_port;
}

void MotorForwarder::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
    //the only thing this class does, is when it's name is called, to send the inputs from PC directly to the motor
    motor_port->outputDoc.set(json_request);
}