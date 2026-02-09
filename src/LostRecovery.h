#ifndef _LOST_RECOVERY_H_
#define _LOST_RECOVERY_H_

#include "Utils.h"
#include "Peripheral.h"
#include "Settings.h"
#include "Pid_Control.h"
#include "Maneuver.h"

// --- CONFIGURATION ---
#define REVERSE_PATH_FINDER_TIMEOUT 3000    // Time to reverse
#define LOST_COUNT_MAX 11                   // Time until lost recovery triggers
#define FORWARD_BACKWARD_WAIT_MS 200        // Base time for movement
#define MAX_RECOVERY_MULTIPLIER 20          // Max intensyfier
//#define RELATIVE_THRESHOLD 8               // Threshold between light sensors

const int FORWARD_POWER = 50;
const int BACKWARD_POWER = -50;
const int spinPower = 100;
const int SPIN_TIMES[] = {200, 300, 400}; // Small, Medium, Large spins

// --- LOST DETECTION ---
bool lostRecoveryEnable = true;
int lostCounter = 0;
bool lost = false;
int recoveryMultiplier = 1;

// --- Memory ---
Direction lastDirection;


// =========================================================
// 1. SENSOR & STATE HELPERS
// =========================================================

/**
 * Determines the visibility of the line.
 * 
 * @param left Raw light value of left sensor
 * @param right Raw light value of right sensor
 * @param middle Raw light value of middle sensor
 * 
 * @returns true if line is visible
 */
bool lostRecovery_isLineVisible(int left, int right, int middle) {
    // If ANY sensor sees black (value < threshold), line is visible

    bool visible = false;

    int avg = (left + right) / 2;

    if(insideThreshold(left, right, RELATIVE_THRESHOLD)) {

        if(!insideThreshold(middle, avg, RELATIVE_THRESHOLD)) {
            visible = true;
        }
    } else {
        visible = true;
    }

    return visible;
}


/**
 * Determines if robot lost line.
 * 
 * @param leftRaw Raw light value of left sensor
 * @param rightRaw Raw light value of right sensor
 * @param middleRaw Raw light value of middle sensor
 * 
 * @returns true if line was lost.
 * @returns always returns false if lost recovery is disabled
 */
bool lostRecovery_isLost(int leftRaw, int rightRaw, int middleRaw) {
    
    if(!lostRecoveryEnable) {
        return false;
    }

    // If NO sensors see the line
    if(!lostRecovery_isLineVisible(leftRaw, rightRaw, middleRaw)) {
        lostCounter++;
    } else {
        lostCounter = 0; 
    }

    if(!insideThreshold(leftRaw, rightRaw, RELATIVE_THRESHOLD)) {

        if(leftRaw < rightRaw) {
            lastDirection = LEFT;
        } else {
            lastDirection = RIGHT;
        }

    }
    
    lost = (lostCounter > LOST_COUNT_MAX);
    return lost;
}


/**
 * Resets routine variables and stops motors.
 */
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

/**
 * "Smart Wait": Moves for 'duration' ms, but checks sensors constantly.
 * Returns if line was found or time expires.
 * 
 * @returns true if line was found (and resets state).
 * @returns false if time expires.
 */
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


/**
 * Wrapper to set motors and call the smart wait.
 * Applies given energy to motors and wait until line was found or time expires.
 * 
 * @param leftPwr Power to apply to left motor while attempt
 * @param rightPwr Power to apply to right motor while attempt
 * @param duration Max duration of attempt.
 * 
 * @returns true if line was found
 */
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


/**
 * Performs: Left -> Center -> Right -> Center
 * 
 * @param duration Duration of wiggle routine
 * 
 * @returns true if line was found.
 * @returns false if line was not found.
 */
bool performWiggleRoutine(int duration) {
    // 1. Look Right

    log_println(StrCat("Wiggle: ", NumToStr(duration)));

    if (attemptMove(-spinPower, spinPower, duration)) return true;
    
    // 2. Return Center (Left)
    if (attemptMove(spinPower, -spinPower, duration)) return true;
    // 3. Look Left
    if (attemptMove(spinPower, -spinPower, duration)) return true;
    // 4. Return Center (Right)
    if (attemptMove(-spinPower, spinPower, duration)) return true;
    
    return false; // Completed wiggle without finding line
}


/**
 * Let robot reverse to find lost path.
 * Should find missed corners.
 * 
 * @returns true if line was found. false otherwise.
 */
bool performReversePathFinder() {

    log_playNotifySound();

    const int timeout = REVERSE_PATH_FINDER_TIMEOUT / LOOP_MS;
    int time = 0;

    log_println("Reverse path finding");

    OnRevSync(LR_MOTOR, TRAVEL_SPEED, 0);

    int left = 0;
    int right = 0;
    int middle = 0;

    while(time < timeout) {
        
        left = Sensor(LEFT_SENSOR);
        right = Sensor(RIGHT_SENSOR);
        middle = Sensor(MIDDLE_SENSOR);
        int avg = (left + right) / 2;
        
        if(!insideThreshold(left, right, RELATIVE_THRESHOLD) || !insideThreshold(avg, middle, RELATIVE_THRESHOLD)) {
            /*
            Direction direction;
            if(left < right) {
                direction = LEFT;
            } else {
                direction = RIGHT;
            }
            */

            maneuver_rotateUntilLine(lastDirection, false);

            return true;
        }
        
        time++;

        Wait(LOOP_MS);
    }

    return false;
}

// =========================================================
// 3. MAIN RECOVERY LOOP
// =========================================================

/**
 * Activate recovery routine.
 * Keeps current thread in routine until completed.
 */
void lostRecovery_handleRecovery() {

    if(!lostRecoveryEnable) {
        return;
    }

    // Safety: Stop motors before starting logic
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);

    // Reset PID terms so they don't interfere later
    integral = 0.0; 
    derivative = 0.0;
    
    // Start with multiplier 1
    recoveryMultiplier = 1;



    // --- Did I miss the line? ---
    if(performReversePathFinder()) {
        return;
    }


    // --- THE INFINITE SEARCH LOOP ---
    // This loop runs forever until the line is found.
    while (true) {
        
        // Beep to indicate active recovery
        

        // Log Info
        log_println("Searching...");
        log_println(StrCat("Multiplier: ", NumToStr(recoveryMultiplier)));
        

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
        if(recoveryMultiplier <= MAX_RECOVERY_MULTIPLIER) {
            recoveryMultiplier++;
        }
    }

}


/**
 * Enable or disable lost recovery handle
 * 
 * @param enable true to enable. false to disable
 */
void lostRecovery_enable(bool enable) {
    lostRecoveryEnable = enable;
}

#endif
// _LOST_RECOVERY_H_