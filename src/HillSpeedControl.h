#ifndef _HILL_SPEED_CONTROL_H_
#define _HILL_SPEED_CONTROL_H_

#include "Settings.h"
#include "Utils.h"
#include "Peripheral.h"
#include "HardwareControl.h"
#include "Pid_Control.h"
#include "Logger.h"


#define MIN_TACHO_SPEED 5
#define MAX_TACHO_SPEED 20

bool hillSpeedRegulation = true;


void hillSpeedControl_enable(bool enable) {
    hillSpeedRegulation = enable;
}


void hillSpeedControl_update(int leftSpeed, int rightSpeed) {

    if(!hillSpeedRegulation) {
        return;
    }

    //log_println(NumToStr(leftSpeed));

    if(leftSpeed < MIN_TACHO_SPEED && rightSpeed < MIN_TACHO_SPEED) {
        pid_setBaseSpeed(MAX_SPEED);
        //PlayTone(500, 1000);
        log_println("Too slow");
    }

    if(leftSpeed > MAX_TACHO_SPEED || rightSpeed > MAX_TACHO_SPEED) {
        pid_setBaseSpeed(TRAVEL_SPEED);
        //PlayTone(100, 1000);
        log_println("Too fast");
    }

}



#endif
