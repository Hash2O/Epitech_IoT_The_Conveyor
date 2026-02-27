#ifndef RFID_READER_HPP
#define RFID_READER_HPP

#include <Arduino.h>

// Déclarations des nouvelles fonctions globales
void rfid_init(void);
bool rfid_is_card_detected(void);
bool rfid_read_product_code(char *code_buffer, uint8_t buffer_size);
void rfid_stop_communication(void);

#endif
