#pragma once
#include <Arduino.h>
#include <DataStructures.h>

#ifndef _SYSTEMMGMT_h
#define _SYSTEMMGMT_h

class SystemManagement{
	private:

	public: 
        CANMessage getModuleStatusResponse(CANMessage& msg, uint32_t moduleId);
};


#endif