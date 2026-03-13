#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Uart.h"
#include "PortF.h"
#include "stdint.h"
#include "stdbool.h"

void SystemInit(void);
void printMenu(void);
void DelayMs(uint32_t ms);

typedef enum states{
	MainMenu,
	Mode1,
	Mode2,
	Mode3
} state;

state State = MainMenu;

volatile bool red;
volatile bool blue;
volatile bool green;


int main(){

	SystemInit();

	UART_OutString((uint8_t*)COLOR_RESET);

	printMenu();



	
	while(1){

if(red){
    PortF_SetDuty(1,100);
}
else if(blue){
    PortF_SetDuty(2,50);
}
else if(green){
    PortF_SetDuty(3,10);
}
else{
    PortF_SetDuty(1,0); // or turn all off however you prefer
}
		
	}
}

void SystemInit(){

	PLL_Init();
	UART_Init(true,false);
	PortF_Init();
}

void printMenu(){

	UART_OutString((uint8_t*)"Welcome to CECS 447 Project 2\n\r");
	UART_OutString((uint8_t*)"1 - LED Control\n\r");
	UART_OutString((uint8_t*)"2 - Color Wheel\n\r");
	UART_OutString((uint8_t*)"3 - Chat Room\n\r");
	UART_OutString((uint8_t*)"Choose mode: ");
}

void DelayMs(uint32_t ms){
    volatile uint32_t count;
    
    while(ms--){
        count = 12500;   // ~1ms at 50MHz
        while(count--){
        }
    }
}
void GPIOPortF_Handler(void)
{
    uint32_t status = GPIO_PORTF_MIS_R; // latch cause
    GPIO_PORTF_ICR_R = status;          // clear flags

    if(status & SW1){
        red = !red;
    }
    if(status & SW2){
        green = !green;
    }
}

// optional alias if your startup expects GPIOF_Handler instead:
void GPIOF_Handler(void)
{
    GPIOPortF_Handler();
}
