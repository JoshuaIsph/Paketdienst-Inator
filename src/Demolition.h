#ifndef _DEMOLITION_H_
#define _DEMOLITION_H_

#include "Peripheral.h"
#include "Settings.h"
#include "HardwareControl.h"
#include "Maneuver.h"
#include "Logger.h"
#include "LostRecovery.h"


#define DEMOLITION_DISTANCE 10
#define DEMOLITION_REVERSE_TIMEOUT 200
#define DEMOLITION_RETURN_FORWARD_TIME 500
#define DEMOLITION_RETURN_BACKWARD_TIME 800
#define DEMOLITION_RETURN_GAP_TIME 140

#define RELATIVE_THRESHOLD_DEMOLITION 10
#define DEMO_SMOOTHING 5


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

    maneuver_rotateUntilLine(RIGHT, false);

    //OnFwdSync(LR_MOTOR, TRAVEL_SPEED, -100);

    //Wait(1400);

    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);
}


/**
 * Find way back to line...
 */
void returnToPath() {

    OnRevSync(LR_MOTOR, MAX_SPEED, 0);


    int left = 0;
    int middle = 0;
    int right = 0;

    int time = 0;

    while(time < DEMOLITION_REVERSE_TIMEOUT) {

        left = Sensor(LEFT_SENSOR);
        middle = Sensor(MIDDLE_SENSOR);
        right = Sensor(RIGHT_SENSOR);

        if(!insideThreshold(left, right, RELATIVE_THRESHOLD_DEMOLITION)) {
            break;
        } else if(!insideThreshold(left, middle, RELATIVE_THRESHOLD_DEMOLITION)) {
            break;
        } else if(!insideThreshold(middle, right, RELATIVE_THRESHOLD_DEMOLITION)) {
            break;
        }

        time++;

        Wait(10);
    }

    //applyMotorPower(LEFT_MOTOR, MIN_SPEED);
    //applyMotorPower(RIGHT_MOTOR, MIN_SPEED);

    OnFwdSync(LR_MOTOR, MIN_SPEED, 0);

    Wait(DEMOLITION_RETURN_FORWARD_TIME);

    maneuver_rotateUntilLine();

}


/**
 * On the way to remove any obstacle on its way.
 * 
 * @param lightLeft Current left light sensor value
 * @param middleLeft Current middle light sensor value
 * @param rightLeft Current right light sensor value
 * 
 * @returns True if mission fulfilled
 */
bool demolition_attackS(int lightLeft, int lightMiddle, int lightRight) {

    static int relativeThreshold = 10;
    static bool pushing = false;

    int highest = lightLeft;
    if(lightRight > highest) {
        highest = lightRight;
    }

    bool inside = insideThreshold(lightMiddle, highest, relativeThreshold) || lightMiddle > highest;

    log_printSerial(NumToStr(lightMiddle));

    if(inside) {
        log_printSerial("Touch ");
    }

    if(!pushing && inside) {
        log_printSerial("Push block");
        log_playNotifySound();
        pushing = true;
    }
    if(pushing && !inside) {
        log_printSerial("Block gone!");
        pushing = false;
        log_playNotifySound();
        returnToPath();

        return true;
    }

    return false;
}


int sampleBuffer[DEMO_SMOOTHING];
int demo_index = 0;

void add(int sample) {

    sampleBuffer[demo_index] = sample;

    demo_index++;

    if(demo_index >= DEMO_SMOOTHING) {
        demo_index = 0;
    }
}

int get() {

    int res = 0;

    for(unsigned int i = 0; i < DEMO_SMOOTHING; i++) {
        res += sampleBuffer[i];
    }

    return res / DEMO_SMOOTHING;
}

void demolition_init() {

    for(unsigned int i = 0; i < DEMO_SMOOTHING; i++) {
        sampleBuffer[i] = 0;
    }

}

/**
 * On the way to remove any obstacle on its way.
 * 
 * @param lightLeft Current left light sensor value
 * @param middleLeft Current middle light sensor value
 * @param rightLeft Current right light sensor value
 * 
 * @returns True if mission fulfilled
 */
bool demolition_attackML(int lightMiddle) {

    static int lastLight = 0;
    static int relativeThreshold = 5;

    int avg = get();

    //log_printSerial(NumToStr(lightMiddle - avg));

    if(!insideThreshold(lightMiddle, avg, RELATIVE_THRESHOLD)) {

        if(lightMiddle < avg) {
            log_printSerial("Block gone!");
            log_playStatusSound();
            returnToPath();

            return true;
        }

    }


    lastLight = lightMiddle;
    add(lightMiddle);

    return false;
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

        OnFwdSync(LR_MOTOR, MAX_SPEED, 0);

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
        maneuver_rotateUntilLine(RIGHT, false);
        
        OnFwdSync(LR_MOTOR, TRAVEL_SPEED, 0);
        Wait(200);
        lostRecovery_reset();
        return true;
    }

    return false;
}


#endif
// _DEMOLITION_H_