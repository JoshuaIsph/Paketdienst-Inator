#ifndef _BARCODE_READER_H_
#define _BARCODE_READER_H_

#include "Peripheral.h"
#include "Utils.h"
#include "Logger.h"
#include "Settings.h"


#define BARCODE_SCAN_TIMEOUT 7
#define RELATIVE_THRESHOLD_BARCODE 6


bool barcodeScanner_enable = true;

enum BarcodeCommand {
    NONE,
    PUSH_BLOCK
};



/**
 * Initializes barcode reader.
 */
void barcodeReader_init() {
    barcodeScanner_enable = true;
}


/**
 * Scans for barcode and validates and returns command.
 * 
 * @param leftLightVal Light value of left sensor
 * @param rightLightVal Light value of right sensor
 * @param scannerLightVal Light value of barcodescanner
 * 
 * @returns Command as enum BarcodeCommand
 */
BarcodeCommand barcodeReader_update(int leftLightVal, int rightLightVal, int scannerLightVal) {

    static bool barState = false;
    static bool change = false;
    static int barCount = 0;
    static int timeSinceChange = 0;


    if(!barcodeScanner_enable) {
        return NONE;
    }

    int highVal;

    if(leftLightVal < rightLightVal) {
        highVal = rightLightVal;
    } else {
        highVal = leftLightVal;
    }

    if(insideThreshold(highVal, scannerLightVal, RELATIVE_THRESHOLD_BARCODE)) {
        // White bar
        if(!barState) {
            barState = true;
            change = true;
            barCount++;
        }

    } else if(scannerLightVal < highVal) {
        // Black bar
        if(barState) {
            barState = false;
            change = true;
        }

    } else {
        // Strange readings
        barCount = 0;
        return NONE;
    }

    if(change) {
        change = false;
        timeSinceChange = 0;
    } else {
        timeSinceChange++;
    }

    BarcodeCommand command = NONE;

    if(timeSinceChange > BARCODE_SCAN_TIMEOUT) {
        timeSinceChange = 0;

        log_println(StrCat("Barcode: ", NumToStr(barCount)));

        if(barCount == 3) {
            command = PUSH_BLOCK;
        }

        barCount = 0;
    }

    return command;
}



/**
 * Enables/Disables barcode reader.
 * 
 * @param enable true to enable, false to disable
 */
void barcodeScanner_setEnable(bool enable) {
    barcodeScanner_enable = enable;
}



#endif
// _BARCODE_READER_H_