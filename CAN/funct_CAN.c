#include "CAN.h"
#include "LPC17xx.h"

void send_data(uint8_t time, uint8_t lives, uint16_t score) {
  CAN_TxMsg.data[0] = time;
  CAN_TxMsg.data[1] = lives;
  CAN_TxMsg.data[2] = (score & 0xFF00) >> 8;
  CAN_TxMsg.data[3] = score & 0xFF;
  CAN_TxMsg.len = 4;
  CAN_TxMsg.id = 2;
  CAN_TxMsg.format = STANDARD_FORMAT;
  CAN_TxMsg.type = DATA_FRAME;
  CAN_wrMsg(1, &CAN_TxMsg); /* transmit message */
}