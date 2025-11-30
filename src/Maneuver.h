#ifndef _MANEUVER_H_
#define _MANEUVER_H_


#include "Peripheral.h"
#include "HardwareControl.h"
#include "Utils.h"


#define DETECTION_THRESHOLD 10


// Rotate right until line found
void maneuver_rotateUntilLine() {


    bool rotate = true;

    
    applyMotorPower(LEFT_MOTOR, MAX_SPEED);
    applyMotorPower(RIGHT_MOTOR, -MAX_SPEED);

    int lightLeft = 0;
    int lightRight = 0;
    int lightMiddle = 0;

    bool lineFound = false;

    while(rotate) {

        lightLeft = Sensor(LEFT_SENSOR);
        lightRight = Sensor(RIGHT_SENSOR);
        lightMiddle = Sensor(MIDDLE_SENSOR);

        bool lr_equal = insideThreshold(lightLeft, lightRight, DETECTION_THRESHOLD);

        if(lineFound) {

            if(lr_equal) {
                log_playNotifySound();
                int avg = (lightLeft + lightRight) / 2;

                if(!insideThreshold(avg, lightMiddle, DETECTION_THRESHOLD) && lightMiddle < avg) {
                    rotate = false;
                }

            }

        } else {
            if(!lr_equal) {

                if(lightRight < lightLeft) {
                    lineFound = true;
                    log_playStatusSound();
                }
            }
        }
        


        Wait(1);

    }

    applyMotorPower(LEFT_MOTOR, 0);
    applyMotorPower(RIGHT_MOTOR, 0);
}


#endif
