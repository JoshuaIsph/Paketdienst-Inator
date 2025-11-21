
#ifndef _LOGGER_H_
#define _LOGGER_H_

#define SERIAL_MONITOR 1

#if SERIAL_MONITOR
#include "SerialMonitor.h"
#endif

#define LETTER_WIDTH 5
#define LETTER_HEIGHT 8
#define COLUMN_COUNT DISPLAY_HEIGHT/LETTER_HEIGHT
#define LINE_COUNT DISPLAY_WIDTH/LETTER_WIDTH

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


// Only because of nxc compiler limitations...
struct StringContainer {
    string text;
};

StringContainer printBuffer[LINE_COUNT];
int printBufferStart = LINE_COUNT - 1;


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


// Print to serial monitor
void log_printSerial(const string& data) {

    #if SERIAL_MONITOR
    serial_println(data);
    #endif
}


// Internal function to shift line buffer
void log_intern_shiftBuffer() {

    if(printBufferStart <= 0) {
        printBufferStart = LINE_COUNT - 1;
    } else {
        printBufferStart--;
    }
}


int log_intern_getCurrentLineIndex() {
   return printBufferStart;
}

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


// Wrapper function to print on screen and serial monitor
void log_println(const string& msg) {

    serial_println(msg);

    StringContainer newLine;
    newLine.text = msg;

    log_intern_shiftBuffer();
    int currentIndex = log_intern_getCurrentLineIndex();
    printBuffer[currentIndex] = newLine;

    ClearScreen();
    log_intern_printBuffer();
}



// Play sound for state change of state machine
void log_playStatusSound() {
    PlayTone(STATUS_TONE_FREQ, STATUS_TONE_DURATION); Wait(STATUS_TONE_DURATION);
}

// Play notify sound for important debug message on screen
void log_playNotifySound() {

    PlayTone(NOTIFY_TONE_FREQ_1, NOTIFY_TONE_DURATION * 2); Wait(NOTIFY_TONE_DURATION);
    PlayTone(NOTIFY_TONE_FREQ_2, NOTIFY_TONE_DURATION); Wait(NOTIFY_TONE_DURATION);

}

// Internal function
void log_internal_alarmTone() {

    PlayTone(ALARM_FREQ_1, ALARM_TONE_DURATION * 3); Wait(ALARM_TONE_DURATION);
    PlayTone(ALARM_FREQ_2, ALARM_TONE_DURATION * 3); Wait(ALARM_TONE_DURATION);
    PlayTone(ALARM_FREQ_3, ALARM_TONE_DURATION * 3); Wait(ALARM_TONE_DURATION);

}

// Play sound for any runtime error
void log_playAlarm() {

    for(unsigned int i = 0; i < 2; i++) {
        for(unsigned int u = 0; u < 10; u++) {
            log_internal_alarmTone();
        }
        Wait(100);
    }

}


// Returns if user pressed orange button
void log_waitForUserInput() {

    while(!ButtonPressed(BTNCENTER, false));
}


#endif
