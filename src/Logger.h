
#ifndef _LOGGER_H_
#define _LOGGER_H_

#define SERIAL_MONITOR 1    // Enable serial monitor
#define ENABLE_SOUND 1      // Enable logging sound

#if SERIAL_MONITOR
#include "SerialMonitor.h"
#endif

// Display properties
#define LETTER_WIDTH 5
#define LETTER_HEIGHT 8
#define COLUMN_COUNT DISPLAY_HEIGHT/LETTER_HEIGHT
#define LINE_COUNT DISPLAY_WIDTH/LETTER_WIDTH

// Sound properties
#define STATUS_TONE_FREQ 475
#define STATUS_TONE_DURATION 90

#define NOTIFY_TONE_FREQ_1 590
#define NOTIFY_TONE_FREQ_2 600
#define NOTIFY_TONE_DURATION 30

#define ALARM_BASE_FREQ 770
#define ALARM_FREQ_1 ALARM_BASE_FREQ+60
#define ALARM_FREQ_2 ALARM_BASE_FREQ+80
#define ALARM_FREQ_3 ALARM_BASE_FREQ+100
#define ALARM_TONE_DURATION 2



/**
 * Container for strings of unknown length for arrays.
 * Only exists because of nxc compiler limitations...
 * 
 * NXC won't allow to create an array of strings of unknown size.
 * But allows this...
 */
struct StringContainer {
    string text;
};

StringContainer printBuffer[LINE_COUNT];    // Buffer for string displayed.
int printBufferStart = LINE_COUNT - 1;      // Starting index of buffer

bool log_enableLogger = true;


/**
 * Initialize logger and setup serial monitor if present.
 * Set bluetooth channel to 0 for string prints.
 */
void log_init() {

    #if SERIAL_MONITOR
    serial_setChannel(0);
    serial_println("Robot BT COM");
    #endif

    for(unsigned int i = 0; i < LINE_COUNT; i++) {
        StringContainer line;
        line.text = "";
        printBuffer[i] = line;
    }

}


/**
 * Print string to serial monitor.
 * 
 * @param data String to print to serial monitor
 */
 void log_printSerial(const string& data) {

    #if SERIAL_MONITOR
    serial_println(data);
    #endif
}


/**
 * Internal function to shift line buffer
 */
void log_intern_shiftBuffer() {

    if(printBufferStart <= 0) {
        printBufferStart = LINE_COUNT - 1;
    } else {
        printBufferStart--;
    }
}


/**
 * getter function for start index of print buffer.
 * 
 * @returns Starting index of print buffer
 */
int log_intern_getCurrentLineIndex() {
   return printBufferStart;
}


/**
 * Prints whole line buffer on display.
 */
void log_intern_printBuffer() {

    int index = printBufferStart;

    for(unsigned int i = 0; i < LINE_COUNT; i++) {

        TextOut(0, LETTER_HEIGHT * i, printBuffer[index].text);
        index++;
        if(index >= LINE_COUNT) {
            index = 0;
        }

    }

}


/**
 * Print string on screen and serial monitor, if present.
 * 
 * @param msg String to print
 */
void log_println(const string& msg) {

    serial_println(msg);

    if(!log_enableLogger && !veryImportant) {
        return;
    }

    StringContainer newLine;
    newLine.text = msg;

    log_intern_shiftBuffer();
    int currentIndex = log_intern_getCurrentLineIndex();
    printBuffer[currentIndex] = newLine;

    ClearScreen();
    log_intern_printBuffer();
}


/**
 * Plays sound for state changes of state machine.
 */
void log_playStatusSound() {
    #if ENABLE_SOUND
    PlayTone(STATUS_TONE_FREQ, STATUS_TONE_DURATION); Wait(STATUS_TONE_DURATION);
    #endif
}


/**
 * Plays notify sound for important debug messages
 */
void log_playNotifySound() {
    #if ENABLE_SOUND
    PlayTone(NOTIFY_TONE_FREQ_1, NOTIFY_TONE_DURATION * 2); Wait(NOTIFY_TONE_DURATION);
    PlayTone(NOTIFY_TONE_FREQ_2, NOTIFY_TONE_DURATION); Wait(NOTIFY_TONE_DURATION);
    #endif
}


/**
 * Internal function.
 * Plays alarm tone sequence.
 */
void log_internal_alarmTone() {
    #if ENABLE_SOUND
    PlayTone(ALARM_FREQ_1, ALARM_TONE_DURATION * 3); Wait(ALARM_TONE_DURATION);
    PlayTone(ALARM_FREQ_2, ALARM_TONE_DURATION * 3); Wait(ALARM_TONE_DURATION);
    PlayTone(ALARM_FREQ_3, ALARM_TONE_DURATION * 3); Wait(ALARM_TONE_DURATION);
    #endif
}


/**
 * Plays alarm sound for errors.
 */
void log_playAlarm() {

    for(unsigned int i = 0; i < 2; i++) {
        for(unsigned int u = 0; u < 10; u++) {
            log_internal_alarmTone();
        }
        Wait(100);
    }

}


/**
 * Holds current thread until user pressed the centered button of the nxt controller.
 */
void log_waitForUserInput() {

    while(!ButtonPressed(BTNCENTER, false));
}


void log_enable(bool enable) {
    log_enableLogger = enable;
}


#endif
// _LOGGER_H_