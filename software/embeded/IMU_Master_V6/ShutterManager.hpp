#pragma once

class ShutterManager : public Modulex {
public :
    uint8_t shutter_pin;

    bool off_polarity = LOW;//by default, LOW is the state wehn shutter is off.

    void activate_shutter();
    void deactivate_shutter();

    void answer_json_request(JsonObject json_request, JsonDocument& outputDoc);

    ShutterManager(const char* l_name, uint8_t l_pin);
    ShutterManager(const char* l_name, uint8_t l_pin, bool l_off_polarity);

};

ShutterManager::ShutterManager(const char* l_name, uint8_t l_pin) : Modulex(l_name) {
    shutter_pin = l_pin;
    pinMode(l_pin, OUTPUT);
    digitalWrite(l_pin, off_polarity);
}

ShutterManager::ShutterManager(const char* l_name, uint8_t l_pin, bool l_off_polarity) : Modulex(l_name) {
    shutter_pin = l_pin;
    off_polarity = l_off_polarity;
    pinMode(l_pin, OUTPUT);
    digitalWrite(l_pin, off_polarity);
}

void ShutterManager::activate_shutter() {
    digitalWrite(shutter_pin, !off_polarity);
    status = activated;
}

void ShutterManager::deactivate_shutter() {
    digitalWrite(shutter_pin, off_polarity);
    status = deactivated;
}

void ShutterManager::answer_json_request(JsonObject json_request, JsonDocument& outputDoc) {
    if (json_request.containsKey("set_status")) {
        if ((bool)json_request["set_status"]) {
            activate_shutter();
        }
        else {
            deactivate_shutter();
        }
    }
    Modulex::answer_json_request(json_request, outputDoc);
}
