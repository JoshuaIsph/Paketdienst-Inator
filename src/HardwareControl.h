#ifndef _HARDWARE_CONTROL_H_
#define _HARDWARE_CONTROL_H_

#include "Utils.h"
#include "Peripheral.h"
#include "Settings.h"
#include "logger.h"


// TODO: speed adjustment on the hill

// Test for hardware failure on startup
void hardwareControl_internal_selfTest() {

  byte errorFlag = 0;

  if(SensorValue(LEFT_SENSOR) == 0) {
    errorFlag = 1;
    log_println("Left Light Sensor");
  }
  if(SensorValue(RIGHT_SENSOR) == 0) {
    errorFlag = 1;
    log_println("Right Light Sensor");
  }
  if(SensorValue(MIDDLE_SENSOR) == 0) {
    errorFlag = 1;
    log_println("Middle Light Sensor");
  }
  if(SensorUS(ULTRA_SONIC_SENSOR) == 0) {
    errorFlag = 1;
    log_println("Ultra Sonic Sensor");
  }


  if(errorFlag) {

    log_println("Not responding");
    log_playAlarm();
    log_waitForUserInput();
  }

  log_playStatusSound();
}


// configure sensors
void hardwareControl_init() {
  
  SetSensorLight(LEFT_SENSOR, true);
  SetSensorLight(RIGHT_SENSOR, true);
  SetSensorLight(BARCODE_SENSOR, true);

  SetSensorLowspeed(ULTRA_SONIC_SENSOR);


  hardwareControl_internal_selfTest();

}


void applyMotorPower(int motorPort, int signedPower) {
    int p = clampInt(signedPower, -POWER_RANGE, POWER_RANGE);
    if (p > 0) OnFwd(motorPort, p);
    else if (p < 0) OnRev(motorPort, -p);
    else Off(motorPort);
}

#endif
