#ifndef _LOST_RECOVERY_H_
#define _LOST_RECOVERY_H_

#include "Peripheral.h"
#include "Settings.h"
#include "Pid_Control.h"

// --- CONFIGURATION ---
#define LOST_COUNT_MAX 25 // TODO adjust lost count threshold
#define FORWARD_BACKWARD_WAIT_MS 200
#define MAX_RECOVERY_MULTIPLIER 5 // Maximum multiplier for forward/backward

const int FORWARD_POWER = 50; // TODO adjust forward Power
const int BACKWARD_POWER = -50;// TODO adjust backward Power
const int spinPower = 50;       // TODO adjust spin Power
const int lineThreshold = 30;   // TODO adjust line detection threshold

// Spin duration for small/medium/large turns
const int SPIN_TIMES[3] = {100, 150, 250}; // TODO adjust spin durations

// --- LOST DETECTION ---
int lostCounter = 0;
bool lost = false;

// --- RECOVERY STATE ---
int recoveryStep = 0;
bool recoveryDirRight = true;
int recoveryMultiplier = 1; // progressive multiplier for forward/backward

// --- FUNCTION DECLARATIONS ---
bool lostRecovery_isLost(int leftRaw, int rightRaw, int middleRaw);
bool lostRecovery_isLineVisible(int left, int right, int middle);
void lostRecovery_handleRecovery();
void lostRecovery_reset();

// --- FUNCTION IMPLEMENTATIONS ---
bool lostRecovery_isLineVisible(int left, int right, int middle) {
    return (left < lineThreshold) || (right < lineThreshold) || (middle < lineThreshold);
}

bool lostRecovery_isLost(int leftRaw, int rightRaw, int middleRaw) {
    static bool soundPlayed = false;

    bool leftOnLine = (leftRaw < lineThreshold);
    bool rightOnLine = (rightRaw < lineThreshold);
    bool middleOnLine = (middleRaw < lineThreshold);

    if (!leftOnLine && !rightOnLine && !middleOnLine) {
        if (!soundPlayed) {
            PlayTone(1000, 200); // play once per lost event
            soundPlayed = true;
        }
        lostCounter++;
    } else {
        lostCounter = 0;
        soundPlayed = false;
    }

    lost = (lostCounter > LOST_COUNT_MAX);
    return lost;
}

void lostRecovery_reset() {
    lost = false;
    lostCounter = 0;
    recoveryStep = 0;
    recoveryMultiplier = 1; // reset progressive multiplier
}

void lostRecovery_handleRecovery() {
    // reset PID
    integral = 0.0;
    derivative = 0.0;

    // --- Repeated beep during recovery ---
    static long lastToneTime = 0;
    long currentTime = CurrentTick();
    if (currentTime - lastToneTime > 500) {
        PlayTone(1000, 100);
        lastToneTime = currentTime;
    }

    // --- Display Lost-Counter ---
    ClearScreen();
    TextOut(0, 0, "Lost Counter:");
    NumOut(0, 1, lostCounter);

    // --- Small/Medium/Large spins ---
    if (recoveryStep <= 2) {
        int spinTime = SPIN_TIMES[recoveryStep];

        if (recoveryDirRight) {
            applyMotorPower(LEFT_MOTOR, spinPower);
            applyMotorPower(RIGHT_MOTOR, -spinPower);
        } else {
            applyMotorPower(LEFT_MOTOR, -spinPower);
            applyMotorPower(RIGHT_MOTOR, spinPower);
        }

        Wait(spinTime);
        recoveryStep++;
        return;
    }

    // --- Forward ---
    if (recoveryStep == 3) {
        int forwardTime = FORWARD_BACKWARD_WAIT_MS * recoveryMultiplier;
        applyMotorPower(LEFT_MOTOR, FORWARD_POWER);
        applyMotorPower(RIGHT_MOTOR, FORWARD_POWER);
        Wait(forwardTime);
        recoveryStep++;
        return;
    }

    // --- Backward + change direction ---
    if (recoveryStep == 4) {
        int backwardTime = FORWARD_BACKWARD_WAIT_MS * recoveryMultiplier;
        applyMotorPower(LEFT_MOTOR, BACKWARD_POWER);
        applyMotorPower(RIGHT_MOTOR, BACKWARD_POWER);
        Wait(backwardTime);

        // Increase multiplier for next cycle, but cap it
        recoveryMultiplier++;
        if (recoveryMultiplier > MAX_RECOVERY_MULTIPLIER) recoveryMultiplier = MAX_RECOVERY_MULTIPLIER;

        recoveryDirRight = !recoveryDirRight;
        recoveryStep = 0;
        return;
    }

    // --- Check sensors ---
    int l = SensorValue(LEFT_SENSOR);
    int r = SensorValue(RIGHT_SENSOR);
    int m = SensorValue(BARCODE_SENSOR);

    if (lostRecovery_isLineVisible(l, r, m)) {
        lostRecovery_reset();
        lastToneTime = 0; // reset beep timer
    }
}

#endif
