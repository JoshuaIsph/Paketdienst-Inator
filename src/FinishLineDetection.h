#ifndef _FINISH_LINE_DETECTION_H_
#define _FINISH_LINE_DETECTION_H_


#include "Utils.h"

#define DETECTION_THRESHOLD 10          // Threshold for difference between light sensors
#define WALL_FINISHLINE_DISTANCE 17     // Distance to detect wall at finish line in cm
#define FINISHLINE_VALIDATE_TIMEOUT 20  // Timeout for detection if potential finishline was found



/**
 * Calculates if finishline was found with currently
 * and previously given values.
 * 
 * @param lightLeft Current raw value of left light sensor
 * @param lightMiddle Current raw value of middle light sensor
 * @param lightRight Current raw value of right light sensor
 * @param wallDistance Current distance to wall in cm
 * 
 * @returns true if finish line was detected
 */
bool detection_finishLineReached(int lightLeft, int lightMiddle, int lightRight, int wallDistance) {

    static int validateTime = 0;
    static bool validateLine = false;

    validateTime++;

    if(validateTime >= FINISHLINE_VALIDATE_TIMEOUT) {
        validateTime = 0;
        validateLine = false;
    }

    if(insideThreshold(lightLeft, lightRight, DETECTION_THRESHOLD)) {

        int avg = (lightLeft + lightRight) / 2;

        if(wallDistance < WALL_FINISHLINE_DISTANCE) {
            if(insideThreshold(avg, lightMiddle, DETECTION_THRESHOLD)) {
                
                PlayTone(1500, 200);

                validateLine = true;
                //return true;
            } else if(validateLine) {
                validateLine = false;
                return true;
            }
        }

    }


    return false;
}



#endif
// _FINISH_LINE_DETECTION_H_