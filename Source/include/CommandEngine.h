// CommandEngine.h
#pragma once
#include <DataStructures.h>
#include <Constants.h>
#include <Stepper.h>
#include <LinkedList.h>

#ifndef _COMMANDENGINE_h
#define _COMMANDENGINE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CommandEngine
{    
    private:
        Stepper stepper;
        LinkedList<CANMessage> linkedList;
        uint8_t engineId;
        uint8_t pinStep;
        uint8_t pinDirection;
        uint8_t pinEnable;
        uint16_t _discardedCount = 0;

        void processCommand(CANMessage msg);

    public:
        CommandEngine(uint8_t engineId, uint8_t pinEnable, uint8_t pinDirection, uint8_t pinStep);
        uint8_t getEngineId();
        void handleCommand(CANMessage msg);
        void stepMotor(uint32_t now); // called from ISR only to avoid stutter
        void run();
        void begin();
        void printStatus();
};

#endif