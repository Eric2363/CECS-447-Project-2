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

typedef enum{
    MCU2_IDLE,
    MODE2_WAIT_COLOR,
    MODE2_SELECT_COLOR,
    MODE3_CHAT
} STATE;

STATE CurrentState = MCU2_IDLE;

int main(void){

    SystemInit();
    DelayMs(100);


while(1){

    char c;

    switch(CurrentState){

        case MCU2_IDLE:

            // waiting for MCU1 to start Mode2
            c = UART5_InChar();

            if(c == '2'){
                UART0_OutString((uint8_t*)"Mode 2 MCU2\r\n");
                UART0_OutString((uint8_t*)"Waiting for color code from MCU1 ...\r\n");

                CurrentState = MODE2_WAIT_COLOR;
            }

        break;


        case MODE2_WAIT_COLOR:

            c = UART5_InChar();

            // MCU1 exited mode
            if(c == '^'){
                GPIO_PORTF_DATA_R &= ~LEDS;
                CurrentState = MCU2_IDLE;
                break;
            }

            LedColor(c);

            UART0_OutString((uint8_t*)"Mode 2 MCU2\r\n");
            UART0_OutString((uint8_t*)"In color Wheel State.\r\n");
            UART0_OutString((uint8_t*)"Please press sw2 to go through the colors in the following color wheel: Dark, Red, Green, Blue, Yellow, Cyan, Purple, White.\r\n");
            UART0_OutString((uint8_t*)"Once a color is selected, press sw1 to send the color to MCU1.\r\n");

            UART0_OutString((uint8_t*)"Current color: ");

            switch(c){
                case 'd': UART0_OutString((uint8_t*)"Dark\r\n"); break;
                case 'r': UART0_OutString((uint8_t*)"Red\r\n"); break;
                case 'g': UART0_OutString((uint8_t*)"Green\r\n"); break;
                case 'b': UART0_OutString((uint8_t*)"Blue\r\n"); break;
                case 'y': UART0_OutString((uint8_t*)"Yellow\r\n"); break;
                case 'c': UART0_OutString((uint8_t*)"Cyan\r\n"); break;
                case 'p': UART0_OutString((uint8_t*)"Purple\r\n"); break;
                case 'w': UART0_OutString((uint8_t*)"White\r\n"); break;
            }

            CurrentState = MODE2_SELECT_COLOR;

        break;


        case MODE2_SELECT_COLOR:

            if(SENDCOLOR){

                SENDCOLOR = false;

                char color;

                switch(WheelColor){
                    case 0: color = 'd'; break;
                    case 1: color = 'r'; break;
                    case 2: color = 'g'; break;
                    case 3: color = 'b'; break;
                    case 4: color = 'y'; break;
                    case 5: color = 'c'; break;
                    case 6: color = 'p'; break;
                    case 7: color = 'w'; break;
                }

                UART5_OutChar(color);

                UART0_OutString((uint8_t*)"Mode 2 MCU2\r\n");
                UART0_OutString((uint8_t*)"Current color: ");

                switch(color){
                    case 'd': UART0_OutString((uint8_t*)"Dark\r\n"); break;
                    case 'r': UART0_OutString((uint8_t*)"Red\r\n"); break;
                    case 'g': UART0_OutString((uint8_t*)"Green\r\n"); break;
                    case 'b': UART0_OutString((uint8_t*)"Blue\r\n"); break;
                    case 'y': UART0_OutString((uint8_t*)"Yellow\r\n"); break;
                    case 'c': UART0_OutString((uint8_t*)"Cyan\r\n"); break;
                    case 'p': UART0_OutString((uint8_t*)"Purple\r\n"); break;
                    case 'w': UART0_OutString((uint8_t*)"White\r\n"); break;
                }

                UART0_OutString((uint8_t*)"Waiting for color code from MCU1 ...\r\n");

                CurrentState = MODE2_WAIT_COLOR;
            }

        break;


        case MODE3_CHAT:

            // placeholder for Mode3
            break;
    }
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