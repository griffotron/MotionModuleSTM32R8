#include <Arduino.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <DataStructures.h>
#include <Constants.h>

Stepper::Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable){
    this->_pinStep = pinStep;
    this->_pinDirection = pinDirection;
    this->_pinEnable = pinEnable;
    this->_stepsToGo = 0;
    this->_newTargetSet = false;
    this->_emergencyStop = false;
    this->_enabled = false;
    this->_currentPosition = 0;
    this->_targetPosition = 0;
    this-> _totalSteps = 0;
    this-> _previousMicros = 0;
    this-> _currentInterval = MIN_STEP_INTERVAL;
    this-> _currentDirection = _forwardSignal;
    this->_totalStepsTaken = 0;
}

void Stepper::begin(){
    DEBUG_PRINT.println("Setting motor pins");
    pinMode(_pinStep, OUTPUT);
    pinMode(_pinDirection, OUTPUT);
    pinMode(_pinEnable, OUTPUT);
    disable();
}

void Stepper::setTarget(int32_t targetPosition, uint8_t speed, bool hold){
    DEBUG_PRINT.printf("New target postition:%i\n", targetPosition);

    if(targetPosition == _currentPosition){
        return; // already at position
    }

    if (_targetPosition == targetPosition && _stepsToGo > 0) {
        return; 
    }

    this->_targetPosition = targetPosition;

    if (_targetPosition < _currentPosition) {
        setReverse();
        _totalSteps = (_currentPosition - _targetPosition);
    }
    else {
        setForward();
        _totalSteps = (_targetPosition - _currentPosition);
    }

    _newTargetSet = true;
    _stepsToGo = _totalSteps;

    // speed is treated as a percentage of max speed. e.g. speed 90 = 90% of max speed
    if(speed > _maxSpeed) {
        speed = _maxSpeed;
    }

    if(speed == 0) {
        speed = 1;  // treat speed zero as one to avoid division by zero below (which would cause a hard fault!)
    }

    _currentInterval = (MIN_STEP_INTERVAL * (uint32_t)_maxSpeed) / (uint32_t)speed;
}

bool Stepper::done(){
    return _stepsToGo == 0;
}

void Stepper::cancel(){
    _stepsToGo = 0;
    disable();
}

void Stepper::step(uint32_t now){
    if (done() || _emergencyStop)
        return;
    
    if (now - _previousMicros < _currentInterval)
        return;

    _previousMicros = now;

    if (!_enabled)
        enable();

    if (_currentDirection == _forwardSignal) // increment or decrement steps depending on direction
        _currentPosition++;
    else
        _currentPosition--;

    digitalWrite(_pinStep, HIGH);
    digitalWrite(_pinStep, LOW);

    _stepsToGo--;
    _totalStepsTaken++;

    if (done())
        disable();
}

void Stepper::setForward() {
    _currentDirection = _forwardSignal;
    digitalWrite(_pinDirection, _forwardSignal);
}

void Stepper::setReverse() {
    _currentDirection = _reverseSignal;
    digitalWrite(_pinDirection, _reverseSignal);
}

void Stepper::enable(){
    _enabled = true;
    digitalWrite(_pinEnable, LOW);
}

void Stepper::disable() {
    _enabled = false;
    digitalWrite(_pinEnable, HIGH);
}

void Stepper::emergencyStop() {
    DEBUG_PRINT.print("Emergency stop!");
    _emergencyStop = true;
    disable();
}

void Stepper::emergencyStopReset(){
    DEBUG_PRINT.print("Emergency stop reset");
    _emergencyStop = false;
    _stepsToGo = 0;
}

long Stepper::currentPosition(){
    return _currentPosition;
}

void Stepper::setCurrentPositionToZero(){
    _currentPosition = 0;
    _targetPosition = 0;
}
