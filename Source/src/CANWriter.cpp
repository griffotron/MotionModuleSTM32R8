#include "Arduino.h"
#include "CANWriter.h"
#include <Constants.h>

CANWriter::CANWriter(CANBus& canBus) : _canBus(canBus)
{
}

void CANWriter::write(CANMessage msg){
    CAN_message_t canMsg;

    canMsg.id = msg.AddressId;
    canMsg.buf[0] = msg.Type;
    canMsg.buf[1] = msg.Priority;
    canMsg.buf[2] = msg.Data[0];
    canMsg.buf[3] = msg.Data[1];
    canMsg.buf[4] = msg.Data[2];
    canMsg.buf[5] = msg.Data[3];
    canMsg.buf[6] = msg.Data[4];
    canMsg.buf[7] = msg.Data[5];

    if(_canBus.write(canMsg)){
        _messagesSent++;
        DEBUG_PRINTF("Sent message to address %d\n", canMsg.id);
    }
    else{
        _messagesFailed++;
        DEBUG_PRINTF("Failed to send message to address %d", canMsg.id);
    }
}