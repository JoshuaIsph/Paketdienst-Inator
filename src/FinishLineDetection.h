#ifndef _FINISH_LINE_DETECTION_H_
#define _FINISH_LINE_DETECTION_H_


#include "Utils.h"
#include "Logger.h"

#define DETECTION_THRESHOLD 10
#define WALL_FINISHLINE_DISTANCE 17
#define FINISHLINE_VALIDATE_TIMEOUT 20 // 20



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
