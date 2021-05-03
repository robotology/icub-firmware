/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sio_config.h"
#include "Driver_USART.h"
#include "lwip/sys.h"
#include "lwip/sio.h"

#define _DRIVER_USART(n)  Driver_USART##n
#define  DRIVER_USART(n) _DRIVER_USART(n)

#define USART_DATA_BITS(n) \
  ((n == 8) ? ARM_USART_DATA_BITS_8 : 0U)

#define USART_PARITY(n) \
  ((n == 0) ? ARM_USART_PARITY_NONE : \
   (n == 1) ? ARM_USART_PARITY_EVEN : \
   (n == 2) ? ARM_USART_PARITY_ODD  : \
   0U)

#define USART_STOP_BITS(n) \
  ((n == 1) ? ARM_USART_STOP_BITS_1 : \
   (n == 2) ? ARM_USART_STOP_BITS_2 : \
   0U)

#define USART_FLOW_CONTROL(n) \
  ((n == 0) ? ARM_USART_FLOW_CONTROL_NONE    : \
   (n == 1) ? ARM_USART_FLOW_CONTROL_RTS     : \
   (n == 2) ? ARM_USART_FLOW_CONTROL_CTS     : \
   (n == 3) ? ARM_USART_FLOW_CONTROL_RTS_CTS : \
   0U)

typedef struct {
  ARM_DRIVER_USART *drv;                // USART driver
  uint16_t rx_cnt;                      // Number of bytes received
  uint16_t rx_index;                    // Receive read index
  uint8_t  rx_abort;                    // Receive abort flag
  uint8_t  tx_busy;                     // Transmit busy (tx count)
  uint8_t  rx_buf[256];                 // Receive buffer (2^n)
  uint8_t  tx_buf[66];                  // Transmit buffer
} sio_status_t;

#if SIO0_INTERFACE
extern ARM_DRIVER_USART DRIVER_USART(SIO0_USART_DRV_NUM);
static sio_status_t   sio0_status;
static const uint32_t sio0_usart_mode = (
  ARM_USART_MODE_ASYNCHRONOUS                  |
  USART_DATA_BITS    (SIO0_USART_DATA_BITS)    |
  USART_PARITY       (SIO0_USART_PARITY)       |
  USART_STOP_BITS    (SIO0_USART_STOP_BITS)    |
  USART_FLOW_CONTROL (SIO0_USART_FLOW_CONTROL)
);
static const uint32_t sio0_usart_baudrate =
                      SIO0_USART_BAUDRATE;
static void sio0_notify (uint32_t event) {
  if (event & ARM_USART_EVENT_SEND_COMPLETE) {
    sio0_status.tx_busy = 0;
  }
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
    sio0_status.rx_cnt += sizeof(sio0_status.rx_buf);
    sio0_status.drv->Receive (sio0_status.rx_buf, sizeof(sio0_status.rx_buf));
  }
}
#endif
#if SIO1_INTERFACE
extern ARM_DRIVER_USART DRIVER_USART(SIO1_USART_DRV_NUM);
static sio_status_t   sio1_status;
static const uint32_t sio1_usart_mode = (
  ARM_USART_MODE_ASYNCHRONOUS                  |
  USART_DATA_BITS    (SIO1_USART_DATA_BITS)    |
  USART_PARITY       (SIO1_USART_PARITY)       |
  USART_STOP_BITS    (SIO1_USART_STOP_BITS)    |
  USART_FLOW_CONTROL (SIO1_USART_FLOW_CONTROL)
);
static const uint32_t sio1_usart_baudrate =
                      SIO1_USART_BAUDRATE;
static void sio1_notify (uint32_t event) {
  if (event & ARM_USART_EVENT_SEND_COMPLETE) {
    sio1_status.tx_busy = 0;
  }
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
    sio1_status.rx_cnt += sizeof(sio1_status.rx_buf);
    sio1_status.drv->Receive (sio1_status.rx_buf, sizeof(sio1_status.rx_buf));
  }
}
#endif
#if SIO2_INTERFACE
extern ARM_DRIVER_USART DRIVER_USART(SIO2_USART_DRV_NUM);
static sio_status_t   sio2_status;
static const uint32_t sio2_usart_mode = (
  ARM_USART_MODE_ASYNCHRONOUS                  |
  USART_DATA_BITS    (SIO2_USART_DATA_BITS)    |
  USART_PARITY       (SIO2_USART_PARITY)       |
  USART_STOP_BITS    (SIO2_USART_STOP_BITS)    |
  USART_FLOW_CONTROL (SIO2_USART_FLOW_CONTROL)
);
static const uint32_t sio2_usart_baudrate =
                      SIO2_USART_BAUDRATE;
static void sio2_notify (uint32_t event) {
  if (event & ARM_USART_EVENT_SEND_COMPLETE) {
    sio2_status.tx_busy = 0;
  }
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
    sio2_status.rx_cnt += sizeof(sio2_status.rx_buf);
    sio2_status.drv->Receive (sio2_status.rx_buf, sizeof(sio2_status.rx_buf));
  }
}
#endif

