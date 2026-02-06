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
#define DEMOLITION_RETURN_BACKWARD_TIME 1500
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

#define DEMOLITION_SMOOTHING 3

int samples[DEMOLITION_SMOOTHING];
int demo_index = 0;


void demo_add(int sample) {

    samples[demo_index] = sample;
    demo_index++;
    if(demo_index >= DEMOLITION_SMOOTHING) {
        demo_index = 0;
    }
}

int calcSmoothed() {

    int res = 0;

    for(unsigned int i = 0; i < DEMOLITION_SMOOTHING; i++) {
        res += samples[i];
    }

    return res / DEMOLITION_SMOOTHING;
}

int getSum() {

    int res = 0;

    for(unsigned int i = 0; i < DEMOLITION_SMOOTHING; i++) {
        res += samples[i];
    }

    return res;
}

int getMax() {

    int max = samples[0];

    for(unsigned int i = 1; i < DEMOLITION_SMOOTHING; i++) {
        if(samples[i] > max) {
            max = samples[i];
        }
    }

    return max;
}

void clear() {
    for(unsigned int i = 0; i < DEMOLITION_SMOOTHING; i++) {
        samples[i] = 0;
    }
}


bool demolition_attackT(int motorSpeedL, int motorSpeedR) {


    const int tachoThreshold = 9;
    const int speedThreshold = 2;
    const int timeout = 50;

    static int lastSpeed = 0;
    static int lastSmoothed = 0;
    static int time = 0;
    static int count = 0;

    static bool pushing = false;


    if(time < 2) {
        time++;
        return false;
    }


    int speed = (motorSpeedL + motorSpeedR) / 2;


    int roughDifference = speed - getMax();

    demo_add(speed - lastSpeed);
    int smoothed = calcSmoothed();


    log_printSerial(StrCat("T: ", NumToStr(smoothed)));

    if(!pushing && smoothed < lastSmoothed && smoothed < tachoThreshold) {
        time = 0;
        log_playAlarm();
        log_printSerial("Pushing");
        pushing = true;

    }

    if(pushing && smoothed > (tachoThreshold + 1)) {
        log_printSerial("Block gone!");
        log_playNotifySound();

        applyMotorPower(LEFT_MOTOR, 0);
        applyMotorPower(RIGHT_MOTOR, 0);

        pushing = false;

        while(true) {
            Wait(100);
        }

        return true;
    }


    lastSpeed = speed;
    lastSmoothed = smoothed;

    return false;
}


bool demolition_attackT2(int motorSpeedL, int motorSpeedR) {


    //const float maxDifference = 0.1;
    const float upperLimit = 1.2;
    const float lowerLimit = 0.9;
    const int tachoThreshold = 9;
    const int speedThreshold = 2;
    const int timeout = 50;

    static int lastSpeed = 0;
    static int lastSmoothed = 0;
    static int lastSum = 1;
    static int time = 0;
    static int count = 0;

    static bool pushing = false;


    if(time < 2) {
        time++;
        return false;
    }


    int speed = (motorSpeedL + motorSpeedR) / 2;

    if(count < DEMOLITION_SMOOTHING) {
        count++;
        demo_add(speed);
        return false;
    }
    count = 0;
    int sum = getSum();



    int roughDifference = speed - getMax();

    //demo_add(speed - lastSpeed);
    int smoothed = calcSmoothed();

    float dif = sum / lastSum;

    log_printSerial(StrCat("T: ", NumToStr(dif)));

    if(!pushing && dif < lowerLimit) {
        pushing = true;

        log_playAlarm();
        log_printSerial("Push");

        time = 0;
    }

    if(pushing && dif > upperLimit) {

        log_playNotifySound();

        applyMotorPower(LEFT_MOTOR, 0);
        applyMotorPower(RIGHT_MOTOR, 0);

        while(true) {
            Wait(100);
        }
    }

/*
    if(!pushing && smoothed < lastSmoothed && smoothed < tachoThreshold) {
        time = 0;
        log_playAlarm();
        log_printSerial("Pushing");
        pushing = true;

    }

    if(pushing && smoothed > (tachoThreshold + 1)) {
        log_printSerial("Block gone!");
        log_playNotifySound();

        applyMotorPower(LEFT_MOTOR, 0);
        applyMotorPower(RIGHT_MOTOR, 0);

        pushing = false;

        while(true) {
            Wait(100);
        }

        return true;
    }
*/

    lastSpeed = speed;
    lastSmoothed = smoothed;
    lastSum = sum;

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