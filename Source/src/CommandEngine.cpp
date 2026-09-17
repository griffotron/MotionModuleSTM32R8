// CommandEngine.h
#include <DataStructures.h>
#include <Constants.h>
#include <Stepper.h>
#include <CommandEngine.h>

CommandEngine::CommandEngine(uint8_t engineId, uint8_t pinEnable, uint8_t pinDirection, uint8_t pinStep): 
    stepper(pinStep, pinDirection, pinEnable), linkedList()
{
    this->engineId = engineId;
    this->pinStep = pinStep;
    this->pinDirection = pinDirection;
    this->pinEnable = pinEnable;
}

void CommandEngine::begin(){
    DEBUG_PRINT.printf("Initialising engine %i. Starting stepper\n", engineId);
    stepper.begin();
}

uint8_t CommandEngine::getEngineId(){
    return engineId;
}

void CommandEngine::processCommand(CANMessage msg){
    // msg.Data[0] is engine id
    uint8_t speed = msg.Data[1];
    int32_t targetPosition = decodeInt32(msg.Data, 2);

    switch(msg.Type){
        case CANMessageType::MoveToTarget:
            DEBUG_PRINT.printf("Setting new target position to %d\n", targetPosition);
            __disable_irq(); // need this to avoid stepper observing partial update (_stepsToGo, _currentDirection)
            stepper.setTarget(targetPosition, speed, false);
            __enable_irq();
            break;
        case CANMessageType::SetCurrentPositionToZero:
            DEBUG_PRINT.printf("Setting current position to zero\n");
            __disable_irq(); // need this to avoid stepper observing partial update (_stepsToGo, _currentDirection)
            stepper.setCurrentPositionToZero();
            __enable_irq();
            break;
    }
}

static int compare(CANMessage &a, CANMessage &b){
    return a.Priority > b.Priority;
}

void CommandEngine::handleCommand(CANMessage msg){
    // some commands are handled immediately, some like motor movement will be queued
    switch(msg.Type){
        case CANMessageType::EmergencyStop:
            stepper.emergencyStop();
            break;
        case CANMessageType::CancelLastCommand:
            stepper.cancel();
            break;
        case CANMessageType::EmergencyStopReset:
            stepper.emergencyStopReset();
            linkedList.clear();
            break;
        case CANMessageType::SetCurrentPositionToZero:
        case CANMessageType::MoveToTarget:
            if(linkedList.size() >= MAX_COMMAND_QUEUE_LENGTH){
                DEBUG_PRINT.printf("Discarding msg for engine %i - queue full!\n", engineId);
                _discardedCount++;
                return;
            }
            linkedList.add(msg);
            linkedList.sort(compare);
            DEBUG_PRINT.printf("New queue length for engine %i: %i\n", engineId, linkedList.size());
            break;
    }       
}

void CommandEngine::run(){
    // run in main loop to process queued messages
    if(stepper.isEmergencyStop())
        return;

    if(stepper.done() && linkedList.size() > 0){ // process next move msg if we're ready
        CANMessage cmd = linkedList.shift();

        #if DEBUG
        int32_t targetPosition = decodeInt32(cmd.Data, 2);
        DEBUG_PRINT.printf("Popped from queue for engine %i. Priority: %i AddressId: %i position: %i Queue Length: %i\n", engineId, cmd.Priority, cmd.AddressId, targetPosition, linkedList.size());
        #endif
        
        processCommand(cmd);
    }
}

void CommandEngine::stepMotor(uint32_t now){
    // run only from ISR to prevent possible stutter if called from main loop
    stepper.step(now);
}

void CommandEngine::printStatus() {
    const char* stateStr;
    if      (stepper.isEmergencyStop())  stateStr = "E-STOP ";
    else if (!stepper.done())            stateStr = "Moving ";
    else                                 stateStr = "Idle   ";

    Serial.printf(" %2u  %10ld  %10ld  %5d  %8u  %10ld  %12lu  %s\n",
        engineId,
        stepper.currentPosition(),
        stepper.getTargetPosition(),
        linkedList.size(),
        _discardedCount,
        stepper.getStepsToGo(),
        stepper.getTotalStepsTaken(),
        stateStr
    );
}
