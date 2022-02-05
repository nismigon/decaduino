#include <SPI.h>
#include <DecaDuino.h>

// Timeout parameters
#define TIMEOUT_WAIT_ACK_SENT 5 //ms
#define TIMEOUT_WAIT_DATA_REPLY_SENT 5 //ms
#define ACK_DATA_REPLY_INTERFRAME 10 //ms

// TWR server states state machine enumeration: see state diagram on documentation for more details

enum { Idle, Send };
// Message types of the TWR protocol
#define TWR_MSG_TYPE_UNKNOWN 0
#define TWR_MSG_TYPE_START 1
#define TWR_MSG_TYPE_ACK 2
#define TWR_MSG_TYPE_DATA_REPLY 3
#define GRP_NUMBER 12
uint64_t t2, t3;

DecaDuino decaduino;
uint8_t txData[120];
uint8_t rxData[128];
uint16_t rxLen;
int state;
uint32_t timeout;


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
  // Set RX buffer
  decaduino.setRxBuffer(rxData, &rxLen);
  state = Idle;
  decaduino.plmeRxEnableRequest();
  delay(1000);
}


void loop()
{
  switch (state) {
    // Idle state : wait for a message
    case Idle:
      if (decaduino.rxFrameAvailable()) {
       if (rxData[0] == GRP_NUMBER) {
          state = Send;
        }
        decaduino.plmeRxEnableRequest();
      }
      break;
    // Send state : response to the client
    case Send:
      t2 = decaduino.getLastRxTimestamp();
      txData[0] = GRP_NUMBER;
      decaduino.encodeUint64(t2, &txData[1]);
      t3 = decaduino.getSystemTimeCounter();
      decaduino.encodeUint64(t3, &txData[9]);
      decaduino.plmeRxDisableRequest();
      decaduino.pdDataRequest(txData, 120);
      while(!decaduino.hasTxSucceeded() ) {}
      state = Idle;
      decaduino.plmeRxEnableRequest();
      break;
  }
}


