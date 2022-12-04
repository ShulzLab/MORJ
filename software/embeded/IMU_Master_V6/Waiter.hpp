#pragma once

//////////////////////////////
//	FORWARD DECLARATIONS	//
//////////////////////////////


struct Waiter {
    enum unit_t : bool {
        millisec = false,
        microsec = true,
    };

    unit_t unit = millisec;
    uint32_t memory;
    uint32_t interval;

    //Set units either in milliseconds (Waiter::millisec) (default behaviour) or in microseconds (Waiter::microsec).
    void set_unit(unit_t l_unit);

    //call this when you want to start waiting for a duration of interval (non blocking). Duration will be in millis or micros depending on if you set_unit.
    void wait(uint32_t _interval);
    void wait();
    void wait_impatiently();//start waiting on call, and don't recall wait on sucessive calls (that would delay the elapsed evaluation as true) until exhausted by an elapsed call. 
    //Calling a simple wait before elapsed becomes true cancels this behaviour.
    bool elapsed();
    void elapse_now();
    void stop();

    uint32_t get_elapsed_time();

    bool operator()();// call the object to verify if time has passed. If it returns true, time is due and subsequent calls will be false. Call wait again to begin a new waiting phase.

    bool exhausted = true;//once the boolean true has been sent for a waiting period, we don't send true again until wait is called again. Exhausted serves this purpose
    bool impatient = false;

    Waiter(uint32_t _interval, unit_t l_unit, bool start_now);
    Waiter(uint32_t _interval, unit_t l_unit);
    Waiter(uint32_t _interval);
    Waiter();
};

uint32_t Waiter::get_elapsed_time() {
    if (unit) {//unit = microsec
        return micros() - memory;
    }
    //else unit = millisec
    return millis() - memory;
}

Waiter::Waiter(uint32_t _interval, unit_t l_unit, bool start_now) {
    interval = _interval;
    unit = l_unit;
    if (start_now) {
        wait();
    }
}

Waiter::Waiter(uint32_t _interval, unit_t l_unit) {
    interval = _interval;
    unit = l_unit;
}

Waiter::Waiter(uint32_t _interval) {
    interval = _interval;
    unit = millisec;
}

Waiter::Waiter() {
    interval = 0;
    unit = millisec;
}


void Waiter::set_unit(unit_t l_unit) {
    unit = l_unit;
}

void Waiter::wait(uint32_t _interval) {//next .elapsed() evaluates as true after interval has passed. (sets new interval)
    wait();
    interval = _interval;
}

void Waiter::wait() {//next .elapsed() evaluates as true after interval has passed. (internal value is kept from setup and previous calls)
    impatient = false;
    if (unit) {//unit = microsec
        memory = micros();
    }
    else {//unit = millisec
        memory = millis();
    }
    exhausted = false;
}

void Waiter::wait_impatiently() {
    if (!impatient) {
        wait();
        impatient = true;
    }
}

void Waiter::elapse_now() {//next .elapsed() evaluates as true immediately.
    wait();
    memory = memory - interval;
}

void Waiter::stop() {//prevent next .elapsed() from evaluating true if a wait has been called before. Stop the "timer" from ringing.
    exhausted = true;
    impatient = false;
}

bool Waiter::elapsed() {// test wether enoug time has passed since previous wait call. Returns true only once per wait call.
    if (unit) {//unit = microsec
        if ((!exhausted) && (micros() - memory >= interval)) {
            exhausted = true;
            impatient = false;
            return true;
        }
    }
    else {//unit = millisec
        if ((!exhausted) && (millis() - memory >= interval)) {
            exhausted = true;
            impatient = false;
            return true;
        }
    }
    return false;
}

bool Waiter::operator()() {// just another way to call elapsed and check wether time has passed : ``Waiter()`` instead of ``Waiter.elapsed()``. returns true or false.
    return elapsed();
}