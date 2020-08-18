 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  USART configuration                              *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#include <string.h>
#include "stm32f10x.h"
#include "usart.h"
#include "define.h"
#include "globals.h"


/*----------------------------------------------------------------------------
  Notes:
  The length of the receive and transmit buffers must be a power of 2.
  Each buffer has a next_in and a next_out index.
  If next_in = next_out, the buffer is empty.
  (next_in - next_out) % buffer_size = the number of characters in the buffer.
 *----------------------------------------------------------------------------*/
#define TBUF_SIZE   256	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   256      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

/*----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
#if TBUF_SIZE < 2
#error TBUF_SIZE is too small.  It must be larger than 1.
#elif ((TBUF_SIZE & (TBUF_SIZE-1)) != 0)
#error TBUF_SIZE must be a power of 2.
#endif

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
struct buf_st {
  unsigned int in;                        /* Next In Index                    */
  unsigned int out;                       /* Next Out Index                   */
  char buf [RBUF_SIZE];                   /* Buffer                           */
};

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

static struct buf_st tbuf = { 0, 0, };
#define SIO_TBUFLEN ((unsigned short)(tbuf.in - tbuf.out))

static unsigned int tx1_restart = 1;       /* NZ if TX restart is required     */
static unsigned int tx2_restart = 1;       /* NZ if TX restart is required     */


// ----------------------------------------------------------------------------
// Initialize USART1 pins, Baudrate
// ----------------------------------------------------------------------------
void USART1_Init (void) {
    int i;

    RCC->APB2ENR |=  0x00000001;            // enable clock Alternate Function
    AFIO->MAPR   &= ~0x00000004;            // clear USART1 remap

    RCC->APB2ENR |=  0x00000004;            // enable GPIOA clock
    GPIOA->CRH   &= ~0x00000FF0;            // clear PA9, PA10
    GPIOA->CRH   |=  0x000000B0;            // USART1 Tx (PA9) output push-pull
    GPIOA->CRH   |=  0x00000400;            // USART1 Rx (PA10) input floating

    RCC->APB2ENR |=  0x00004000;            // enable USART#1 clock

    USART1->BRR   = 0x0208;                 // 115200 baud @ PCLK2 60MHz (Only USART1 is clocked with PCLK2 (72 MHz Max). Other USARTs are clocked with PCLK1 (36 MHz Max).)
    USART1->CR1   = ((1 <<  2) |            // enable RX
                    (1 <<  3)  |            // enable TX
                    (1 <<  5)  |            // enable RXNE Interrupt
                    (1 <<  7)  |            // enable TXE Interrupt
                    (0 << 12)  );           // 1 start bit, 8 data bits
    USART1->CR2   = 0x0000;                 // 1 stop bit
    USART1->CR3   = 0x0000;                 // no flow control
    for (i = 0; i < 0x1000; i++) __NOP();   // avoid unwanted output

    NVIC_EnableIRQ(USART1_IRQn);
    USART1->CR1  |= ((   1 << 13) );        // enable USART
}

// ----------------------------------------------------------------------------
// USART1_IRQHandler
// Handles USART1 global interrupt request.
// ----------------------------------------------------------------------------
void USART1_IRQHandler (void) {
  volatile unsigned int IIR;
  struct buf_st *p;

    IIR = USART1->SR;
    if (IIR & USART_SR_RXNE) {            /* read interrupt                   */
      USART1->SR &= ~USART_SR_RXNE;	      /* clear interrupt                  */

      p = &rbuf;

      if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
        p->buf [p->in & (RBUF_SIZE-1)] = (USART1->DR & 0x1FF);
        p->in++;
      }
    }

    if (IIR & USART_SR_TXE) {
      USART1->SR &= ~USART_SR_TXE;	      /* clear interrupt                    */

      p = &tbuf;

      if (p->in != p->out) {
        USART1->DR = (p->buf [p->out & (TBUF_SIZE-1)] & 0x1FF);
        p->out++;
        tx1_restart = 0;
      }
      else {
        tx1_restart = 1;
        USART1->CR1 &= ~USART_SR_TXE;     /* disable TX IRQ if nothing to send  */

      }
    }
}






