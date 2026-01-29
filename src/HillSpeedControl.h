#ifndef _HILL_SPEED_CONTROL_H_
#define _HILL_SPEED_CONTROL_H_

#include "Settings.h"
#include "Utils.h"
#include "Peripheral.h"
#include "HardwareControl.h"
#include "Pid_Control.h"
#include "Logger.h"


#define MIN_TACHO_SPEED 7  // 7
#define MAX_TACHO_SPEED 15 // 20

bool hillSpeedRegulation = true;    // Variable to enable hill speed regulation



/**
 * Function to enable or disable hill speed regulation.
 * 
 * @param enable Set true to enable. Set false to disable.
 */
void hillSpeedControl_enable(bool enable) {
    hillSpeedRegulation = enable;
}


/**
 * Updates hill speed regulation control.
 * Controls the speedlimit of pid control, to get over the hill.
 * 
 * If motors sense tacho values lower than MIN_TACHO_SPEEd, but higher speed is expected,
 * speed limit will be increased to MAX_SPEED.
 * If motors sense tacho values higher than MAX_TACHO_SPEED,
 * speed limit will be lowerd to TAVEL_SPEED.
 * 
 * @param leftSpeed Current raw tacho value of left motor
 * @param rightSpeed Current raw tacho value fo right motor
 */
void hillSpeedControl_update(int leftSpeed, int rightSpeed) {

    if(!hillSpeedRegulation) {
        return;
    }

    //log_println(NumToStr(leftSpeed));

    int leftPowerSet = MotorPower(LEFT_MOTOR);
    int rightPowerSet =  MotorPower(RIGHT_MOTOR);

    //log_println(StrCat("Left: ", NumToStr(leftPowerSet)));
    //log_println(StrCat("Right: ", NumToStr(rightPowerSet)));

    // Prevent cpu overload, mainly due to printing
    static bool tooSlow = true;
    static bool tooFast = true;

    const static int min_speed_threshold = (MIN_SPEED + TRAVEL_SPEED) / 2;

    if(leftPowerSet >= min_speed_threshold && rightPowerSet >= min_speed_threshold) {
        if(tooSlow && leftSpeed < MIN_TACHO_SPEED && rightSpeed < MIN_TACHO_SPEED) {
            pid_setBaseSpeed(MAX_SPEED);
            tooFast = true;
            tooSlow = false;
            //PlayTone(500, 1000);
            log_println("Too slow");
        }
    }

    if(tooFast && (leftSpeed > MAX_TACHO_SPEED || rightSpeed > MAX_TACHO_SPEED)) {
        pid_setBaseSpeed(TRAVEL_SPEED);
        tooFast = false;
        tooSlow = true;
        //PlayTone(100, 1000);
        log_println("Too fast");
    }

}



#endif
// _HILL_SPEED_CONTROL_H_