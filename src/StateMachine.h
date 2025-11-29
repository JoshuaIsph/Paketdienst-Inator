#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_


#include "Pid_Control.h"
#include "HillSpeedControl.h"
#include "FinishLineDetection.h"


enum State {
    START,
    RUNNING,
    BRICK,
    FINISH
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
bool stateMachine_update(int lightLeft, int lightMiddle, int lightRight, int wallDistance) {



    switch(currentState) {

        case START:{
            // Approach wall and turn around

            currentState = RUNNING;
        }break;

        case RUNNING:{
            // Do nothing special
            // Normal movement

            // Detect finish line
            if(detection_finishLineReached(lightLeft, lightMiddle, lightRight, wallDistance)) {
                currentState = FINISH;
                hillSpeedControl_enable(false);
            }
        }break;

        case BRICK:{
            // Kick brick from table
        }break;

        case FINISH:{
            // Slowly approach basket and deliver package
            
            return true; // Placeholder
        }

    }


    return false;
}


#endif
