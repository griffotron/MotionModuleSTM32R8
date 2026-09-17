#pragma once
#include <DataStructures.h>
#include <Constants.h>

#ifndef _CANMSGGEN_h
#define _CANMSGGEN_h

class CANMessageGenerator {
	private:
		CANMessageGenerator() = default;

	public: 
		CANMessageGenerator(const CANMessageGenerator&) = delete;
    	CANMessageGenerator& operator=(const CANMessageGenerator&) = delete;
        
		static CANMessage createMoveToTargetMessage(int32_t target, uint8_t engineId);
		static CANMessage createModuleStatusResponseMessage(uint32_t address, uint32_t moduleId, uint8_t status);
};

#endif