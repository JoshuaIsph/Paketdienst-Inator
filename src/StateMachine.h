#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include "Settings.h"
#include "Logger.h"
#include "Pid_Control.h"
#include "HillSpeedControl.h"
#include "FinishLineDetection.h"
#include "Basket.h"

#define BASKET_VALIDATION_TIMEOUT 10


enum State {
    START,
    RUNNING,
    BRICK,
    FINISH,
    BASKET,
    HOLD
};


State currentState;


void stateMachine_init() {
    currentState = START;
}

/*
* Update state machine and returns false if robot should continue the work.
* If robot should stop working, return value is true.
* Some subroutines can keep the thread in this function until completion.
*/
bool stateMachine_update(int lightLeft, int lightMiddle, int lightRight, int wallDistance, int leftTacho, int rightTacho) {



    switch(currentState) {

        case START:{
            // Approach wall and turn around

            currentState = RUNNING;
            log_playStatusSound();
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
        }break;

        case BRICK:{
            // Kick brick from table
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
