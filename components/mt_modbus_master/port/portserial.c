/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "mt_port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mt_mb.h"
#include "mt_mbport.h"
#include "mt_mbrtu.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "soc/dport_access.h"
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "FREE_MODBUS_SERIAL";
#define MB_LOG(...) ESP_LOGW(__VA_ARGS__)

#define MB_UART UART_NUM_2
#define MB_UART_TX_PIN 23
#define MB_UART_RX_PIN 22
#define MB_UART_EN_PIN 18

#define MB_UART_BUF_SIZE (1024)
#define RS_TX_MODE 1
#define RS_RX_MODE 0

static QueueHandle_t mb_uart_queue = NULL;
static uint8_t *mbDataP = NULL;

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

static uint8_t mb_serial_read(uint8_t *data, uint8_t size) {
  mbDataP = data;
  uint8_t remaindBytes = size;
  // printf("get %d byte %lld\n", size, esp_timer_get_time()/1000);
  while (remaindBytes--) {
    prvvUARTRxISR();
  };
  mbDataP = NULL;
  return 0;
}

static void uart_event_task(void *pvParameters) {
  uart_event_t event;
  size_t buffered_size;
  uint8_t *dtmp = (uint8_t *)malloc(MB_UART_BUF_SIZE);
  uint8_t *ptr = NULL;
  uint8_t size = 0;
  for (;;) {
    // Waiting for UART event.
    if (xQueueReceive(mb_uart_queue, (void *)&event,
                      (portTickType)portMAX_DELAY)) //
    {
      bzero(dtmp, MB_UART_BUF_SIZE);
      switch (event.type) {
      // Event of UART receving data
      /*We'd better handler data event fast, there would be much more data
               events than other types of events. If we take too much time on
           data event, the queue might be full.*/
      case UART_DATA: {
        uart_read_bytes(MB_UART, dtmp, event.size,
                        portMAX_DELAY); // portMAX_DELAY
        // ESP_LOGI(TAG, "[DATA EVT]: size:%d", event.size);

        if (event.size > 1) // ignore char 0x00
        {
          if (0 == dtmp[0]) {
            ptr = dtmp + 1;
            size = event.size - 1;
          } else {
            ptr = dtmp;
            size = event.size;
          }
          mb_serial_read(ptr, size);
        }

        uart_flush_input(MB_UART);
      } break;
      // Event of HW FIFO overflow detected
      case UART_FIFO_OVF:
        ESP_LOGI(TAG, "hw fifo overflow");
        // If fifo overflow happened, you should consider adding flow control
        // for your application. The ISR has already reset the rx FIFO, As an
        // example, we directly flush the rx buffer here in order to read more
        // data.
        uart_flush_input(MB_UART);
        xQueueReset(mb_uart_queue);
        break;
      // Event of UART ring buffer full
      case UART_BUFFER_FULL:
        ESP_LOGI(TAG, "ring buffer full");
        // If buffer full happened, you should consider encreasing your buffer
        // size As an example, we directly flush the rx buffer here in order
        // to read more data.
        uart_flush_input(MB_UART);
        xQueueReset(mb_uart_queue);
        break;
      // Event of UART RX break detected
      case UART_BREAK:
        // ESP_LOGI(TAG, "uart rx break");
        break;
      // Event of UART parity check error
      case UART_PARITY_ERR:
        ESP_LOGI(TAG, "uart parity error");
        break;
      // Event of UART frame error
      case UART_FRAME_ERR:
        ESP_LOGI(TAG, "uart frame error");
        break;
      // UART_PATTERN_DET
      case UART_PATTERN_DET:
        uart_get_buffered_data_len(MB_UART, &buffered_size);
        int pos = uart_pattern_pop_pos(MB_UART);
        ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos,
                 buffered_size);
        if (pos == -1) {
          // There used to be a UART_PATTERN_DET event, but the pattern
          // position queue is full so that it can not record the position. We
          // should set a larger queue size. As an example, we directly flush
          // the rx buffer here.
          uart_flush_input(MB_UART);
        }
        break;
      // Others
      default:
        ESP_LOGE(TAG, "uart event type: %d", event.type);
        break;
      }
    }
  }
  free(dtmp);
  dtmp = NULL;
  vTaskDelete(NULL);
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
                       eMBParity eParity, int tx_pin, int rx_pin, int en_pin) {
  esp_log_level_set(TAG, ESP_LOG_INFO);
  int lDataBit = UART_DATA_8_BITS;
  int lParity = UART_PARITY_DISABLE;
  switch (ucDataBits) {
  case 5:
    lDataBit = UART_DATA_5_BITS;
    break;
  case 6:
    lDataBit = UART_DATA_6_BITS;
    break;
  case 7:
    lDataBit = UART_DATA_7_BITS;
    break;
  case 8:
    lDataBit = UART_DATA_8_BITS;
    break;
  default:
    break;
  }
  switch (eParity) {
  case MB_PAR_NONE:
    lParity = UART_PARITY_DISABLE;
    break;
  case MB_PAR_EVEN:
    lParity = UART_PARITY_EVEN;
    break;
  case MB_PAR_ODD:
    lParity = UART_PARITY_ODD;
    break;
  default:
    break;
  }
  /* Configure parameters of an UART driver,
   * communication pins and install the driver */
  uart_config_t uart_config = {.baud_rate = ulBaudRate, // 115200,
                               .data_bits = lDataBit,   // UART_DATA_8_BITS,
                               .parity = lParity,
                               .stop_bits = UART_STOP_BITS_1,
                               .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                               .rx_flow_ctrl_thresh = 122};
  uart_param_config(MB_UART, &uart_config);

  if (tx_pin == 0)
    tx_pin = MB_UART_TX_PIN;

  if (rx_pin == 0)
    rx_pin = MB_UART_RX_PIN;

  if (en_pin == 0)
    en_pin = MB_UART_EN_PIN;

  // Set UART log level
  esp_log_level_set(TAG, ESP_LOG_INFO);
  // Set UART pins (using UART0 default pins ie no changes.)
  uart_set_pin(MB_UART, tx_pin, rx_pin, en_pin, UART_PIN_NO_CHANGE);
  // Install UART driver, and get the queue.
  uart_driver_install(MB_UART, MB_UART_BUF_SIZE * 2, MB_UART_BUF_SIZE * 2, 20,
                      &mb_uart_queue, 0);
  // set uart mode rs485
  uart_set_mode(MB_UART, UART_MODE_RS485_HALF_DUPLEX);
  // Create a task to handler UART event from ISR
  xTaskCreatePinnedToCore(uart_event_task, "uart_event_task", 4096, NULL,
                          configMAX_PRIORITIES, NULL, 1);

  return TRUE;
}

