#ifndef _UTILS_H_
#define _UTILS_H_

// --- HELPER FUNCTIONS ---

int clampInt(int v, int lo, int hi) {
if (v < lo) return lo;
if (v > hi) return hi;
return v;
}

int fastAbs(int a) {
    if(a < 0) {
        return -a;
    }
    return a;
}

bool inRange(float a, float b, float range) {

    float distance = a - b;

    if(distance < 0) {
        distance = -distance;
    }

    if(distance <= range) {
        return true;
    }

    return false;
}


bool insideThreshold(int a, int b, int threshold) {

    int distance = a - b;

    if(distance < 0) {
        distance = -distance;
    }

    if(distance <= threshold) {
        return true;
    }

    return false;
}


#endif
// _UTILS_H_