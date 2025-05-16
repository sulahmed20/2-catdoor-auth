#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10
#define SERVO_PIN 3

MFRC522 rfid(SS_PIN, RST_PIN);
Servo doorServo;

// Replace with actual authorized UID (from serial monitor output)
byte authorizedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

bool isAuthorizedUID(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != authorizedUID[i]) return false;
  }
  return true;
}

void openDoor() {
  doorServo.write(90);  // Open position
  delay(5000);          // Wait 5 seconds
  doorServo.write(0);   // Close position
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  doorServo.attach(SERVO_PIN);
  doorServo.write(0); // Start closed
  Serial.println("RFID cat door ready.");
}

void loop() {
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(500);
    return;
  }

  Serial.print("UID tag: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (isAuthorizedUID(rfid.uid.uidByte)) {
    Serial.println("✅ Authorized cat detected. Opening door...");
    openDoor();
  } else {
    Serial.println("❌ Unauthorized tag.");
  }

  // Halt PICC and stop encryption
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000); // Wait before scanning again
}