#pragma once

#include "Modulex.hpp"

class LED_interface : public Modulex {
public :
	const int Plus = 33;
	const int Minus = 37;
	const int SetupPin = 35;
	const int Led = 3;
};