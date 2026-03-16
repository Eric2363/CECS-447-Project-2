#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART0.h"
#include "UART5.h"
#include "PortF.h"
#include "stdint.h"
#include "stdbool.h"

void SystemInit(void);

void DelayMs(uint32_t ms);
void LedColor(char color);
bool ColorWheelSelected = false;
volatile uint8_t WheelColor = 0;

volatile bool SENDCOLOR = false;



typedef enum{
    LISTEN,
    TALK
} MODE;

int main(void){

    SystemInit();
    DelayMs(100);


    while(1){

	
			
			char c = UART5_InChar();
			LedColor(c);
			
			
			
    }

}

void SystemInit(){

	PLL_Init();
	PortF_Init();
	UART0_Init();
	UART5_Init();
	
}



void DelayMs(uint32_t ms){
    volatile uint32_t count;
    
    while(ms--){
        count = 12500;   // ~1ms at 50MHz
        while(count--){
        }
    }
}
void GPIOPortF_Handler(void){
    uint32_t status = GPIO_PORTF_MIS_R; // latch cause
    GPIO_PORTF_ICR_R = status;          // clear flags
		
    if(status & SW1){
			DelayMs(20);
			
			SENDCOLOR = true;
       
    }
    if(status & SW2){
			DelayMs(20);
			WheelColor +=1;
			
			if(WheelColor == 8){
				WheelColor = 0;
			}
        
    }
}

void LedColor(char color){
	GPIO_PORTF_DATA_R &=~LEDS;
    switch(color){

        case 'd':
						
						GPIO_PORTF_DATA_R &=~LEDS;
            
            break;

        case 'r':
						
						GPIO_PORTF_DATA_R = RED;
            
            break;

        case 'g':
						GPIO_PORTF_DATA_R = GREEN;
            
            break;

        case 'b':
						GPIO_PORTF_DATA_R = BLUE;
            
            break;
				
				case 'y':
						GPIO_PORTF_DATA_R = YELLOW;
						
						break;
				case 'c':
						GPIO_PORTF_DATA_R = CYAN;
						
						break;
				case 'p':
						GPIO_PORTF_DATA_R = PURPLE;
						
						break;
				case 'w':
						GPIO_PORTF_DATA_R = LEDS;
						
    }
}