static INLINE BOOL xMBPortSerialPutByte(CHAR ucByte) // const
{
  /* Put a byte in the UARTs transmit buffer. This function is called
   * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
   * called. */
  uart_write_bytes(MB_UART, &ucByte, 1);
  return TRUE;
}

static INLINE BOOL xMBPortSerialGetByte(CHAR *pucByte) {
  /* Return the byte in the UARTs receive buffer. This function is called
   * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
   */
  *pucByte = (CHAR) * (mbDataP++);
  return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void) { pxMBMasterFrameCBTransmitterEmpty(); }

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR(void) { pxMBMasterFrameCBByteReceived(); }

BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
                             eMBParity eParity, int tx_pin, int rx_pin,
                             int en_pin) {
  return xMBPortSerialInit(ucPORT, ulBaudRate, ucDataBits, eParity, tx_pin,
                           rx_pin, en_pin);
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable) {
  vMBPortEnterCritical();
  UCHAR len = get_s_usLength() + 1;

  if (xTxEnable && !xRxEnable) {
    for (int i = 0; i < len; i++)
      prvvUARTTxReadyISR();
  }
  vMBPortExitCritical();
}

BOOL xMBMasterPortSerialGetByte(CHAR *pucByte) {
  return xMBPortSerialGetByte(pucByte);
}

BOOL xMBMasterPortSerialPutByte(const CHAR ucByte) {
  return xMBPortSerialPutByte(ucByte);
}
