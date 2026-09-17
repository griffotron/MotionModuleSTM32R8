#include "Arduino.h"
#include "CANReader.h"
#include <Constants.h>

CANReader::CANReader(CANBus& canBus) : _canBus(canBus)
{
}

void CANReader::addFilterId(int index, unsigned long id){
   _canBus.setMBFilterProcessing(static_cast<CAN_BANK>(index), id, 0x1FFFFFFF, STD);
}

void CANReader::addFilterIdPair(int index, unsigned long id1, unsigned long id2){
    //(CAN_BANK bank_num, uint32_t id1, uint32_t id2, IDE std_ext)
    _canBus.setMBFilter(static_cast<CAN_BANK>(index), id1, id2, STD);
}

void CANReader::begin(){
    DEBUG_PRINT.println("Setting up CAN");

    _canBus.begin();
    //_canBus.setBaudRate(500000);  //500KBPS
    _canBus.setBaudRate(1000000);  //1000KBPS

    DEBUG_PRINT.println("Receiver ready");
}

void processBuffer(CANMessage& msg, CAN_message_t canMessage){

    DEBUG_PRINT.printf("Recieved msg for module %u\n", canMessage.id);

    msg.AddressId = canMessage.id;
    msg.Type = canMessage.buf[0];
    msg.Priority = canMessage.buf[1];

    for (uint8_t i = 2; i < 8; i++) {
        msg.Data[i-2] = canMessage.buf[i];
    }
}

static CAN_message_t CAN_RX_msg;

bool CANReader::read(CANMessage& msg){
    if (_canBus.read(CAN_RX_msg) ) {
        if (CAN_RX_msg.flags.remote == false && CAN_RX_msg.len == 8) {
            _messagesReceived++;
            processBuffer(msg, CAN_RX_msg);
            return true;
        }
        _remoteFramesDropped++;
        DEBUG_PRINT.println("Frame dropped");
        return false;
    }

    return false;
}