#ifndef _WALL_PUSH_MISSION_H_
#define _WALL_PUSH_MISSION_H_

#include "Peripheral.h"
#include "Settings.h"
#include "HardwareControl.h"
#include "Pid_Control.h"


#define BASE_SPEED 30
#define GAP_LOOP_DELAY_MS 20
#define GAP_TOLERANCE_MS 200        // allows gap spacing difference
#define MIN_GAP_MS 80               // ignore tiny gaps to prevent false triggering
#define MAX_DETECT_TIME_MS 30000    // timeout for initial detection
#define TURN_90_MS 420              // timed 90° turn (timing needs to be checked)
#define TURN_180_MS (TURN_90_MS*2)
#define WALL_DETECT_CM 35           // when to stop following and approach wall
#define WALL_PUSH_POWER 40
#define WALL_GONE_CM 60             // consider wall moved when ultrasound reads greater        


void lineFollowStep(int baseSpeed) {
    int l = SensorValue(LEFT_SENSOR);
    int r = SensorValue(RIGHT_SENSOR);

    applyMotorPower(LEFT_MOTOR,  baseSpeed);
    applyMotorPower(RIGHT_MOTOR, baseSpeed);
}

// Measure a single gap (returns duration in ms, 0 if no gap found before timeout)
int measureGap(int maxWaitMs) {
    int elapsed = 0;
    // wait for gap start: all sensors off the line (white)
    while (elapsed < maxWaitMs) {
        int l = SensorValue(LEFT_SENSOR);
        int r = SensorValue(RIGHT_SENSOR);
        int m = SensorValue(BARCODE_SENSOR);
        int lineThreshold = 27;
        if (l > lineThreshold && r > lineThreshold && m > lineThreshold) break;
        lineFollowStep(BASE_SPEED);
        Wait(GAP_LOOP_DELAY_MS);
        elapsed += GAP_LOOP_DELAY_MS;
    }


    // gap started: measure until line is seen again
    int gapTime = 0;
    while (true) {
        int l = SensorValue(LEFT_SENSOR);
        int r = SensorValue(RIGHT_SENSOR);
        int m = SensorValue(BARCODE_SENSOR);
        int lineThreshold = 27;
        if (l < lineThreshold || r < lineThreshold || m < lineThreshold) break;
        Wait(GAP_LOOP_DELAY_MS);
        gapTime += GAP_LOOP_DELAY_MS;
        if (gapTime > maxWaitMs){
            break;
        }
    }
    return gapTime;
}

// Detect two equally-sized gaps while continuing to follow the line.
// Returns true when two matching gaps found (gap sizes returned in gap1/gap2).
bool detectTwoEqualGaps(int &gap1, int &gap2, int timeoutMs = MAX_DETECT_TIME_MS) {
    int elapsed = 0;
    gap1 = gap2 = 0;
    while (elapsed < timeoutMs) {
        // Normal line follow while searching for the first gap
        lineFollowStep(BASE_SPEED);
        Wait(GAP_LOOP_DELAY_MS);
        elapsed += GAP_LOOP_DELAY_MS;

        int maybeGap1 = measureGap(timeoutMs - elapsed);
        if (maybeGap1 >= MIN_GAP_MS) {
            gap1 = maybeGap1;
            elapsed += maybeGap1;

            // continue line following until next gap
            int maybeGap2 = measureGap(timeoutMs - elapsed);
            if (maybeGap2 >= MIN_GAP_MS) {
                gap2 = maybeGap2;
                // Compare sizes
                int diff = abs(gap1 - gap2);
                int bigger = (gap1 > gap2) ? gap1 : gap2;
                if (diff <= GAP_TOLERANCE_MS) {
                    return true;
                } else {
                    // not equal enough: continue searching forward (skip ahead)
                    gap1 = 0; gap2 = 0;
                }
            } else {
                // timed out before second gap, continue searching
            }
        }
    }
    return false;
}

// Timed right turn (adjust TURN_90_MS for your robot or replace with gyro-based rotation)
void turnRight90() {
    applyMotorPower(LEFT_MOTOR,  40);
    applyMotorPower(RIGHT_MOTOR, -40);
    Wait(TURN_90_MS);
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR,0);
    Wait(50);
}

void turn180Degrees() {
    applyMotorPower(LEFT_MOTOR,  40);
    applyMotorPower(RIGHT_MOTOR, -40);
    Wait(TURN_180_MS);
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR,0);
    Wait(50);
}

// Follow the line until ultrasonic detects wall within WALL_DETECT_CM or until the timeout threshold is reached
bool followLineUntilWall(int timeoutMs) {
    int elapsed = 0;
    while (elapsed < timeoutMs) {
        int ultra_sonic = SensorValue(ULTRA_SONIC_SENSOR);
        if (ultra_sonic > 0 && ultra_sonic <= WALL_DETECT_CM) { //this checks if the wall is detected
            applyMotorPower(LEFT_MOTOR, 0);
            applyMotorPower(RIGHT_MOTOR,0);
            return true;
        }
        lineFollowStep(BASE_SPEED);
        Wait(GAP_LOOP_DELAY_MS);
        elapsed += GAP_LOOP_DELAY_MS;
    }
    return false;
}

// Drive forward pushing until ultrasonic sensor no longer detects the wall
void pushWallUntilGone() {
    // approach into contact
    applyMotorPower(LEFT_MOTOR, BASE_SPEED/2);
    applyMotorPower(RIGHT_MOTOR, BASE_SPEED/2);
    Wait(300);
    // push
    while (true) {
        int ultra_sonic = SensorValue(ULTRA_SONIC_SENSOR);
        if (ultra_sonic > 0 && ultra_sonic >= WALL_GONE_CM) break; // wall moved
        if (ultra_sonic == 0) {
            // no reading -> give a short retreat and re-try a bit then stop if persists
            applyMotorPower(LEFT_MOTOR, -20);
            applyMotorPower(RIGHT_MOTOR,-20);
            Wait(150);
            break;
        }
        applyMotorPower(LEFT_MOTOR,  WALL_PUSH_POWER);
        applyMotorPower(RIGHT_MOTOR, WALL_PUSH_POWER);
        Wait(100);
    }
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR,0);
    Wait(100);
}

// This function blocks until complete (or times out internally).
void executeWallPushMission() {
    int g1, g2;
    // 1) wait until two equally-sized gaps are detected
    bool found = detectTwoEqualGaps(g1, g2);
    if (!found) return; // timed out or failed

    // 2) turn right and follow that branch of the line
    turnRight90();
    bool wallFound = followLineUntilWall(20000);
    if (!wallFound) {
        // nothing to push, try to return to original location
        turn180Degrees();
        // follow back until same two gaps are seen
        detectTwoEqualGaps(g1, g2, 20000);
        return;
    }

    // drive into and push the wall until it's gone
    pushWallUntilGone();

    // turn around and return to the gaps
    turn180Degrees();
    detectTwoEqualGaps(g1, g2, 30000);

    // stop motors since control will be handed back oveer to the main program
    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);
}

#endif
