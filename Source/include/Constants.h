#ifndef _CONSTANTS_h
#define _CONSTANTS_h

#define DEBUG false  //set to true for debug output, false for no debug output
#define DEBUG_PRINT if(DEBUG)Serial
#define DEBUG_PRINTF if(DEBUG)Serial.printf

#define PIN_STATUS_LED PD2

/* THIS IS THE UNIQUE IDENTIFIER FOR EACH PCB MODULE */
const uint32_t MODULE_ID = 1; 

const uint8_t TOTAL_ENGINES = 14;
const uint8_t MAX_COMMAND_QUEUE_LENGTH = 20;
const uint8_t ALL_MODULE_ID = 0;
const uint32_t MIN_STEP_INTERVAL = 300;
const uint8_t MODULE_STATUS_OK = 1;

#endif