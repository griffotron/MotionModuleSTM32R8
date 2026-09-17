// Receiver.h
#pragma once
#include <DataStructures.h>
#include <Constants.h>
#include "CANBus.h"

#ifndef _CANWRITER_h
#define _CANWRITER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CANWriter{
	private:
		CANBus& _canBus;

		//stats
		uint32_t _messagesSent = 0;
        uint32_t _messagesFailed = 0;

	public: 
        CANWriter(CANBus& canBus);
		void write(CANMessage msg);

		//getters for stats
		uint32_t getMessagesSent() { return _messagesSent; }
        uint32_t getMessagesFailed() { return _messagesFailed; }
};

#endif