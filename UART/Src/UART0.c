#include "tm4c123gh6pm.h"
#include "UART0.h"
#include <stdint.h>
#include <stdbool.h>
#include "States.h"
#include "PortF.h"
#include "UART4.h"

#define UART0_NVIC_EN   0x20
#define RB_SIZE         64

extern volatile State CurrentState;
extern volatile bool SENDCOLOR;
extern volatile bool UpdateColorDisplay;
extern volatile uint8_t WheelColor;

typedef struct{
  volatile uint16_t head;
  volatile uint16_t tail;
  volatile uint8_t buf[RB_SIZE];
} RingBuffer;

RingBuffer UART0_RxRB;

//Ring buffer helpers
void rb0_Init(RingBuffer *rb){
  rb->head = 0;
  rb->tail = 0;
}

bool rb0_Empty(const RingBuffer *rb){
  return (rb->head == rb->tail);
}

bool rb0_Full(const RingBuffer *rb){
  return (((rb->head + 1) % RB_SIZE) == rb->tail);
}

bool rb0_Put(RingBuffer *rb, uint8_t data){
  uint16_t next = (rb->head + 1) % RB_SIZE;
  if(next == rb->tail){
    return false;   // buffer full
  }
  rb->buf[rb->head] = data;
  rb->head = next;
  return true;
}

bool rb0_Get(RingBuffer *rb, uint8_t *data){
  if(rb0_Empty(rb)){
    return false;   // buffer empty
  }
  *data = rb->buf[rb->tail];
  rb->tail = (rb->tail + 1) % RB_SIZE;
  return true;
}

//UART0_Init
// UART0 on PA0/PA1
// Baud rate: 9600 for 50 MHz clock
// RX interrupt
// UART0 interrupt priority: Level 0

void UART0_Init(void){
  SYSCTL_RCGCUART_R |= 0x01;   // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01;   // activate Port A
  while((SYSCTL_PRGPIO_R & 0x01) == 0){}

  rb0_Init(&UART0_RxRB);

  UART0_CTL_R &= ~UART_CTL_UARTEN;   // disable UART
  UART0_IBRD_R = 325;                // 50 MHz, 9600 baud
  UART0_FBRD_R = 33;
  UART0_LCRH_R = UART_LCRH_WLEN_8;   
  UART0_ICR_R  = 0x7FF;              // clear all interrupt flags
  UART0_IM_R   = 0x00;               // mask all interrupts first

  GPIO_PORTA_AFSEL_R |= 0x03; //enable alt function
  GPIO_PORTA_DEN_R   |= 0x03;//digital enable
	//select UARt
  GPIO_PORTA_PCTL_R   = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;// disable analog

  UART0_IM_R |= UART_IM_RXIM;        // arm interupt

  NVIC_PRI1_R = (NVIC_PRI1_R & ~0x0000E000); // UART0 priority level 0
  NVIC_EN0_R |= UART0_NVIC_EN;               // enable IRQ 5
	
	//turn on UART
  UART0_CTL_R |= UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN;
}


// Returns true if data exists in RX ring buffer
bool UART0_Available(void){
  return !rb0_Empty(&UART0_RxRB);
}


// Wait for and return one character from RX ring buffer
uint8_t UART0_InChar(void){
  uint8_t data;
  while(!rb0_Get(&UART0_RxRB, &data)){}
  return data;
}


// Output one character to UART0
void UART0_OutChar(uint8_t data){
  while((UART0_FR_R & UART_FR_TXFF) != 0){}
  UART0_DR_R = data;
}


// Output  string
void UART0_OutString(uint8_t *pt){
  while(*pt){
    UART0_OutChar(*pt);
    pt++;
  }
}


// Accept ASCII characters until CR or max-1 chars
void UART0_InString(uint8_t *bufPt, uint16_t max){
  int length = 0;
  char character;

  character = UART0_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART0_OutChar(BS);
      }
    }
    else if(length < (max - 1)){
      *bufPt = character;
      bufPt++;
      length++;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  *bufPt = 0;
}


// Accept unsigned decimal input
uint32_t UART0_InUDec(void){
  uint32_t number = 0, length = 0;
  char character;

  character = UART0_InChar();
  while(character != CR){
    if((character >= '0') && (character <= '9')){
      number = 10*number + (character - '0');
      length++;
      UART0_OutChar(character);
    }
    else if((character == BS) && length){
      number /= 10;
      length--;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  return number;
}


// Output unsigned decimal number
void UART0_OutUDec(uint32_t n){
  if(n >= 10){
    UART0_OutUDec(n/10);
    n = n%10;
  }
  UART0_OutChar(n + '0');
}


// Output CRLF
void UART0_OutCRLF(void){
  UART0_OutChar(CR);
  UART0_OutChar(LF);
}


// RX ISR checks special commands and pushes normal bytes into RX ring buffer
void UART0_Handler(void){
  uint8_t data;

  if(UART0_RIS_R & UART_RIS_RXRIS){
    while((UART0_FR_R & UART_FR_RXFE) == 0){
      data = (uint8_t)(UART0_DR_R & 0xFF);

      // Special handling for Mode 2 exit
      if(data == '^' &&
         (CurrentState == Mode2_Enter ||
          CurrentState == Mode2_MCU1_SelectColor ||
          CurrentState == Mode2_MCU1_WaitReply)){

        // Clear Mode 2 flags
        SENDCOLOR = false;
        UpdateColorDisplay = false;
        WheelColor = 0;

        // Turn off MCU1 LED
        PortF_SetRGB(0,0,0);

        // Tell MCU2 to exit Mode 2
        UART4_OutChar('^');

        // Return MCU1 to main menu
        CurrentState = MainMenu;

        // Do not store '^' in the buffer
      }
      else{
        // Normal data goes into UART0 RX ring buffer
        rb0_Put(&UART0_RxRB, data);
      }
    }

    UART0_ICR_R = UART_ICR_RXIC;
  }

  if(UART0_RIS_R & UART_RIS_TXRIS){
    UART0_ICR_R = UART_ICR_TXIC;
  }
}