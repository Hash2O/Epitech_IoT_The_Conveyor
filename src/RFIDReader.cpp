#include "RFIDReader.hpp"
#include "MFRC522_I2C.h"
#include <Arduino.h>
#include <Wire.h>

#define RFID_I2C_ADDR 0x28

static MFRC522 rfid(RFID_I2C_ADDR);

void rfid_init(void) { rfid.PCD_Init(); }

bool rfid_is_card_detected(void) {
  if (!rfid.PICC_IsNewCardPresent())
    return false;

  delay(50);
  return rfid.PICC_ReadCardSerial();
}

bool rfid_read_product_code(char *code_buffer, uint8_t buffer_size) {
  if (code_buffer == NULL || buffer_size < 33)
    return false;

  memset(code_buffer, 0, buffer_size);

  sprintf(code_buffer, "%02X%02X%02X%02X%02X%02X%02X", rfid.uid.uidByte[0],
          rfid.uid.uidByte[1], rfid.uid.uidByte[2], rfid.uid.uidByte[3],
          rfid.uid.uidByte[4], rfid.uid.uidByte[5], rfid.uid.uidByte[6]);

  return true;
}

void rfid_stop_communication(void) {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
