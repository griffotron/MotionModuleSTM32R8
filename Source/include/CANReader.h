// Receiver.h
#pragma once
#include <DataStructures.h>
#include <Constants.h>
#include "CANBus.h"

#ifndef _CANREADER_h
#define _CANREADER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CANReader{
	private:
		CANBus& _canBus;

		// stats
		uint32_t _messagesReceived    = 0;
        uint32_t _remoteFramesDropped = 0;

	public: 
        CANReader(CANBus& canBus);
		void begin();
        void addFilterId(int index, unsigned long id);
		void addFilterIdPair(int index, unsigned long id1, unsigned long id2);
		bool read(CANMessage& buffer);

		//getters for stats
		uint32_t getMessagesReceived() { return _messagesReceived; }
        uint32_t getRemoteFramesDropped() { return _remoteFramesDropped; } 
};

#endif