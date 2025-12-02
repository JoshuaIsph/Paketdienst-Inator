#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_
// Connected peripheral devices

// Sensors
#define LEFT_SENSOR         S1
#define RIGHT_SENSOR        S2
#define MIDDLE_SENSOR       S4
#define BARCODE_SENSOR      MIDDLE_SENSOR
#define ULTRA_SONIC_SENSOR  S3

#define OUT_A 0
#define OUT_B 1
#define OUT_C 2

// Motors
#define BASKET_MOTOR    OUT_A
#define LEFT_MOTOR      OUT_B
#define RIGHT_MOTOR     OUT_C


// Black magic

#define ERROR_RIGHT_MOTOR_CHOICE Unsupported choice setting of right motor


#if LEFT_MOTOR == OUT_A
    #if RIGHT_MOTOR == OUT_B
        #define LR_MOTOR OUT_AB
    #elif RIGHT_MOTOR == OUT_C
        #define LR_MOTOR = OUT_AC
    #else
        #error ERROR_RIGHT_MOTOR_CHOICE
    #endif
#elif LEFT_MOTOR == OUT_B
    #if RIGHT_MOTOR == OUT_A
        #define LR_MOTOR OUT_AB
    #elif RIGHT_MOTOR == OUT_C
        #define LR_MOTOR OUT_BC
    #else
        #error ERROR_RIGHT_MOTOR_CHOICE
    #endif
#elif LEFT_MOTOR == OUT_C
    #if RIGHT_MOTOR == OUT_A
        #define LR_MOTOR OUT_AC
    #elif RIGHT_MOTOR == OUT_B
        #define LR_MOTOR OUT_BC
    #else
        #error ERROR_RIGHT_MOTOR_CHOICE
    #endif
#else
    #error Unsupported choice setting of left motor
#endif


#endif
