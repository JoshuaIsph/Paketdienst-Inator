#ifndef _HARDWARE_CONTROL_H_
#define _HARDWARE_CONTROL_H_

#include "Utils.h"
#include "Peripheral.h"
#include "Settings.h"


// TODO: speed adjustment on the hill


// configure sensors
void hardwareControl_init() {
  SetSensorType(LEFT_SENSOR, SENSOR_TYPE_LIGHT);
  SetSensorType(RIGHT_SENSOR, SENSOR_TYPE_LIGHT);
  SetSensorType(BARCODE_SENSOR, SENSOR_TYPE_LIGHT);
  SetSensorLight(LEFT_SENSOR, true);
  SetSensorLight(RIGHT_SENSOR, true);
  SetSensorLight(BARCODE_SENSOR, true);
}


void applyMotorPower(int motorPort, int signedPower) {
    int p = clampInt(signedPower, -POWER_RANGE, POWER_RANGE);
    if (p > 0) OnFwd(motorPort, p);
    else if (p < 0) OnRev(motorPort, -p);
    else Off(motorPort);
}

#endif
