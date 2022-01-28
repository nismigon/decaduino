// DecaDuinoTWR_client
// A simple implementation of the TWR protocol, client side
// Contributors: Adrien van den Bossche, Réjane Dalcé, Ibrahim Fofana, Robert Try, Thierry Val
// This sketch is a part of the DecaDuino Project - please refer to the DecaDuino LICENCE file for licensing details
// This sketch implements the skew correction published in "Nezo Ibrahim Fofana, Adrien van den Bossche, Réjane
// Dalcé, Thierry Val, "An Original Correction Method for Indoor Ultra Wide Band Ranging-based Localisation System"
// https://arxiv.org/pdf/1603.06736.pdf

#include <SPI.h>
#include <DecaDuino.h>

// Timeout parameters
#define TIMEOUT_WAIT_START_SENT 5 //ms
#define TIMEOUT_WAIT_ACK 10 //ms
#define TIMEOUT_WAIT_DATA_REPLY 20 //ms

// States
enum {STATE_IDLE, STATE_SEND, STATE_WAIT};

// Delay before each ranging
#define RANGING_DELAY 1000

// Group Number
#define GROUP_NUMBER 12

// Packet size
#define PACKET_SIZE 120

uint64_t t1, t2, t3, t4;
uint64_t mask = 0xFFFFFFFFFF;

DecaDuino decaduino;
uint8_t txData[PACKET_SIZE];
uint8_t rxData[PACKET_SIZE];
uint16_t rxLen;
int state;
unsigned long timeout;


void setup()
{
  pinMode(13, OUTPUT); // Internal LED (pin 13 on DecaWiNo board)
  Serial.begin(9600); // Init Serial port
  SPI.setSCK(14); // Set SPI clock pin (pin 14 on DecaWiNo board)

  // Init DecaDuino and blink if initialisation fails
  if ( !decaduino.init() ) {
    Serial.println("decaduino init failed");
    while(1) { digitalWrite(13, HIGH); delay(50); digitalWrite(13, LOW); delay(50); }
  }

  // Set RX buffer
  decaduino.setRxBuffer(rxData, &rxLen);
  state = STATE_IDLE;
  delay(1000);
}

void loop()
{
  switch (state) {
    case STATE_IDLE:
      Serial.println("STATE : Idle");
      delay(RANGING_DELAY);
      // RAZ values
      t1 = 0;
      t2 = 0;
      t3 = 0;
      t4 = 0;
      state = STATE_SEND;
      break;
    case STATE_SEND:
      Serial.println("STATE : Send");
      decaduino.plmeRxDisableRequest();
      // Creation of the packet
      txData[0] = GROUP_NUMBER;
      // Send packet
      decaduino.pdDataRequest(txData, PACKET_SIZE);
      while(!decaduino.hasTxSucceeded());
      Serial.print("\t");
      for (int i = 0; i < PACKET_SIZE; i++) {
        Serial.print(txData[i]);
        Serial.print(" | ");
      }
      Serial.println();
      Serial.println("\tSuccessfully send packet");
      // Save the TX value
      t1 = decaduino.getLastTxTimestamp();
      timeout = millis();
      // Next state
      decaduino.plmeRxEnableRequest();
      state = STATE_WAIT;
      break;
    case STATE_WAIT:
      if(decaduino.rxFrameAvailable()) {
        if(rxData[0] == GROUP_NUMBER) {
          Serial.println("STATE : Wait");
          Serial.println("\tFrame received, extracting information...");
          t2 = decaduino.decodeUint64(&rxData[1]);
          t3 = decaduino.decodeUint64(&rxData[9]);
          t4 = decaduino.getLastRxTimestamp();
          uint32_t tof = (((t4 - t1) & mask) - ((t3 - t2) & mask))/2;
          float distance = tof*RANGING_UNIT;
          Serial.print("\tTime of flight : ");
          Serial.print(tof);
          Serial.print(", Distance : ");
          Serial.print(distance);
          Serial.println();
          Serial.print("\t");
          for (int i = 0; i < PACKET_SIZE; i++) {
            Serial.print(rxData[i]);
            Serial.print(" | ");
          }
          state = STATE_IDLE;
        }
        else {
          if (timeout + 1000 < millis()) {
            Serial.println("\tFrame received is not correct... Reemission...");
            state = STATE_IDLE;
          }
          else {
            decaduino.plmeRxEnableRequest();
          }
        }
      }
      break; 
    default:
      state = STATE_IDLE;
      break;
  }
}

