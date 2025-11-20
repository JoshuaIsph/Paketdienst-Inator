#ifndef _SERIAL_MONITOR_H_
#define _SERIAL_MONITOR_H_


byte bluetoothChannel = 0;


void serial_setChannel(byte channel) {

    bluetoothChannel = channel;
}


void serial_println(const string& data) {

    //string line = StrCat(data, "\n");

    const int length = StrLen(data);

    byte buffer[];

    ArrayInit(buffer, 0, length);

    StrToByteArray(data, buffer);

    while(BluetoothStatus(bluetoothChannel) & BT_BRICK_PORT_OPEN);

    int status = BluetoothWrite(bluetoothChannel, buffer);
    Wait(3);

}



#endif
