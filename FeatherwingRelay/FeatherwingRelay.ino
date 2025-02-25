#include <Adafruit_MCP2515.h>

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_CAN
   #define CS_PIN    PIN_CAN_CS
#else
   #define CS_PIN    7
#endif

#define CAN_BAUDRATE (500000)
#define SIGNAL 4

Adafruit_MCP2515 mcp(CS_PIN);

// 0x10F 0x08 0x11 0x50 0x01 0x00 0x00 0x00 0x00 (CLOSE)
// 0x10F 0x08 0x11 0x50 0x02 0x00 0x00 0x00 0x00 (OPEN)

// Define the two target messages
const uint8_t OPEN_MESSAGE[]  = {0x08, 0x11, 0x50, 0x02, 0x00, 0x00, 0x00, 0x00};
const uint8_t CLOSE_MESSAGE[]  = {0x08, 0x11, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00};

void setup() {
  Serial.begin(115200);
  pinMode(SIGNAL, OUTPUT);
  // while (1) delay(10);

  Serial.println("MCP2515 Receiver with Strict Message Filtering!");

  if (!mcp.begin(CAN_BAUDRATE)) {
    Serial.println("Error initializing MCP2515.");
    // while (1) delay(10);
  }
  Serial.println("MCP2515 chip found");
}

void loop() {
  int packetSize = mcp.parsePacket();

  if (packetSize) {
    uint32_t receivedId = mcp.packetId();  // Get the received CAN ID

    // **Check if the received message has the correct CAN ID (0x60F)**
    if (receivedId != 0x60E) {  
      return; // Ignore messages with different CAN IDs
    }

    // **Read the received data into a buffer**
    uint8_t receivedData[8] = {0};  // Initialize an array to store the received message
    int index = 0;
    while (mcp.available() && index < 8) {
      receivedData[index++] = mcp.read();
    }

    // **Check if the received message matches the RETRACT or EXTEND command**
    if (memcmp(receivedData, CLOSE_MESSAGE, 8) == 0) {
      Serial.println("Received CLOSE command!");
      digitalWrite(SIGNAL, LOW);
    } 
    else if (memcmp(receivedData, OPEN_MESSAGE, 8) == 0) {
      Serial.println("Received OPEN command!");
      digitalWrite(SIGNAL, HIGH);
    } 

    // if (memcmp(receivedData, OPEN_MESSAGE, 8) == 0) {
    //   Serial.println("Received OPEN command!");
    //   digitalWrite(SIGNAL, HIGH);
    //   delay(10000);
    //   digitalWrite(SIGNAL, LOW);
    // } 

    else {
      Serial.println("Unknown CAN message received, ignoring.");
    }
  }
}
