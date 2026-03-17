#include "tm4c123gh6pm.h"
#include "UART5.h"
#include <stdint.h>
#include <stdbool.h>

#define UART5_NVIC_EN   0x20000000
#define RB_SIZE         64



typedef struct{
  volatile uint16_t head;
  volatile uint16_t tail;
  volatile uint8_t buf[RB_SIZE];
} RingBuffer;

RingBuffer UART5_RxRB;

// ---------- Ring buffer helpers ----------
void rb5_Init(RingBuffer *rb){
  rb->head = 0;
  rb->tail = 0;
}

bool rb5_Empty(const RingBuffer *rb){
  return (rb->head == rb->tail);
}

bool rb5_Full(const RingBuffer *rb){
  return (((rb->head + 1) % RB_SIZE) == rb->tail);
}

bool rb5_Put(RingBuffer *rb, uint8_t data){
  uint16_t next = (rb->head + 1) % RB_SIZE;
  if(next == rb->tail){
    return false;
  }
  rb->buf[rb->head] = data;
  rb->head = next;
  return true;
}

bool rb5_Get(RingBuffer *rb, uint8_t *data){
  if(rb5_Empty(rb)){
    return false;
  }
  *data = rb->buf[rb->tail];
  rb->tail = (rb->tail + 1) % RB_SIZE;
  return true;
}

//------------UART5_Init------------
// Initialize UART5 on PE4Rx/PE5Tx
// Baud rate = 14400 for 50 MHz clock
// RX interrupt always enabled
// UART5 interrupt priority = 1
void UART5_Init(void){
  SYSCTL_RCGCUART_R |= 0x20;   // activate UART5
  SYSCTL_RCGCGPIO_R |= 0x10;   // activate Port E
  while((SYSCTL_PRGPIO_R & 0x10) == 0){}

  rb5_Init(&UART5_RxRB);

  UART5_CTL_R &= ~UART_CTL_UARTEN;   // disable UART
  UART5_IBRD_R = 217;                // 50 MHz, 14400 baud
  UART5_FBRD_R = 1;
  UART5_LCRH_R = UART_LCRH_WLEN_8;   // 8-bit, no parity, 1 stop, FIFO off
  UART5_ICR_R  = 0x7FF;              // clear all interrupt flags
  UART5_IM_R   = 0x00;               // mask all interrupts first

  GPIO_PORTE_AFSEL_R |= 0x30;
  GPIO_PORTE_DEN_R   |= 0x30;
  GPIO_PORTE_PCTL_R   = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) + 0x00110000;
  GPIO_PORTE_AMSEL_R &= ~0x30;

  UART5_IM_R |= UART_IM_RXIM;        // force RX interrupt enabled

  // UART5 priority level 1
  NVIC_PRI15_R = (NVIC_PRI15_R & ~0x0000E000) | 0x00002000;

  // Enable IRQ 61 (UART5)
  NVIC_EN1_R |= UART5_NVIC_EN;

  UART5_CTL_R |= UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN;
}

//------------UART5_Available------------
bool UART5_Available(void){
  return !rb5_Empty(&UART5_RxRB);
}

//------------UART5_InChar------------
uint8_t UART5_InChar(void){
  uint8_t data;
  while(!rb5_Get(&UART5_RxRB, &data)){}
  return data;
}

//------------UART5_OutChar------------
void UART5_OutChar(uint8_t data){
  while((UART5_FR_R & UART_FR_TXFF) != 0){}
  UART5_DR_R = data;
}

//------------UART5_OutString------------
void UART5_OutString(uint8_t *pt){
  while(*pt){
    UART5_OutChar(*pt);
    pt++;
  }
}

//------------UART5_InString------------
void UART5_InString(uint8_t *bufPt, uint16_t max){
  int length = 0;
  char character;

  character = UART5_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART5_OutChar(BS);
      }
    }
    else if(length < (max - 1)){
      *bufPt = character;
      bufPt++;
      length++;
      UART5_OutChar(character);
    }
    character = UART5_InChar();
  }
  *bufPt = 0;
}

//------------UART5_InUDec------------
uint32_t UART5_InUDec(void){
  uint32_t number = 0, length = 0;
  char character;

  character = UART5_InChar();
  while(character != CR){
    if((character >= '0') && (character <= '9')){
      number = 10*number + (character - '0');
      length++;
      UART5_OutChar(character);
    }
    else if((character == BS) && length){
      number /= 10;
      length--;
      UART5_OutChar(character);
    }
    character = UART5_InChar();
  }
  return number;
}

//------------UART5_OutUDec------------
void UART5_OutUDec(uint32_t n){
  if(n >= 10){
    UART5_OutUDec(n/10);
    n = n%10;
  }
  UART5_OutChar(n + '0');
}

//------------UART5_OutCRLF------------
void UART5_OutCRLF(void){
  UART5_OutChar(CR);
  UART5_OutChar(LF);
}

//------------UART5_Handler------------
// RX ISR pushes bytes into RX ring buffer
void UART5_Handler(void){
  if(UART5_RIS_R & UART_RIS_RXRIS){
    while((UART5_FR_R & UART_FR_RXFE) == 0){
      rb5_Put(&UART5_RxRB, (uint8_t)(UART5_DR_R & 0xFF));
    }
    UART5_ICR_R = UART_ICR_RXIC;
  }

  if(UART5_RIS_R & UART_RIS_TXRIS){
    UART5_ICR_R = UART_ICR_TXIC;
  }
}
