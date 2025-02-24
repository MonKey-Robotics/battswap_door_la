#include <Adafruit_MCP2515.h>

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_CAN
   #define CS_PIN    PIN_CAN_CS
#else
   #define CS_PIN    7
#endif

#define CAN_BAUDRATE (500000)
#define PWM 5
#define DIR 9

Adafruit_MCP2515 mcp(CS_PIN);

// 0x60F 0x08 0x11 0x30 0x01 0x00 0x00 0x00 0x00 (Retract)
// 0x60F 0x08 0x11 0x30 0x02 0x00 0x00 0x00 0x00 (Extend)

// Define the two target messages
const uint8_t RETRACT_MESSAGE[] = {0x08, 0x11, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00};
const uint8_t EXTEND_MESSAGE[]  = {0x08, 0x11, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};

void off_lin_act()
{
  digitalWrite(PWM, LOW);
  digitalWrite(DIR, LOW);
}

void extend_lin_act()
{
  digitalWrite(PWM, HIGH);
  digitalWrite(DIR, LOW);
}

void retract_lin_act()
{
  digitalWrite(PWM, HIGH);
  digitalWrite(DIR, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(DIR, OUTPUT);
  pinMode(PWM, OUTPUT);
  while (!Serial) delay(10);

  Serial.println("MCP2515 Receiver with Strict Message Filtering!");

  if (!mcp.begin(CAN_BAUDRATE)) {
    Serial.println("Error initializing MCP2515.");
    while (1) delay(10);
  }
  Serial.println("MCP2515 chip found");
}

void loop() {
  int packetSize = mcp.parsePacket();

  if (packetSize) {
    uint32_t receivedId = mcp.packetId();  // Get the received CAN ID

    // **Check if the received message has the correct CAN ID (0x60F)**
    if (receivedId != 0x60F) {  
      return; // Ignore messages with different CAN IDs
    }

    // **Read the received data into a buffer**
    uint8_t receivedData[8] = {0};  // Initialize an array to store the received message
    int index = 0;
    while (mcp.available() && index < 8) {
      receivedData[index++] = mcp.read();
    }

    // **Check if the received message matches the RETRACT or EXTEND command**
    if (memcmp(receivedData, RETRACT_MESSAGE, 8) == 0) {
      Serial.println("Received RETRACT command!");
      off_lin_act();
      delay(100);
      retract_lin_act();
      delay(11000);
      off_lin_act();
    } 
    else if (memcmp(receivedData, EXTEND_MESSAGE, 8) == 0) {
      Serial.println("Received EXTEND command!");
      off_lin_act();
      delay(100);
      extend_lin_act();
      delay(11000);
      off_lin_act();
    } 
    else {
      Serial.println("Unknown CAN message received, ignoring.");
    }
  }
}
