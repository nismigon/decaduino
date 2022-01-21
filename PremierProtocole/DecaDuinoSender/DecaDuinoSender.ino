// DecaDuinoSender
// This sketch shows how to use the DecaDuino library to send messages over the UWB radio 
// by Adrien van den Bossche <vandenbo@univ-tlse2.fr>
// This sketch is a part of the DecaDuino Project - please refer to the DecaDuino LICENCE file for licensing details

#include <SPI.h>
#include <DecaDuino.h>

#define MAX_FRAME_LEN 120
#define GROUP_NUMBER 2
#define MAX_RETRIES 3
#define TIMEOUT 100
uint8_t txData[MAX_FRAME_LEN];
uint16_t txLen;
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
    Serial.println("decaduino init failed");
    while(1) { digitalWrite(13, HIGH); delay(50); digitalWrite(13, LOW); delay(50); }
  }
  decaduino.setChannel(GROUP_NUMBER);
  // Set RX buffer and enable RX
  decaduino.setRxBuffer(rxData, &rxLen);
}


void loop()
{
  unsigned long emission = millis(); // packet emission time
  unsigned int transmission_number = 0;
  
  // creation of the packet
  txData[0] = GROUP_NUMBER;
  for (int i=1; i<MAX_FRAME_LEN; i++) {
    txData[i] = 1;
  }

  // send packet
  send_packet(txData, transmission_number);
  transmission_number++;

  // ack
  bool receivedACK = false;
  decaduino.plmeRxEnableRequest();
  while (!receivedACK) {
    // Verify timeout
    if (millis() - emission > TIMEOUT * transmission_number) {
      if (transmission_number >= MAX_RETRIES) {
        Serial.println("Maximum retransmission have been reached, pass to next packet");
        break;
      } else {
        send_packet(txData, transmission_number);
        transmission_number++; 
      }
    }
    // Verify received frame
    if (decaduino.rxFrameAvailable()) {
      if (rxData[0] == GROUP_NUMBER) {
          Serial.println("ACK received from the receiver");
          receivedACK = true;
      } else {
        decaduino.plmeRxEnableRequest(); 
      }
    }
  }
  // wait 1 second
  if (emission + 1000 - millis() > 0) {
     delay(emission + 1000 - millis());
  }
}

void send_packet(uint8_t *packet, int transmission_number) {
  decaduino.plmeRxDisableRequest();
  decaduino.pdDataRequest(packet, MAX_FRAME_LEN);
  while ( !decaduino.hasTxSucceeded() );
  Serial.print("Transmission ");
  Serial.print(transmission_number);
  Serial.println(" : TX sended to the receiver");
  decaduino.plmeRxEnableRequest();
}


