#include "tm4c123gh6pm.h"
#include "UART4.h"
#include <stdint.h>
#include <stdbool.h>
#include "States.h"



typedef struct{
  volatile uint16_t head;
  volatile uint16_t tail;
  volatile uint8_t buf[RB_SIZE];
} RingBuffer;

RingBuffer UART4_RxRB;


void rb4_Init(RingBuffer *rb){
  rb->head = 0;
  rb->tail = 0;
}

bool rb4_Empty(const RingBuffer *rb){
  return (rb->head == rb->tail);
}

bool rb4_Full(const RingBuffer *rb){
  return (((rb->head + 1) % RB_SIZE) == rb->tail);
}

bool rb4_Put(RingBuffer *rb, uint8_t data){
  uint16_t next = (rb->head + 1) % RB_SIZE;
  if(next == rb->tail){
    return false;
  }
  rb->buf[rb->head] = data;
  rb->head = next;
  return true;
}

bool rb4_Get(RingBuffer *rb, uint8_t *data){
  if(rb4_Empty(rb)){
    return false;
  }
  *data = rb->buf[rb->tail];
  rb->tail = (rb->tail + 1) % RB_SIZE;
  return true;
}


// UART4 on PC4/PC5
// Baud rate: 14400 for 50 MHz clock
// UART4 interrupt priority: Level 1
void UART4_Init(void){
  SYSCTL_RCGCUART_R |= 0x10;   // activate UART4
  SYSCTL_RCGCGPIO_R |= 0x04;   // activate Port C
  while((SYSCTL_PRGPIO_R & 0x04) == 0){}

  rb4_Init(&UART4_RxRB);

  GPIO_PORTC_LOCK_R = 0x4C4F434B;
  GPIO_PORTC_CR_R  |= 0x30;

  UART4_CTL_R &= ~UART_CTL_UARTEN;   // disable UART
  UART4_IBRD_R = 217;                // 50 MHz, 14400 baud
  UART4_FBRD_R = 1;
  UART4_LCRH_R = UART_LCRH_WLEN_8;   // 8-bit, no parity, 1 stop, FIFO off
  UART4_ICR_R  = 0x7FF;              // clear all interrupt flags
  UART4_IM_R   = 0x00;               // mask all interrupts first

  GPIO_PORTC_AFSEL_R |= 0x30;
  GPIO_PORTC_DEN_R   |= 0x30;
  GPIO_PORTC_PCTL_R   = (GPIO_PORTC_PCTL_R & 0xFF00FFFF) + 0x00110000;
  GPIO_PORTC_AMSEL_R &= ~0x30;

  UART4_IM_R |= UART_IM_RXIM;        // arm interupt

  // priority level 1
  NVIC_PRI15_R = (NVIC_PRI15_R & ~0xE0000000) | 0x20000000;

  // Enable IRQ 60 
  NVIC_EN1_R |= UART4_NVIC_EN;

//turn on uart
  UART4_CTL_R |= UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN;
}


bool UART4_Available(void){
  return !rb4_Empty(&UART4_RxRB);
}


uint8_t UART4_InChar(void){
  uint8_t data;
  while(!rb4_Get(&UART4_RxRB, &data)){}
  return data;
}


void UART4_OutChar(uint8_t data){
  while((UART4_FR_R & UART_FR_TXFF) != 0){}
  UART4_DR_R = data;
}


void UART4_OutString(uint8_t *pt){
  while(*pt){
    UART4_OutChar(*pt);
    pt++;
  }
}


void UART4_InString(uint8_t *bufPt, uint16_t max){
  int length = 0;
  char character;

  character = UART4_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART4_OutChar(BS);
      }
    }
    else if(length < (max - 1)){
      *bufPt = character;
      bufPt++;
      length++;
      UART4_OutChar(character);
    }
    character = UART4_InChar();
  }
  *bufPt = 0;
}


uint32_t UART4_InUDec(void){
  uint32_t number = 0, length = 0;
  char character;

  character = UART4_InChar();
  while(character != CR){
    if((character >= '0') && (character <= '9')){
      number = 10*number + (character - '0');
      length++;
      UART4_OutChar(character);
    }
    else if((character == BS) && length){
      number /= 10;
      length--;
      UART4_OutChar(character);
    }
    character = UART4_InChar();
  }
  return number;
}


void UART4_OutUDec(uint32_t n){
  if(n >= 10){
    UART4_OutUDec(n/10);
    n = n%10;
  }
  UART4_OutChar(n + '0');
}


void UART4_OutCRLF(void){
  UART4_OutChar(CR);
  UART4_OutChar(LF);
}


// RX ISR pushes bytes into RX ring buffer
void UART4_Handler(void){
  if(UART4_RIS_R & UART_RIS_RXRIS){
    while((UART4_FR_R & UART_FR_RXFE) == 0){
      rb4_Put(&UART4_RxRB, (uint8_t)(UART4_DR_R & 0xFF));
    }
    UART4_ICR_R = UART_ICR_RXIC;
  }

  if(UART4_RIS_R & UART_RIS_TXRIS){
    UART4_ICR_R = UART_ICR_TXIC;
  }
}