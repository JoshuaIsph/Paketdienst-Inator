#ifndef _BASKET_H_
#define _BASKET_H_


#include "Peripheral.h"
#include "HardwareControl.h"
#include "Logger.h"


#define EMPTY_ANGLE -100    // Angle to move to empty basket
#define EMPTY_POWER 30      // Power used to move basket


/**
 * Internal function.
 * Function holds current thread until basket motor stops spinning,
 * either by setting power to 0 or physically blockage.
 */
void basket_internal_waitUntilMotorStop() {

    
    int tacho = MotorBlockTachoCount(BASKET_MOTOR);

    while(true) {

        //log_println(StrCat("OverL: ", NumToStr(tacho)));

        Wait(100);

        int cache = MotorBlockTachoCount(BASKET_MOTOR);

        if(cache == tacho) {
            break;
        }
        tacho = cache;
    }

    Off(BASKET_MOTOR); // If motor stopped or blocked

}


/**
 * Internal function.
 * Moves basket to its parking position
 * and sets relative angle to 0.
 */
void basket_internal_prepare() {

    applyMotorPower(BASKET_MOTOR, EMPTY_POWER);

    basket_internal_waitUntilMotorStop();

    applyMotorPower(BASKET_MOTOR, 0);
    ResetTachoCount(BASKET_MOTOR);
    RotateMotor(BASKET_MOTOR, EMPTY_POWER, -20);

    Wait(5);

    ResetTachoCount(BASKET_MOTOR);
    log_println("Reset basket tacho");
}


/**
 * Moves basket to its parking position.
 */
void basket_init() {

    basket_internal_prepare();
}


/**
 * Empty basket.
 * Move basket to throw out its content.
 */
void basket_empty() {

    basket_internal_prepare();

    log_println("Empty basket");
    RotateMotor(BASKET_MOTOR, EMPTY_POWER, EMPTY_ANGLE);

    basket_internal_waitUntilMotorStop();

    Wait(2000);

    log_println("Park basket");    
    RotateMotor(BASKET_MOTOR, EMPTY_POWER, -(EMPTY_ANGLE));


    basket_internal_waitUntilMotorStop();

}



#endif
// _BASKET_H_