// ----------------------------------------------------------------------------
// Initialize USART2 pins, Baudrate
// ----------------------------------------------------------------------------
void USART2_Init (void) {
    int i;

    RCC->APB2ENR |=  0x00000001;            // enable clock Alternate Function
    AFIO->MAPR   &= ~0x00000008;            // clear USART2 remap

    RCC->APB2ENR |=  0x00000004;            // enable GPIOA clock
    GPIOA->CRL   &= ~0x0000FF00;            // clear PA2, PA3
    GPIOA->CRL   |=  0x00000B00;            // USART2 Tx (PA2) output push-pull
    GPIOA->CRL   |=  0x00004000;            // USART2 Rx (PA3) input floating

    RCC->APB1ENR |=  0x00020000;            // enable USART#2 clock

    USART2->BRR   = 0x0104;                 // 115200 baud @ PCLK1 30MHz (Only USART1 is clocked with PCLK2 (72 MHz Max). Other USARTs are clocked with PCLK1 (36 MHz Max).)
    USART2->CR1   = ((1 <<  2) |            // enable RX
                    ( 1 <<  3) |            // enable TX
                    ( 1 <<  5) |            // enable RXNE Interrupt
                    ( 1 <<  7) |            // enable TXE Interrupt
                    ( 0 << 12) );           // 1 start bit, 8 data bits
    USART2->CR2   = 0x0000;                 // 1 stop bit
    USART2->CR3   = 0x0000;                 // no flow control
    for (i = 0; i < 0x1000; i++) __NOP();   // avoid unwanted output

    NVIC_EnableIRQ(USART2_IRQn);
    USART2->CR1  |= ((   1 << 13) );        // enable USART
}

// ----------------------------------------------------------------------------
// USART2_IRQHandler
// Handles USART1 global interrupt request.
// ----------------------------------------------------------------------------
void USART2_IRQHandler (void) {
  volatile unsigned int IIR;
  struct buf_st *p;

    IIR = USART2->SR;
    if (IIR & USART_SR_RXNE) {            /* read interrupt                   */
      USART2->SR &= ~USART_SR_RXNE;	      /* clear interrupt                  */

      p = &rbuf;

      if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
        p->buf [p->in & (RBUF_SIZE-1)] = (USART2->DR & 0x1FF);
        p->in++;
      }
    }

    if (IIR & USART_SR_TXE) {
      USART2->SR &= ~USART_SR_TXE;	      /* clear interrupt                    */

      p = &tbuf;

      if (p->in != p->out) {
        USART2->DR = (p->buf [p->out & (TBUF_SIZE-1)] & 0x1FF);
        p->out++;
        tx2_restart = 0;
      }
      else {
        tx2_restart = 1;
        USART2->CR1 &= ~USART_SR_TXE;     /* disable TX IRQ if nothing to send  */

      }
    }
}















// ----------------------------------------------------------------------------
// buffer_Init
// initialize the buffers
// ----------------------------------------------------------------------------
void buffer_Init (void) {

  tbuf.in = 0;                            /* Clear com buffer indexes           */
  tbuf.out = 0;
  tx1_restart = 1;

  rbuf.in = 0;
  rbuf.out = 0;
}

// ----------------------------------------------------------------------------
// SendChar
// transmit a character
// ----------------------------------------------------------------------------
int SendChar1 (int c) {
  struct buf_st *p = &tbuf;

  if (SIO_TBUFLEN >= TBUF_SIZE)           /* If the buffer is full              */
    return (-1);                          /* return an error value              */
                                                  
  p->buf [p->in & (TBUF_SIZE - 1)] = c;   /* Add data to the transmit buffer.   */
  p->in++;

  if (tx1_restart) {                       /* If TX interrupt is disabled        */
    tx1_restart = 0;                       /*     enable it                      */
    USART1->CR1 |= USART_SR_TXE;          /* enable TX interrupt                */
  }

  return (0);
}


// ----------------------------------------------------------------------------
// GetKey
// receive a character
// ----------------------------------------------------------------------------
int GetKey1 (void) {
  struct buf_st *p = &rbuf;
	//LED_BT_ON;
  if (SIO_RBUFLEN == 0)
    return (-1);

  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}



// ----------------------------------------------------------------------------
// SendChar
// transmit a character
// ----------------------------------------------------------------------------
int SendChar2 (int c) {
  struct buf_st *p = &tbuf;

  if (SIO_TBUFLEN >= TBUF_SIZE)           /* If the buffer is full              */
    return (-1);                          /* return an error value              */
                                                  
  p->buf [p->in & (TBUF_SIZE - 1)] = c;   /* Add data to the transmit buffer.   */
  p->in++;

  if (tx2_restart) {                       /* If TX interrupt is disabled        */
    tx2_restart = 0;                       /*     enable it                      */
    USART2->CR1 |= USART_SR_TXE;          /* enable TX interrupt                */
  }

  return (0);
}


// ----------------------------------------------------------------------------
// Send #len bytes
// transmit a character
// ----------------------------------------------------------------------------
int SendUart2 (uint8_t *array, char len) {
    do{
        SendChar2(*(array++));
    }while(--len);
    return (0);
}





// ----------------------------------------------------------------------------
// GetKey
// receive a character
// ----------------------------------------------------------------------------
int GetKey2 (void) {
  struct buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);

  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}

