#include "HardwareControl.h"
#include "Peripheral.h"

#define BASE_SPEED 61

void rotate180() {
    applyMotorPower(LEFT_MOTOR, 100);
    applyMotorPower(RIGHT_MOTOR, -100);
    Wait(1650);
    applyMotorPower(LEFT_MOTOR, BASE_SPEED);
    applyMotorPower(RIGHT_MOTOR, BASE_SPEED);
}