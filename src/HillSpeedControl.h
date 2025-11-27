#ifndef _HILL_SPEED_CONTROL_H_
#define _HILL_SPEED_CONTROL_H_

#include "Peripheral.h"
#include "HardwareControl.h"
#include "Pid_Control.h"
#include "Logger.h"

#define MIN_SPEED 60
#define MAX_SPEED 80

byte motors[] = {LEFT_MOTOR, RIGHT_MOTOR};
int lastTachoVal[DRIVE_MOTOR_COUNT];
int tachoVal[DRIVE_MOTOR_COUNT];


void hillSpeedControl_init() {

    log_printSerial(StrCat("Drive Motor Count: ", NumToStr(DRIVE_MOTOR_COUNT), "\n"));
    log_printSerial(StrCat("Speed range: ", NumToStr(MIN_SPEED), " - ", NumToStr(MAX_SPEED), "\n"));

    for(unsigned int i = 0; i < DRIVE_MOTOR_COUNT; i++) {
        lastTachoVal[i] = 0;
        tachoVal[i] = 0;
    }

}


void hillSpeedControl_internal_updateTachoVal() {

    for(unsigned int i = 0; i < DRIVE_MOTOR_COUNT; i++) {
        lastTachoVal[i] = tachoVal[i];
        byte motor = motors[i];
        tachoVal[i] = MotorBlockTachoCount(motor);
    }

}


void hillSpeedControl_update() {

    hillSpeedControl_internal_updateTachoVal();

}



#endif
