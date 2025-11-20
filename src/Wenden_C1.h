#include "HardwareControl.h"
#include "Transformations.h"
#include "Logger.h"

bool wenden() {
    int leftRaw = SensorValue(LEFT_SENSOR);
    int rightRaw = SensorValue(RIGHT_SENSOR);
    int middleRaw = SensorValue(BARCODE_SENSOR);
    log_println(String(leftRaw));
    if (leftRaw == rightRaw && rightRaw == middleRaw) {
        rotate180();
        return true;
    }
    return false;
}