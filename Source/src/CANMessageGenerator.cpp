#include <Arduino.h>
#include <DataStructures.h>
#include <Constants.h>
#include <CANMessageGenerator.h>

CANMessage CANMessageGenerator::createMoveToTargetMessage(int32_t target, uint8_t engineId){
  std::array<uint8_t, 4> encodedTarget = encodeInt32(target);
  std::array<uint8_t, 6> data = { engineId, 100, encodedTarget[0], encodedTarget[1], encodedTarget[2], encodedTarget[3] };
  
  CANMessage msg = { .AddressId = MODULE_ID, .Type = CANMessageType::MoveToTarget, .Priority = 1, .Data = data};

  DEBUG_PRINT.printf("Created MoveToTarget CAN message for engine %u with target %d\n", engineId, target);
  
  return msg;
}

CANMessage CANMessageGenerator::createModuleStatusResponseMessage(uint32_t address, uint32_t moduleId, uint8_t status){
  std::array<uint8_t, 4> encodedModuleId = encodeInt32(moduleId);
  std::array<uint8_t, 6> data = { status, 0, encodedModuleId[0], encodedModuleId[1], encodedModuleId[2], encodedModuleId[3] };

  CANMessage msg = { .AddressId = address, .Type = CANMessageType::ModuleStatusResponse, .Priority = 1, .Data = data };

  DEBUG_PRINT.printf("Created ModuleStatusResponse CAN message for address %u with status %u\n", address, status);

  return msg;
}