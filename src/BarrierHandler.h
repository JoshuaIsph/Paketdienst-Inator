#ifndef _BARRIER_HANDLER_H_
#define _BARRIER_HANDLER_H_

#include "Peripheral.h"
#include "HardwareControl.h"
#include "Logger.h"


#define BARRIER_STOP_DISTANCE 20    // CM
#define BARRIER_HANDLER_DELAY 100   // MS
#define SMOOTHING_SAMPLE_COUNT 2

bool handleBarrier = true;


/*
Because of inconsistent readings of ultra sonic sensor,
values will be smoothed over time.
*/
int barrierHandler_currentIndex = 0;
int barrierHandler_distanceSamples[SMOOTHING_SAMPLE_COUNT];


/**
 * Initializes barrier handler.
 */
void barrierHandler_init() {

    handleBarrier = true;

    for(unsigned int i = 0; i < SMOOTHING_SAMPLE_COUNT; i++) {
        barrierHandler_distanceSamples[i] = 255;
    }
    barrierHandler_currentIndex = 0;
}


/**
 * Adds sample to array.
 * Shifts start index instead of shifting all values.
 * 
 * @param distance Distance sample to add
 */
void barrierHandler_internal_addSample(int distance) {

    barrierHandler_distanceSamples[barrierHandler_currentIndex] = distance;

    barrierHandler_currentIndex++;

    if(barrierHandler_currentIndex >= SMOOTHING_SAMPLE_COUNT) {
        barrierHandler_currentIndex = 0;
    }

}

/**
 * Calculates average value of samples.
 * 
 * @returns Avg of samples
 */
int barrierHandler_internal_calcSmoothedDistance() {

    int avg = 0;

    for(unsigned int i = 0; i < SMOOTHING_SAMPLE_COUNT; i++) {
        avg += barrierHandler_distanceSamples[i];
    }

    return avg / SMOOTHING_SAMPLE_COUNT;
}


/**
 * Enables/Disables barrier handler.
 * 
 * @param enable Set true to stop at barrier
 */
void barrierHandler_enable(bool enable) {
    handleBarrier = enable;
}


/**
 * Detects barriers and stops until it clears the way.
 * Holds current thread until barrier clears the way.
 * 
 * @param distance Current read distance of Ultrasonic-Sensor
 */
void barrierHandler_detectAndHandleBarrier(int distance) {

    barrierHandler_internal_addSample(SensorUS(ULTRA_SONIC_SENSOR));
    int avgDistance = barrierHandler_internal_calcSmoothedDistance();

    if(!handleBarrier || avgDistance > BARRIER_STOP_DISTANCE) {
        return;
    }


    // BARRIER FOUND?!

    // Stop motors
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);

    // Notify
    log_playNotifySound();
    log_println("Barrier stop");

    // Wait
    while(avgDistance <= BARRIER_STOP_DISTANCE) {

        barrierHandler_internal_addSample(SensorUS(ULTRA_SONIC_SENSOR));
        avgDistance = barrierHandler_internal_calcSmoothedDistance();

        Wait(BARRIER_HANDLER_DELAY);
    }

    barrierHandler_enable(false); // Only handle once
    log_println("Barrier clear");
}


#undef SMOOTHING_SAMPLE_COUNT
#endif
// _BARRIER_HANDLER_H_