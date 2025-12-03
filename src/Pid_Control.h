
#ifndef _PID_CONTROL_H_
#define _PID_CONTROL_H_

#include "Settings.h"
#include "Utils.h"

// --- CONFIGURATION ---
#define ERROR_SCALE 100.0
#define MAX_DELTA_PER_LOOP 8

// --- PID GAINS ---
float kp = 11; // 4.75 //9.5
float ki = 0.0; // 0.00 -408.5 // -5 -50
float kd = 0.032; // 0.02
const float DERIV_TAU = 0.04;

// --- BASE SPEED ---
int base_speed = TRAVEL_SPEED; // 61
const int INTEGRAL_LIMIT = 30;

// --- GLOBAL PID STATE ---
float integral = 0.0;
float lastError = 0.0;
float derivative = 0.0;

// --- PREVIOUS POWER FOR SLEW ---
int prevLeftApplied = 0;
int prevRightApplied = 0;


// --- Results ---
int pid_leftPower = 0;
int pid_rightPower = 0;


void pid_init(int leftRaw, int rightRaw) {

    float initError = (leftRaw - rightRaw) / ERROR_SCALE;
    lastError = initError;
    
    derivative = 0.0;
    integral = 0.0;
}


int pid_getLeftPower() {
    return pid_leftPower;
}


int pid_getRightPower() {
    return pid_rightPower;
}


void pid_setBaseSpeed(int speed) {
    base_speed = speed;
}



int limitDelta(int prev, int target, int maxDelta) {
    int d = target - prev;
    if (d > maxDelta) d = maxDelta;
    if (d < -maxDelta) d = -maxDelta;
    return prev + d;
}

void pid_update(int leftRaw, int rightRaw) {

    float error = (leftRaw - rightRaw) / ERROR_SCALE;

    integral += error * update_time;
    if (integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
    if (integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;

    float rawD = (error - lastError) / update_time;
    float alpha = update_time / (DERIV_TAU + update_time);
    derivative = derivative * (1.0 - alpha) + rawD * alpha;

    float steering = kp * error + ki * integral + kd * derivative;
    int leftPower  = clampInt(base_speed + steering * POWER_RANGE, -POWER_RANGE, POWER_RANGE);
    int rightPower = clampInt(base_speed - steering * POWER_RANGE, -POWER_RANGE, POWER_RANGE);


    // simple anti-windup
    float unsatLeft = base_speed + steering * POWER_RANGE;
    float unsatRight = base_speed - steering * POWER_RANGE;
    if ((unsatLeft != leftPower) || (unsatRight != rightPower)) {
      integral -= error * update_time;
    }

    // --- SLEW LIMIT ---
    int appliedLeft = limitDelta(prevLeftApplied, leftPower, MAX_DELTA_PER_LOOP);
    int appliedRight = limitDelta(prevRightApplied, rightPower, MAX_DELTA_PER_LOOP);
    prevLeftApplied = appliedLeft;
    prevRightApplied = appliedRight;

    pid_leftPower = appliedLeft;
    pid_rightPower = appliedRight;

    lastError = error;
}

#endif
