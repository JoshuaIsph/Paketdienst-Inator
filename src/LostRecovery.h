#ifndef _LOST_RECOVERY_H_
#define _LOST_RECOVERY_H_

#include "Peripheral.h"
#include "Settings.h"
#include "Pid_Control.h"


// --- CONFIGURATION ---
#define LOST_COUNT_MAX 25


// --- LOST DETECTION ---
int lostCounter = 0;
bool lost = false;


// --- RECOVERY STATE ---
int recoveryStep = 0;
int recoveryPower = 20;
bool recoveryDirRight = true;


int limitDelta(int prev, int target, int maxDelta) {
    int d = target - prev;
    if (d > maxDelta) d = maxDelta;
    if (d < -maxDelta) d = -maxDelta;
    return prev + d;
}

bool isLineVisible(int left, int right, int middle) {
    return (left < lineThreshold) || (right < lineThreshold) || (middle < lineThreshold);
}



void lostRecovery_handleLostRecovery() {
    integral = 0.0;
    derivative = 0.0;

    switch (recoveryStep) {
        case 0: recoveryPower = 20; break;
        case 1: recoveryPower = 35; break;
        case 2: recoveryPower = 50; break;
        
        case 3:{
        applyMotorPower(LEFT_MOTOR, 30);
        applyMotorPower(RIGHT_MOTOR, 30);
        Wait(200);
        recoveryStep++;
        return;
        }
        
        case 4: {
        applyMotorPower(LEFT_MOTOR, -30);
        applyMotorPower(RIGHT_MOTOR, -30);
        Wait(200);
        recoveryDirRight = !recoveryDirRight;
        recoveryStep = 0;
        return;
        }
    }

    if (recoveryDirRight) {
        applyMotorPower(LEFT_MOTOR,  recoveryPower);
        applyMotorPower(RIGHT_MOTOR, -recoveryPower);
    } else {
        applyMotorPower(LEFT_MOTOR, -recoveryPower);
        applyMotorPower(RIGHT_MOTOR,  recoveryPower);
    }

    Wait(150);
    recoveryStep++;

    int l = SensorValue(LEFT_SENSOR);
    int r = SensorValue(RIGHT_SENSOR);
    int m = SensorValue(BARCODE_SENSOR);

    if (isLineVisible(l, r, m)) {
        lost = false;
        lostCounter = 0;
        recoveryStep = 0;
    }
}


bool lostRecovery_isLost(int leftRaw, int rightRaw, int middleRaw) {
    // --- LOST DETECTION ---
    bool leftOn = (leftRaw < lineThreshold);
    bool rightOn = (rightRaw < lineThreshold);
    bool middleOn = (middleRaw < lineThreshold);

    if (!leftOn && !rightOn && !middleOn) lostCounter++;
    else lostCounter = 0;

    lost = lostCounter > LOST_COUNT_MAX;

    return lost;
    /*
    if (lost) {
      lostRecovery_handleLostRecovery();
      continue;
    }
    */
}


#endif