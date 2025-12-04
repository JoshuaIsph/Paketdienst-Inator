#ifndef _SERIAL_MONITOR_H_
#define _SERIAL_MONITOR_H_


byte bluetoothChannel = 0;


/**
 * Set bluetooth channel to send bytes.
 * 
 * @param channel Bluetooth channel (0 to 3)
 */
void serial_setChannel(byte channel) {

    bluetoothChannel = channel;
}


/**
 * Sends string to serial monitor.
 * 
 * @param data String to send
 */
void serial_println(const string& data) {

    string line = StrCat(data, "\n");

    const int length = StrLen(line);

    byte buffer[];

    ArrayInit(buffer, 0, length);

    StrToByteArray(line, buffer);

    //while(BluetoothStatus(bluetoothChannel) & BT_BRICK_PORT_OPEN);

    int status = BluetoothWrite(bluetoothChannel, buffer);
    Wait(3);

}



#endif
// _SERIAL_MONITOR_H_