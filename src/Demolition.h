#ifndef _DEMOLITION_H_
#define _DEMOLITION_H_

#include "Peripheral.h"
#include "Settings.h"
#include "HardwareControl.h"
#include "Maneuver.h"
#include "Logger.h"


#define DEMOLITION_DISTANCE 10
#define DEMOLITION_POWER 60
#define RELATIVE_THRESHOLD 10
#define DEMOLITION_REVERSE_TIMEOUT 200
#define DEMOLITION_RETURN_FORWARD_TIME 500
#define DEMOLITION_RETURN_BACKWARD_TIME 500
#define DEMOLITION_RETURN_GAP_TIME 500


bool demolition_modeEnable = true;


/**
 * Enables/Disables demolition mode.
 * 
 * @param enable True to enable demolition. False to disable
 */
void demolition_enable(bool enable) {
    demolition_modeEnable = enable;
}


void demolition_turn() {

    OnRevSync(LR_MOTOR, TRAVEL_SPEED, 0);

    Wait(DEMOLITION_RETURN_BACKWARD_TIME);

    //maneuver_rotateUntilLine(RIGHT);

    OnFwdSync(LR_MOTOR, TRAVEL_SPEED, -100);

    Wait(1400);

    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);
}


/**
 * Find way back to line...
 */
void returnToPath() {

    OnRevSync(LR_MOTOR, DEMOLITION_POWER, 0);


    int left = 0;
    int middle = 0;
    int right = 0;

    int time = 0;

    while(time < DEMOLITION_REVERSE_TIMEOUT) {

        left = Sensor(LEFT_SENSOR);
        middle = Sensor(MIDDLE_SENSOR);
        right = Sensor(RIGHT_SENSOR);

        if(!insideThreshold(left, right, RELATIVE_THRESHOLD)) {
            break;
        } else if(!insideThreshold(left, middle, RELATIVE_THRESHOLD)) {
            break;
        } else if(!insideThreshold(middle, right, RELATIVE_THRESHOLD)) {
            break;
        }

        time++;

        Wait(10);
    }

    applyMotorPower(LEFT_MOTOR, MIN_SPEED);
    applyMotorPower(RIGHT_MOTOR, MIN_SPEED);

    Wait(DEMOLITION_RETURN_FORWARD_TIME);

    maneuver_rotateUntilLine();

}


/**
 * On its mission to remove any obstacle.
 * 
 * @param distance Current read distance of obstacle
 * @returns True if mission fulfilled
 */
bool demolition_attack(int distance) {


    if(distance < DEMOLITION_DISTANCE) {

        log_println("Wall found");

        OnFwdSync(LR_MOTOR, DEMOLITION_POWER, 0);

        while(SensorUS(ULTRA_SONIC_SENSOR) < DEMOLITION_DISTANCE) {
            Wait(10);
        }

        log_println("Wall pushed");

        returnToPath();

        log_println("Return");

        return true;
    }

    return false;
}


/**
 * Find way back to main path...
 */
bool demolition_return(int lightLeft, int lightMiddle, int lightRight) {

    int light_avg = (lightLeft + lightRight) / 2;

    if(insideThreshold(lightMiddle, light_avg, RELATIVE_THRESHOLD)) {
        // End of track

        log_println("End of path");

        Wait(DEMOLITION_RETURN_GAP_TIME);
        maneuver_rotateUntilLine(RIGHT);
        return true;
    }

    return false;
}


#endif
// _DEMOLITION_H_