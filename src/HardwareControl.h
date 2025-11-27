#ifndef _HARDWARE_CONTROL_H_
#define _HARDWARE_CONTROL_H_

#include "Utils.h"
#include "Peripheral.h"
#include "Settings.h"
#include "logger.h"



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



#define MOTOR_P PID_7
#define MOTOR_I PID_1
#define MOTOR_D PID_0

// Applies motor speed and tries to hold it
void applyMotorSpeed(int motorPort, int signedPower) {
    int p = clampInt(signedPower, -POWER_RANGE, POWER_RANGE);

    
    //OnFwdRegPID(OUT_BC, 10, OUT_REGMODE_SPEED, PID_7, PID_1, PID_0); // Test

    if(p > 0) {
        OnFwdRegPID(motorPort, p, OUT_REGMODE_SPEED, MOTOR_P, MOTOR_I, MOTOR_D);
    } else if(p < 0) {
        OnRevRegPID(motorPort, p, OUT_REGMODE_SPEED, MOTOR_P, MOTOR_I, MOTOR_D);
    } else {
        Off(motorPort);
    }
}


// Just applies power to motor
void applyMotorPower(int motorPort, int signedPower) {
    int p = clampInt(signedPower, -POWER_RANGE, POWER_RANGE);

    
    if (p > 0) OnFwd(motorPort, p);
    else if (p < 0) OnRev(motorPort, -p);
    else Off(motorPort);    
}

#endif
