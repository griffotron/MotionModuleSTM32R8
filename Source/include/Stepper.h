// Stepper.h
#include <SoftwareSerial.h>
#include <DataStructures.h>

#ifndef _STEPPER_h
#define _STEPPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Stepper{
    private:
    	void setReverse();
	    void setForward();

        uint8_t _pinStep;
        uint8_t _pinDirection;
        uint8_t _pinEnable;
        bool _enabled;
        uint32_t _stepsToGo;
        int32_t _targetPosition;
        int32_t _totalSteps;
        int32_t _currentPosition;
	    uint32_t _previousMicros;
	    uint32_t _currentInterval;
        uint8_t _currentDirection;
        int _forwardSignal = HIGH;
	    int _reverseSignal = LOW;
        bool _newTargetSet;
        bool _emergencyStop;
        uint8_t _maxSpeed = 100;
        uint32_t _totalStepsTaken = 0;

    public:
        Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable);
        void setTarget(int32_t targetPosition, uint8_t speed, bool hold);
	    bool done();
	    void step(uint32_t now);
        void begin();
        void disable();
        void enable();
        void cancel();
        void emergencyStop();
        void emergencyStopReset();
        long currentPosition();
        void setCurrentPositionToZero();

        // getters for stats
        int32_t getTargetPosition() { return _targetPosition; }
        int32_t getStepsToGo() { return _stepsToGo; }
        bool isEmergencyStop() { return _emergencyStop; }
        bool isEnabled() { return _enabled; }
        uint32_t getTotalStepsTaken() { return _totalStepsTaken; }
};

#endif