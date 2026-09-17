# Introduction 
This repository contains the PCB design files and source code for the STM32 based Motion Module used in my Mechanical Display.

The Motion Module uses an STM32F103R8 microcontroller to run 14 Trinamic TMC2209 stepper drivers. It recieves motor movement commands via CAN Bus from the ESP32 based Motion Controller (see separate repo).

Full video of the build to follow.

# Getting Started

## PCB
You'll need at least KiCad 8 to view and modify the PCB design files. They PCB/production folder contains files suitable for production with JLC PCB, including a zip containing the Gerbers for fabrication and bom.csv and positions.csv for assembly. The bom.csv uses components from LCSC.com. If any components are no longer be available/out of stock you'll need to select an alternative.

## Code
The code uses PlatformIO (I used the VSCode plugin) and the Arduino framework.

CAN Bus library is from https://github.com/pazi88/CANBus

