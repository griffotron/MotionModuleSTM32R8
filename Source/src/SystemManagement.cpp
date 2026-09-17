#include <SystemManagement.h>
#include <Arduino.h>
#include <DataStructures.h>
#include <CANMessageGenerator.h>
#include <Constants.h>

CANMessage SystemManagement::getModuleStatusResponse(CANMessage& msg, uint32_t moduleId){
    DEBUG_PRINTF("Request for module status received\n");

    uint8_t addr0 = msg.Data[2];
    uint8_t addr1 = msg.Data[3];
    uint8_t addr2 = msg.Data[4];
    uint8_t addr3 = msg.Data[5];
    uint32_t returnAddress = ((uint32_t)addr3 << 24) | ((uint32_t)addr2 << 16) | ((uint32_t)addr1 << 8) | addr0;

    DEBUG_PRINTF("Return address %u\n", returnAddress);

    return CANMessageGenerator::createModuleStatusResponseMessage(returnAddress, moduleId, MODULE_STATUS_OK);    
}