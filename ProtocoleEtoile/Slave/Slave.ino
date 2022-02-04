#include <SPI.h>
#include <DecaDuino.h>

#define MAX_FRAME_LEN 120
#define GROUP_NUMBER 2
uint8_t rxData[MAX_FRAME_LEN];
uint16_t rxLen;
DecaDuino decaduino;
int rxFrames;


void setup()
{
  pinMode(13, OUTPUT); // Internal LED (pin 13 on DecaWiNo board)
  Serial.begin(115200); // Init Serial port
  SPI.setSCK(14); // Set SPI clock pin (pin 14 on DecaWiNo board)

  // Init DecaDuino and blink if initialisation fails
  if ( !decaduino.init() ) {
    Serial.println("Decaduino init failed");
    while(1) { digitalWrite(13, HIGH); delay(50); digitalWrite(13, LOW); delay(50); }
  }
  // Set RX buffer and enable RX
  decaduino.setRxBuffer(rxData, &rxLen);
  decaduino.plmeRxEnableRequest();
  rxFrames = 0;
}


void loop()
{
  // If a message has been received, print it and re-enable receiver
  if ( decaduino.rxFrameAvailable()) {
    if (rxData[0] == GROUP_NUMBER) {
      // Get the encoded timestamp contained in the packet
      uint8_t encodedTimestamp[8];
      for (int i = 0; i < 8; i++) {
          encodedTimestamp[i] = rxData[i+1];
      }
      // Decode the timestamp
      uint64_t masterTimestamp = decaduino.decodeUint64(encodedTimestamp);
      // Offset calculation
      uint64_t offset = masterTimestamp - getSystemTimeCounter();
      // Show in the serial port the result
      Serial.println("Offset : %d", offset);
    }
    decaduino.plmeRxEnableRequest();
  }
}


