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

#ifndef SIO_CONFIG_H
#define SIO_CONFIG_H

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <e>Serial I/O Interface 0
#define SIO0_INTERFACE          1

//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for hardware interface
#define SIO0_USART_DRV_NUM      0

//   <o>USART Baudrate
#define SIO0_USART_BAUDRATE     115200

//   <o>USART Data Bits <8=>8
#define SIO0_USART_DATA_BITS    8

//   <o>USART Parity <0=>None <1=>Even <2=>Odd
#define SIO0_USART_PARITY       0

//   <o>USART Stop Bits <1=>1 <2=>2
#define SIO0_USART_STOP_BITS    1

//   <o>USART Flow Control <0=>None <1=>RTS <2=>CTS <3=>RTS/CTS
#define SIO0_USART_FLOW_CONTROL 0

// </e>

// <e>Serial I/O Interface 1
#define SIO1_INTERFACE          1

//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for hardware interface
#define SIO1_USART_DRV_NUM      1

//   <o>USART Baudrate
#define SIO1_USART_BAUDRATE     115200

//   <o>USART Data Bits <8=>8
#define SIO1_USART_DATA_BITS    8

//   <o>USART Parity <0=>None <1=>Even <2=>Odd
#define SIO1_USART_PARITY       0

//   <o>USART Stop Bits <1=>1 <2=>2
#define SIO1_USART_STOP_BITS    1

//   <o>USART Flow Control <0=>None <1=>RTS <2=>CTS <3=>RTS/CTS
#define SIO1_USART_FLOW_CONTROL 0

// </e>

// <e>Serial I/O Interface 2
#define SIO2_INTERFACE          2

//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for hardware interface
#define SIO2_USART_DRV_NUM      2

//   <o>USART Baudrate
#define SIO2_USART_BAUDRATE     115200

//   <o>USART Data Bits <8=>8
#define SIO2_USART_DATA_BITS    8

//   <o>USART Parity <0=>None <1=>Even <2=>Odd
#define SIO2_USART_PARITY       0

//   <o>USART Stop Bits <1=>1 <2=>2
#define SIO2_USART_STOP_BITS    1

//   <o>USART Flow Control <0=>None <1=>RTS <2=>CTS <3=>RTS/CTS
#define SIO2_USART_FLOW_CONTROL 0

// </e>

//------------- <<< end of configuration section >>> ---------------------------

#endif
