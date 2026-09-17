#include <Arduino.h>
#include <SoftwareSerial.h>
#include <CANBus.h>
#include <CANReader.h>
#include <CANWriter.h>
#include <Stepper.h>
#include <CommandEngine.h>
#include <DataStructures.h>
#include <Constants.h>
#include <SystemManagement.h>
#include <CANMessageGenerator.h>
#include <HardwareTimer.h>

uint8_t engineIds[TOTAL_ENGINES] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; //Engine Ids are the same for all modules. Controller sends message to module and engine id within it

// Pin assignment: Enable, Direction, Step
CommandEngine engineArray[TOTAL_ENGINES] = {
  CommandEngine(engineIds[0], PA7, PB15, PC6), //A
  CommandEngine(engineIds[1], PA6, PB2, PB10), //B
  CommandEngine(engineIds[2], PA5, PB13, PB14), //C
  CommandEngine(engineIds[3], PA4, PB0, PB1), //D
  CommandEngine(engineIds[4], PA1, PB11, PB12), //E
  CommandEngine(engineIds[5], PA0, PC4, PC5), //F
  CommandEngine(engineIds[6], PC2, PC1, PC3), //G
  CommandEngine(engineIds[7], PA15, PC7, PC8), //H
  CommandEngine(engineIds[8], PC12, PC10, PC11), //I
  CommandEngine(engineIds[9], PB3, PC9, PA8), //J
  CommandEngine(engineIds[10], PB6, PB4, PB5), //K
  CommandEngine(engineIds[11], PB7, PA9, PA10), //L
  CommandEngine(engineIds[12], PC13, PB8, PB9), //M
  CommandEngine(engineIds[13], PC0, PC15, PC14) //N
};

HardwareTimer *stepTimer;
CANMessage msg;
CANBus canBus = CANBus(CAN1, DEF);
CANReader canBusReader = CANReader(canBus);
CANWriter canBusWriter = CANWriter(canBus);
SystemManagement systemMgmt = SystemManagement();
static String serialInputBuffer;
extern "C" char* sbrk(int incr);
static uint32_t _minFreeRam = UINT32_MAX;

static uint32_t getFreeRam() {
    char stackTop;
    uint32_t freeRam = (uint32_t)&stackTop - (uint32_t)sbrk(0);
    if (freeRam < _minFreeRam)
      _minFreeRam = freeRam;
    return freeRam;
}

void printStats()
{
  getFreeRam();

  // ---- Uptime -------------------------------------------------------
  const uint32_t ms      = millis();
  const uint32_t seconds = ms      / 1000UL;
  const uint32_t minutes = seconds / 60UL;
  const uint32_t hours   = minutes / 60UL;
  const uint32_t days    = hours   / 24UL;

  Serial.println(F("\n========== System Statistics =========="));
  Serial.printf("Module Id   : %u\n", MODULE_ID);
  Serial.printf("Uptime      : %lud %02lu:%02lu:%02lu.%03lu\n", days, hours % 24, minutes % 60, seconds % 60, ms % 1000);

  // ---- Memory -------------------------------------------------------
  const uint32_t freeRam = getFreeRam();
  Serial.printf("Free RAM    : %lu bytes  (min seen: %lu bytes, total: 20480 bytes)\n", freeRam, _minFreeRam);

  // ---- CPU ----------------------------------------------------------
  Serial.printf("CPU Freq    : %lu MHz\n", SystemCoreClock / 1000000UL);

  // ---- CAN ----------------------------------------------------------
  Serial.printf("CAN RX      : %lu received,  %lu remote frames dropped\n", canBusReader.getMessagesReceived(), canBusReader.getRemoteFramesDropped());
  Serial.printf("CAN TX      : %lu sent,  %lu failed\n", canBusWriter.getMessagesSent(), canBusWriter.getMessagesFailed());

  // ---- Motor status -------------------------------------------------
  Serial.println();
  Serial.println(F("Motor Status:"));
  Serial.println(F(" ID    Position    Target   Queue  Dropped  Steps Left   Total Steps  State"));
  Serial.println(F(" --  ----------  ----------  -----  -------  ----------  ------------  -------"));

  for (uint8_t i = 0; i < TOTAL_ENGINES; i++) {
      engineArray[i].printStatus();
  }

  Serial.println(F("=======================================\n"));
}

void blink(uint8_t times, int32_t durationOn, int32_t durationOff){
  uint8_t flashCount = 0;
  for(flashCount = 0; flashCount < times; flashCount++){
    digitalWrite(PIN_STATUS_LED, HIGH);
    delay(durationOn);
    digitalWrite(PIN_STATUS_LED, LOW);
    delay(durationOff);
  }
}

