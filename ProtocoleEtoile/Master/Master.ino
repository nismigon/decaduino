#include <SPI.h>
#include <DecaDuino.h>

#define MAX_FRAME_LEN 120
#define GROUP_NUMBER 2
uint8_t txData[MAX_FRAME_LEN];
uint16_t txLen;
DecaDuino decaduino;
int rxFrames;


void setup()
{
  pinMode(13, OUTPUT);  // Internal LED (pin 13 on DecaWiNo board)
  Serial.begin(115200); // Init Serial port
  SPI.setSCK(14);       // Set SPI clock pin (pin 14 on DecaWiNo board)

  // Init DecaDuino and blink if initialisation fails
  if ( !decaduino.init() ) {
    Serial.println("Decaduino init failed");
    while(1) { digitalWrite(13, HIGH); delay(50); digitalWrite(13, LOW); delay(50); }
  }
}


void loop()
{
  // Group number on first byte
  txData[0] = GROUP_NUMBER;
  for (int i=1; i<MAX_FRAME_LEN; i++) {
    txData[i] = i;
  }
  // Add timestamp into the packet
  uint64_t currentTimestamp = decaduino.getSystemTimeCounter();
  uint8_t encodedTimestamp[8];
  // Encode Uint64 to table of Uint8
  decaduino.encodeUint64(currentTimestamp, encodedTimestamp);
  for (int i = 0; i < 8; i++) {
      txData[i+1] = encodedTimestamp[i];
  }
  // Send the packet
  decaduino.pdDataRequest(txData, MAX_FRAME_LEN);
  while ( !decaduino.hasTxSucceeded() );
  // Wait 1 second before re-emit
  delay(1000);
}


