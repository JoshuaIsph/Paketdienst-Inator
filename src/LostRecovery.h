#ifndef _LOST_RECOVERY_H_
#define _LOST_RECOVERY_H_

#include "Peripheral.h"
#include "Settings.h"
#include "Pid_Control.h"

// --- CONFIGURATION ---
#define LOST_COUNT_MAX 5 
#define FORWARD_BACKWARD_WAIT_MS 200 // Base time for movement
#define MAX_RECOVERY_MULTIPLIER 20 

const int FORWARD_POWER = 50; 
const int BACKWARD_POWER = -50;
const int spinPower = 100;       
const int lineThreshold = 27;   
const int SPIN_TIMES[] = {200, 300, 400}; // Small, Medium, Large spins

// --- VARIABLES ---
int lostCounter = 0;
bool lost = false;
int recoveryMultiplier = 1; 

// --- DECLARATIONS ---
/*
bool lostRecovery_isLost(int l, int r, int m);
bool lostRecovery_isLineVisible(int l, int r, int m);
void lostRecovery_reset();
bool checkLineOrWait(int duration); 
bool attemptMove(int leftPwr, int rightPwr, int duration);
bool performWiggleRoutine(int duration);
void lostRecovery_handleRecovery();
*/

// =========================================================
// 1. SENSOR & STATE HELPERS
// =========================================================

bool lostRecovery_isLineVisible(int left, int right, int middle) {
    // If ANY sensor sees black (value < threshold), line is visible
    return (left < lineThreshold) || (right < lineThreshold) || (middle < lineThreshold);
}

bool lostRecovery_isLost(int leftRaw, int rightRaw, int middleRaw) {
    
    // If NO sensors see the line
    if (leftRaw >= lineThreshold && rightRaw >= lineThreshold && middleRaw >= lineThreshold) {
        lostCounter++;
    } else {
        lostCounter = 0; 
    }
    
    lost = (lostCounter > LOST_COUNT_MAX);
    return lost;
}

void lostRecovery_reset() {
    lost = false;
    lostCounter = 0;
    recoveryMultiplier = 1; // Reset multiplier for next time
    
    // Stop motors immediately
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);
}

// =========================================================
// 2. MOVEMENT HELPERS (Smart Wait)
// =========================================================

// "Smart Wait": Moves for 'duration' ms, but checks sensors constantly.
// Returns TRUE if line is found (and resets state).
// Returns FALSE if time expires.
bool checkLineOrWait(int duration) {
    long startTime = CurrentTick();
    while (CurrentTick() - startTime < duration) {
        // Check sensors
        if (lostRecovery_isLineVisible(SensorValue(LEFT_SENSOR), SensorValue(RIGHT_SENSOR), SensorValue(BARCODE_SENSOR))) {
            lostRecovery_reset(); // Reset everything
            return true;          // Signal SUCCESS
        }
    }
    return false; // Signal TIMEOUT (keep searching)
}

// Wrapper to set motors and call the smart wait
bool attemptMove(int leftPwr, int rightPwr, int duration) {
    
    applyMotorPower(LEFT_MOTOR, leftPwr);
    applyMotorPower(RIGHT_MOTOR, rightPwr);
    //Wait(1000);
    if (lostRecovery_isLineVisible(SensorValue(LEFT_SENSOR), SensorValue(RIGHT_SENSOR), SensorValue(BARCODE_SENSOR))) {
            lostRecovery_reset(); // Reset everything
            return true;          // Signal SUCCESS
        }
        
    return checkLineOrWait(duration*5);
}

// Performs: Right -> Center -> Left -> Center
bool performWiggleRoutine(int duration) {
    // 1. Look Right

    log_println(StrCat("Wiggle: ", NumToStr(duration)));

    if (attemptMove(spinPower, -spinPower, duration)) return true;
    
    // 2. Return Center (Left)
    if (attemptMove(-spinPower, spinPower, duration)) return true;
    // 3. Look Left
    if (attemptMove(-spinPower, spinPower, duration)) return true;
    // 4. Return Center (Right)
    if (attemptMove(spinPower, -spinPower, duration)) return true;
    
    return false; // Completed wiggle without finding line
}

// =========================================================
// 3. MAIN RECOVERY LOOP
// =========================================================

void lostRecovery_handleRecovery() {
    // Safety: Stop motors before starting logic
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);

    // Reset PID terms so they don't interfere later
    integral = 0.0; 
    derivative = 0.0;
    
    // Start with multiplier 1
    recoveryMultiplier = 1;

    // --- THE INFINITE SEARCH LOOP ---
    // This loop runs forever until the line is found.
    while (true) {
        
        // Beep to indicate active recovery
        

        // Display Info
        ClearScreen();
        TextOut(0, 0, "SEARCHING...");
        TextOut(0, 1, "Multiplier:");
        NumOut(60, 1, recoveryMultiplier);

        // Calculate Timings for this cycle
        int timeIndex = (recoveryMultiplier - 1); 
        if (timeIndex > 2) timeIndex = 2; // Cap spin speed at max index
        
        int spinTime = SPIN_TIMES[timeIndex];
        int moveTime = FORWARD_BACKWARD_WAIT_MS * recoveryMultiplier;

        log_println(StrCat("TIndex: ", NumToStr(timeIndex)));
        log_println(StrCat("STime: ", NumToStr(spinTime)));

        // --- SEQUENCE STEP 1: WIGGLE (At Current Spot) ---
        if (performWiggleRoutine(spinTime)) return; // Return if found

        // --- SEQUENCE STEP 2: FORWARD ---
        if (attemptMove(FORWARD_POWER, FORWARD_POWER, moveTime)) return;

        // --- SEQUENCE STEP 3: WIGGLE (At Forward Spot) ---
        if (performWiggleRoutine(spinTime)) return;

        // --- SEQUENCE STEP 4: BACKWARD (Return + Search Behind) ---
        // We multiply moveTime by 2 to go back past the start point
        if (attemptMove(BACKWARD_POWER, BACKWARD_POWER, moveTime * 2)) return;

        // --- SEQUENCE STEP 5: WIGGLE (At Backward Spot) ---
        if (performWiggleRoutine(spinTime)) return;

        // --- CYCLE FAILED ---
        // If we reach here, the robot did the whole cross pattern and found nothing.
        // Increase the multiplier to search a wider area in the next loop.
        recoveryMultiplier++;
        if (recoveryMultiplier > MAX_RECOVERY_MULTIPLIER) {
             recoveryMultiplier = MAX_RECOVERY_MULTIPLIER;
        }
    }
}

#endif
