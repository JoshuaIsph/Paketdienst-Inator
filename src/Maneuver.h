#ifndef _MANEUVER_H_
#define _MANEUVER_H_


#include "Peripheral.h"
#include "HardwareControl.h"
#include "Utils.h"


enum Direction {
    LEFT,
    RIGHT
};


#define DETECTION_THRESHOLD 10


// Rotate until line found
// By default rotates left
void maneuver_rotateUntilLine(const Direction direction = RIGHT) {

    log_println("Rotate until line");

    bool rotate = true;

    
    {
        const int speed = MAX_SPEED;

        int left = 0;
        int right = 0;

        if(direction == RIGHT) {
            left = speed;
            right = -speed;
        } else {
            left = -speed;
            right = speed;
        }
        
        applyMotorPower(LEFT_MOTOR, left);
        applyMotorPower(RIGHT_MOTOR, right);
    }


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

            int avg = (lightLeft + lightRight) / 2;

            if(lr_equal && !insideThreshold(avg, lightMiddle, DETECTION_THRESHOLD) && lightMiddle < avg
                || !lr_equal
                && (direction == RIGHT && lightLeft < lightRight || direction == LEFT && lightRight < lightLeft)) {
                
                log_playNotifySound();
                rotate = false;
            }

        } else {
            if(!lr_equal) {

                if(direction == RIGHT && lightRight < lightLeft || direction == LEFT && lightLeft < lightRight) {
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