/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
  ARM_USART_SignalEvent_t usart_callback;
  sio_status_t *sio;
  uint32_t usart_mode;
  uint32_t usart_baudrate;
  int32_t  ret;

  switch (devnum) {
#if SIO0_INTERFACE
    case 0U:
      sio      = &sio0_status;
      sio->drv = &DRIVER_USART(SIO0_USART_DRV_NUM);
      usart_mode     = sio0_usart_mode;
      usart_baudrate = sio0_usart_baudrate;
      usart_callback = sio0_notify;
      break;
#endif
#if SIO1_INTERFACE
    case 1U:
      sio      = &sio1_status;
      sio->drv = &DRIVER_USART(SIO1_USART_DRV_NUM);
      usart_mode     = sio1_usart_mode;
      usart_baudrate = sio1_usart_baudrate;
      usart_callback = sio1_notify;
      break;
#endif
#if SIO2_INTERFACE
    case 2U:
      sio      = &sio2_status;
      sio->drv = &DRIVER_USART(SIO2_USART_DRV_NUM);
      usart_mode     = sio2_usart_mode;
      usart_baudrate = sio2_usart_baudrate;
      usart_callback = sio2_notify;
      break;
#endif
    default:
      return NULL;
  }
  sio->rx_cnt   = 0;
  sio->rx_index = 0;
  sio->rx_abort = 0;
  sio->tx_busy  = 0;

  ret = sio->drv->Initialize(usart_callback);
  if (ret != ARM_DRIVER_OK) {
    return NULL;
  }
  ret = sio->drv->PowerControl(ARM_POWER_FULL);
  if (ret != ARM_DRIVER_OK) {
    return NULL;
  }
  ret = sio->drv->Control(usart_mode, usart_baudrate);
  if (ret != ARM_DRIVER_OK) {
    return NULL;
  }
  sio->drv->Control (ARM_USART_CONTROL_TX, 1);
  sio->drv->Control (ARM_USART_CONTROL_RX, 1);
  sio->drv->Receive (sio->rx_buf, sizeof(sio->rx_buf));

  return (sio_fd_t)sio;
}

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
  sio_status_t *sio;

  LWIP_ASSERT("fd != NULL", (fd != NULL));

  sio = (sio_status_t *)fd;

  while (sio->tx_busy) {
    if (sio->tx_busy > 1) {
      sys_msleep(1);
    }
  }
  sio->tx_buf[0] = c;
  sio->tx_busy   = 1;
  sio->drv->Send (sio->tx_buf, 1);
}

/**
 * Receives a single character from the serial device.
 *
 * @param fd serial device handle
 *
 * @note This function will block until a character is received.
 */
u8_t sio_recv(sio_fd_t fd)
{
  sio_status_t *sio;

  LWIP_ASSERT("fd != NULL", (fd != NULL));

  sio = (sio_status_t *)fd;

  while (sio->rx_index == (sio->rx_cnt + sio->drv->GetRxCount())) {
    sys_msleep(1);
  }
  return (sio->rx_buf[sio->rx_index++ & (sizeof(sio->rx_buf)-1)]);
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
  sio_status_t *sio;
  uint32_t nr,n;

  LWIP_ASSERT("fd != NULL", (fd != NULL));

  sio = (sio_status_t *)fd;

  nr = sio->rx_cnt + sio->drv->GetRxCount();
  for (n = 0; (n < len) && !sio->rx_abort; ) {
    if (sio->rx_index == nr) {
      sys_msleep(1);
      nr = sio->rx_cnt + sio->drv->GetRxCount();
      continue;
    }
    data[n++] = sio->rx_buf[sio->rx_index++ & (sizeof(sio->rx_buf)-1)];
  }
  return (n);
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
  sio_status_t *sio;
  uint32_t n,nr;

  LWIP_ASSERT("fd != NULL", (fd != NULL));

  sio = (sio_status_t *)fd;

  nr = sio->rx_cnt + sio->drv->GetRxCount();
  for (n = 0; n < len; ) {
    if (sio->rx_index == nr) {
      break;
    }
    data[n++] = sio->rx_buf[sio->rx_index++ & (sizeof(sio->rx_buf)-1)];
  }
  return (n);
}

/**
 * Writes to the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data to send
 * @param len length (in bytes) of data to send
 * @return number of bytes actually sent
 *
 * @note This function will block until all data can be sent.
 */
u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
  sio_status_t *sio;
  uint32_t n,wi;

  LWIP_ASSERT("fd != NULL", (fd != NULL));

  sio = (sio_status_t *)fd;

  for (n = wi = 0; n < len; ) {
    if (sio->tx_busy) {
      sys_msleep(1);
      continue;
    }
    sio->tx_buf[wi++] = data[n++];
    if ((wi == sizeof(sio->tx_buf)) || (n == len)) {
      sio->tx_busy = (uint8_t)wi;
      sio->drv->Send (sio->tx_buf, wi);
      wi = 0;
    }
  }
  return (len);
}

/**
 * Aborts a blocking sio_read() call.
 *
 * @param fd serial device handle
 */
void sio_read_abort(sio_fd_t fd)
{
  sio_status_t *sio;

  LWIP_ASSERT("fd != NULL", (fd != NULL));

  sio = (sio_status_t *)fd;

  sio->drv->Control(ARM_USART_ABORT_RECEIVE, 0U);
  sio->rx_abort = 1;
}
