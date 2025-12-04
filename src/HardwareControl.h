#ifndef _HARDWARE_CONTROL_H_
#define _HARDWARE_CONTROL_H_

#include "Utils.h"
#include "Peripheral.h"
#include "Settings.h"
#include "Logger.h"




// Test for hardware failure on startup
/**
 * Hardware selftest.
 * Tests if all sensors are connected and working properly.
 * Prints error message and holds thread if one sensor is
 * not working as expected.
 * 
 * @note Light Sensors are expected to return values > 0
 * @note Ultra Sonic Sensor is expected to return values > 0
 */
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
    unsigned int attempt = 0;
    const unsigned int attemptCount = 3;
    for(; attempt < attemptCount; attempt++) {
        if(SensorUS(ULTRA_SONIC_SENSOR) > 0) {
            break;
        }
        Wait(500);
    }
    if(attempt >= (attemptCount - 1)) {
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


/**
 * Initialize and configure ports for its corresponding sensor types.
 */
void hardwareControl_init() {
    
    SetSensorLight(LEFT_SENSOR, true);
    SetSensorLight(RIGHT_SENSOR, true);
    SetSensorLight(BARCODE_SENSOR, true);

    SetSensorLowspeed(ULTRA_SONIC_SENSOR);

    hardwareControl_internal_selfTest();

}


/**
 * Applies power to the motor.
 * 
 * @param motorPort Port of motor to apply power
 * @param power Power in range of -100 to 100 to apply
 */
void applyMotorPower(int motorPort, int signedPower) {
    int p = clampInt(signedPower, -POWER_RANGE, POWER_RANGE);

    
    if (p > 0) OnFwd(motorPort, p);
    else if (p < 0) OnRev(motorPort, -p);
    else Off(motorPort);    
}

#endif
// _HARDWARE_CONTROL_H_