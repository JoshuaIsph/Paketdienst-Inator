#ifndef _BARCODE_SCANNER_H_
#define _BARCODE_SCANNER_H_


#define BARCODE_BUFFER_SIZE 16
#define BARCODE_GAP_MAX 20


#define lineThreshold 65


// TODO: Testing and action


// --- BARCODE READING ---
int barcodeBuffer[BARCODE_BUFFER_SIZE];
int barcodeIndex = 0;
int barcodeGapCounter = 0;
int lastBarcodeBit = -1;
bool barcodeReading = false;


void barcode_update(int middleRaw) {

    int barcodeBit = (middleRaw < lineThreshold) ? 0 : 1;
    if (lastBarcodeBit == -1) lastBarcodeBit = barcodeBit;

    if (barcodeBit != lastBarcodeBit) {
        barcodeGapCounter = 0;
        if (!barcodeReading) {
        barcodeReading = true;
        barcodeIndex = 0;
        }
        if (barcodeIndex < BARCODE_BUFFER_SIZE) barcodeBuffer[barcodeIndex++] = barcodeBit;
        lastBarcodeBit = barcodeBit;
    } else if (barcodeReading) {
        barcodeGapCounter++;
        if (barcodeGapCounter > BARCODE_GAP_MAX) {
            barcodeReading = false;
            TextOut(0, LCD_LINE4, "Barcode: ");
            for (int i = 0; i < barcodeIndex; i++) {
                NumOut(60 + i * 4, LCD_LINE4, barcodeBuffer[i]);
            }
            TextOut(0, LCD_LINE5, "Done     ");
        }
    }
}


#endif
