
#ifndef _DATASTRUCTS_h
#define _DATASTRUCTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

inline int32_t decodeInt32(const std::array<uint8_t,6>& d, uint8_t offset) {
    return (int32_t)(((uint32_t)d[offset+3] << 24) | ((uint32_t)d[offset+2] << 16) |
                     ((uint32_t)d[offset+1] << 8)  |  (uint32_t)d[offset]);
}

inline std::array<uint8_t, 4> encodeInt32(int32_t int32Val) {
    return { (uint8_t)(int32Val & 0xFF), (uint8_t)((int32Val >> 8) & 0xFF), (uint8_t)((int32Val >> 16) & 0xFF), (uint8_t)((int32Val >> 24) & 0xFF) };
}

struct CANMessage {
    uint32_t AddressId; // Id of the Module
	uint8_t Type; 		// byte 1 of CAN message
	uint8_t Priority; 	// byte 2 of CAN message
	std::array<uint8_t, 6> Data; 	// byte 3-8 of CAN message
};

class CANMessageType
{
	public:
	static const uint8_t MoveToTarget = 0x1;
	static const uint8_t MoveToHome = 0x2;
	static const uint8_t SetCurrentPositionToZero = 0x3;
	static const uint8_t CancelLastCommand = 0x4;
	static const uint8_t EmergencyStop = 0x5;
	static const uint8_t EmergencyStopReset = 0x6;
	static const uint8_t Stats = 0x7;
	static const uint8_t FullReset = 0x8;
	static const uint8_t ModuleStatusRequest = 0x9;
	static const uint8_t ModuleStatusResponse = 0x0A;

};


#endif