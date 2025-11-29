#ifndef _FINISH_LINE_DETECTION_H_
#define _FINISH_LINE_DETECTION_H_


#include "Utils.h"
#include "Logger.h"

#define DETECTION_THRESHOLD 10
#define WALL_FINISHLINE_DISTANCE 20

bool detection_finishLineReached(int lightLeft, int lightMiddle, int lightRight, int wallDistance) {

    if(insideThreshold(lightLeft, lightRight, DETECTION_THRESHOLD)) {

        int avg = (lightLeft + lightRight) / 2;

        if(insideThreshold(avg, lightMiddle, DETECTION_THRESHOLD)) {
            if(wallDistance < WALL_FINISHLINE_DISTANCE) {
                PlayTone(1500, 200);
                return true;
            }
        }

    }


    return false;
}



#endif