void reset() {
  DEBUG_PRINT.println("Resetting");
  blink(3, 100, 100);
  NVIC_SystemReset();
}

void runSelfTest(){
  DEBUG_PRINT.println("Running self test...");
  for(uint8_t i = 0; i < TOTAL_ENGINES; i++){
    engineArray[i].handleCommand(CANMessageGenerator::createMoveToTargetMessage(5000, i+1));
    engineArray[i].handleCommand(CANMessageGenerator::createMoveToTargetMessage(-5000, i+1));
    engineArray[i].handleCommand(CANMessageGenerator::createMoveToTargetMessage(0, i+1));
    DEBUG_PRINT.printf("Queued command for motor %u\n", i);
  }
}

void stepAllMotors(){
  const uint32_t now = micros(); // grab time here so all motors stay in sync if ISR overlaps
  for(int i = 0; i < TOTAL_ENGINES; i++){ // step each stepper
    engineArray[i].stepMotor(now);
  }
}

void handleCommand(CANMessage& newMsg){
  DEBUG_PRINT.printf("Command for worker %u recieved\n", newMsg.AddressId);
  uint8_t engineId = newMsg.Data[0];
  
  if(engineId == ALL_MODULE_ID){   //pass to all engines
    for(uint8_t i = 0; i < TOTAL_ENGINES; i++){
      engineArray[i].handleCommand(newMsg);
    }
    return;
  }

  if(engineId >= 1 && engineId <= TOTAL_ENGINES){  //pick specific engine
    engineArray[engineId - 1].handleCommand(newMsg);
    return;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_STATUS_LED,OUTPUT);

  Serial.println("Setting up message engines");

  for(int i = 0; i < TOTAL_ENGINES; i++){ // begin each stepper
    engineArray[i].begin();
  }

  blink(1, 200, 200);
  
  Serial.println("Setting up CAN receiver");
  canBusReader.begin();
  canBusReader.addFilterId(0, MODULE_ID); // read messages meant for this module
  canBusReader.addFilterId(1, ALL_MODULE_ID); // read messages meant for "all modules"
  
  blink(1, 200, 200);

  // TIM2: fire every MIN_STEP_INTERVAL microseconds
  Serial.println("Setting up stepper interrupt timer");
  stepTimer = new HardwareTimer(TIM2);
  stepTimer->setOverflow(MIN_STEP_INTERVAL, MICROSEC_FORMAT); // 300ms = ~3333 Hz
  stepTimer->attachInterrupt(stepAllMotors);
  stepTimer->resume();
  
  blink(1, 200, 200); 

  Serial.println("Ready");
}

void loop() {
  getFreeRam(); // keep ram stats up to date

  while (Serial.available() > 0) {
      char c = (char)Serial.read(); // non blocking read (unlike readStringUntil which can cause motor stutter)
      if (c == '\n') {
          serialInputBuffer.trim();
          if (serialInputBuffer.equals("reset"))
            reset();
          if (serialInputBuffer.equals("test"))
            runSelfTest();
          if (serialInputBuffer.equals("stats"))
            printStats();
          serialInputBuffer = "";
      } else {
          serialInputBuffer += c;
      }
  }

  bool newData = canBusReader.read(msg);
  if(newData)
  {
    digitalWrite(PIN_STATUS_LED, HIGH); // flash on LED during receive

    switch(msg.Type){
      case CANMessageType::MoveToTarget:
      case CANMessageType::SetCurrentPositionToZero: // reset current position of stepper to 0
      case CANMessageType::EmergencyStop:
      case CANMessageType::CancelLastCommand:
      case CANMessageType::EmergencyStopReset:
        handleCommand(msg);
        break;
      case CANMessageType::ModuleStatusRequest:
        canBusWriter.write(systemMgmt.getModuleStatusResponse(msg, MODULE_ID));
        break;
      case CANMessageType::Stats:
        printStats();
        break;
      case CANMessageType::FullReset:  // reset microcontroller
        reset();
        break;
      default: 
        DEBUG_PRINT.print("Unknown message type ");
        DEBUG_PRINT.println(msg.Type);
    }

    digitalWrite(PIN_STATUS_LED, LOW); // endof recieve, LED off  
  }

  for(int i = 0; i < TOTAL_ENGINES; i++){ // step each stepper
    engineArray[i].run();
  }
}




