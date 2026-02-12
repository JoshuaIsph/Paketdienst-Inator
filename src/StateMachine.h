#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include "Settings.h"
#include "Logger.h"
#include "Pid_Control.h"
#include "HillSpeedControl.h"
#include "FinishLineDetection.h"
#include "Basket.h"
#include "Maneuver.h"
#include "BarcodeReader.h"
#include "Demolition.h"
#include "BarrierHandler.h"

#define BASKET_VALIDATION_TIMEOUT 10    // Validation timeout to find finish line
#define START_WALL_DISTANCE 15          // Distance of wall to starting line in cm. Rotate if this distance is reached


/**
 * States of state machine.
 */
enum State {
    START,
    RUNNING,
    BRICK,
    RETURN_PATH,
    FINISH,
    BASKET,
    HOLD
};


State currentState; // Current state of state machine


/**
 * Initialize state machine.
 * Actually just setting current state to START.
 */
void stateMachine_init() {
    currentState = START;
    
    basket_init();
    barcodeReader_init();
    demolition_init();
    barrierHandler_init();

    /*
    //---DELETE----------------------
    currentState = BRICK;
    lostRecovery_enable(false);
    //hillSpeedControl_enable(false);
    //-------------------------------
    */

    //currentState = RUNNING;
}


/**
 * Update state machine and returns false if robot should continue the work.
 * If robot should stop working, return value is true.
 * Some subroutines can keep the thread in this function until completion.
 * 
 * @param lightLeft current raw value of left light sensor
 * @param lightMiddle current raw value of middle light sensor
 * @param lightRight current raw value of right light sensor
 * @param wallDistance current distance read by ultra sonic sensor
 * @param leftTacho current tacho value of left motor
 * @param rightTacho current tacho value of right motor
 * 
 * @returns true if robot should stop working. false if robot should continue work
 */
bool stateMachine_update(int lightLeft, int lightMiddle, int lightRight, int wallDistance, int leftTacho, int rightTacho) {



    switch(currentState) {

        case START:{
            // Approach wall and turn around

            if(wallDistance < START_WALL_DISTANCE) {

                maneuver_rotateUntilLine();
                
                currentState = RUNNING;
                log_playStatusSound();    
            }

        }break;

        case RUNNING:{
            // Do nothing special
            // Normal movement

            // Detect finish line
            if(detection_finishLineReached(lightLeft, lightMiddle, lightRight, wallDistance)) {
                currentState = FINISH;
                hillSpeedControl_enable(false);
                lostRecovery_enable(false);
                pid_setBaseSpeed(MIN_SPEED);
                log_playStatusSound();
            }

            barrierHandler_detectAndHandleBarrier(wallDistance);

            // Scan barcode
            BarcodeCommand command = barcodeReader_read(lightLeft, lightRight, lightMiddle);
            if(command == PUSH_BLOCK) {
                barcodeScanner_setEnable(false); // Only push once
                currentState = BRICK;
                lostRecovery_enable(false); // Disable for push mission
                

                demolition_turn();

                log_println("PUSH BLOCK");
                log_playNotifySound();
                Wait(5);
                log_playNotifySound();
                Wait(5);
                log_playNotifySound();

            }

        }break;

        case BRICK:{
            // Kick brick from table
            if(demolition_attackML(lightMiddle)) {
                currentState = RETURN_PATH;
            }

        }break;

        case RETURN_PATH:{

            if(demolition_return(lightLeft, lightMiddle, lightRight)) {

                lostRecovery_enable(true); // Enable for original mission
                currentState = RUNNING;
            }

        }break;

        case FINISH:{
            // Slowly approach basket and deliver package

            static int lastDistance = 0;
            static int validationTimer = 0;

            if(lastDistance == wallDistance && leftTacho <= 0 && rightTacho <= 0) {
                validationTimer++;
            } else {
                validationTimer = 0;
            }

            if(validationTimer > BASKET_VALIDATION_TIMEOUT) {
                currentState = BASKET;
            }

            lastDistance = wallDistance;
        }break;

        case BASKET: {
            applyMotorPower(LEFT_MOTOR, 0);
            applyMotorPower(RIGHT_MOTOR, 0);
            basket_empty();
            currentState = HOLD;
            return true;
        }

        case HOLD: {
            // Stop process fallback layer
            return true;
        }

    }


    return false;
}


#endif
// _STATE_MACHINE_H_