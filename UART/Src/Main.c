#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART0.h"
#include "UART4.h"
#include "PortF.h"
#include "stdint.h"
#include "stdbool.h"
#include "States.h"

//Variables declared
//acess in ISR or main
volatile State CurrentState = MainMenu;
char USERINPUT;
char mcu2Reply;
bool ColorWheelSelected = false;
volatile uint8_t WheelColor = 0;

volatile bool SENDCOLOR = false;

volatile bool UpdateColorDisplay = false;

//function prototypes
void SystemInit(void);
void DelayMs(uint32_t ms);
void LedColor(char color);
void LedBrightness(uint8_t brightness);
void Mode2SetLED(char response);
char getColor(uint8_t c);
uint8_t ColorToIndex(char c);

//Keep track of currently turned on color
typedef enum Color{
	ColorDark,
	ColorRed,
	ColorGreen,
	ColorBlue,
	ColorYellow,
	ColorCyan,
	ColorPurple,
	ColorWhite
	
}COLOR;


//starting color
COLOR currentColor = ColorDark;

//TODO: I need to make the switch case neater
int main(void){

    SystemInit();
    DelayMs(100);
    while(1){

			//Menu Control
			switch(CurrentState){
				//Print Main Menu
				case MainMenu:
					UART0_OutCRLF();
					UART0_OutString((uint8_t*)"Welcome to CECS 447 Project 2 - UART\r\n");
					UART0_OutString((uint8_t*)"MCU 1\r\n");
					UART0_OutString((uint8_t*)"Main Menu\r\n");
					UART0_OutString((uint8_t*)"1: PC <--> MCU1 LED Control\r\n");
					UART0_OutString((uint8_t*)"2: MCU1 <--> MCU2 Color Wheel\r\n");
					UART0_OutString((uint8_t*)"3: PC1 <--> MCU1 <--> MCU2 <--> PC2 Chat Room\r\n");
					UART0_OutString((uint8_t*)"Please choose communication mode\r\n");
					UART0_OutString((uint8_t*)"(enter 1 or 2 or 3)\r\n");
					//Extra Space
					UART0_OutCRLF();
					//get User input and decide what state to move to
					USERINPUT = UART0_InChar();
					UART0_OutChar(USERINPUT); //echo what the user typed
				
					if(USERINPUT == '1'){
						CurrentState = Mode1;
					}
					else if (USERINPUT == '2'){
						CurrentState = Mode2_Enter;
					}
					else if (USERINPUT == '3'){
					//	CurrentState = Mode3;
					}
				break;
					
				case Mode1:
					UART0_OutCRLF();
					UART0_OutString((uint8_t*)"Mode 1 Menu\r\n");
					UART0_OutString((uint8_t*)"Please select an option from the following list (enter 1 or 2 or 3)\r\n");
					UART0_OutString((uint8_t*)"1: Choose and LED Color.\r\n");
					UART0_OutString((uint8_t*)"2: Change the brightness of current LED(s).\r\n");
					UART0_OutString((uint8_t*)"3: Exit\r\n");
					//Extra space and echo back
					UART0_OutCRLF();
					USERINPUT = UART0_InChar();
					UART0_OutChar(USERINPUT);
					UART0_OutCRLF();
				
					//decide what mode to do
					//choose led color
					if(USERINPUT == '1'){
						UART0_OutCRLF();
						UART0_OutString((uint8_t*)"Please Select a color from the following list:\r\n");
						UART0_OutString((uint8_t*)"d(dark), r(red), g(green), b(blue), y(yellow), c(cyan), p(purple), w(white)\r\n");
						UART0_OutCRLF(); // extra space
						//get user input
						USERINPUT = UART0_InChar();
						UART0_OutChar(USERINPUT);
						char enter = UART0_InChar();
						UART0_OutCRLF();
						if(enter == '\r'){
							LedColor(USERINPUT);
						}
						
					}
					//choose brightness for current led
					else if (USERINPUT == '2'){
						UART0_OutCRLF();
						UART0_OutString((uint8_t*)"Please enter a decimal number from 0 to 100 followed by a return\r\n");
						uint8_t brightness = UART0_InUDec();
						
						UART0_OutCRLF();
						
						LedBrightness(brightness);
						
						
						
					}
					else if (USERINPUT == '3'){
						
						UART0_OutString((uint8_t*)"Exiting........\r\n");
						CurrentState = MainMenu;
						UART0_OutCRLF();
						

					}
				break;
				case Mode2_Enter:
						SENDCOLOR = false;
						UpdateColorDisplay = false;
						WheelColor = 0;

						UART0_OutCRLF();
						UART0_OutString((uint8_t*)"Mode 2 MCU1: press ^ to exit this mode\r\n");
						UART0_OutString((uint8_t*)"In color Wheel State.\r\n");
						UART0_OutString((uint8_t*)"Please press sw2 to go through the colors in\r\n");
						UART0_OutString((uint8_t*)"the following color wheel: Dark, Red, Green,\r\n");
						UART0_OutString((uint8_t*)"Blue, Yellow, Cyan, Purple, White.\r\n");
						UART0_OutString((uint8_t*)"Once a color is selected, press sw1 to send\r\n");
						UART0_OutString((uint8_t*)"the color to MCU2.\r\n");
						UART0_OutString((uint8_t*)"Current color: Dark\r\n");

						Mode2SetLED('d');

						UART4_OutChar('2');   // tell MCU2 to enter mode 2
						CurrentState = Mode2_MCU1_SelectColor;
						break;

						case Mode2_MCU1_SelectColor:
								if(UpdateColorDisplay){
										UpdateColorDisplay = false;

										char c = getColor(WheelColor);
										Mode2SetLED(c);

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
								}

								if(SENDCOLOR){
										SENDCOLOR = false;

										char c = getColor(WheelColor);
										UART4_OutChar(c);

										UART0_OutString((uint8_t*)"Waiting for color code from MCU2 ...\r\n");
										CurrentState = Mode2_MCU1_WaitReply;
								}
								break;
								case Mode2_MCU1_WaitReply:
										if(UART4_Available()){
												mcu2Reply = UART4_InChar();

												Mode2SetLED(mcu2Reply);

												// sync wheel position to received color
												WheelColor = ColorToIndex(mcu2Reply);

												UART0_OutString((uint8_t*)"Current color: ");
												switch(mcu2Reply){
														case 'd': UART0_OutString((uint8_t*)"Dark\r\n"); break;
														case 'r': UART0_OutString((uint8_t*)"Red\r\n"); break;
														case 'g': UART0_OutString((uint8_t*)"Green\r\n"); break;
														case 'b': UART0_OutString((uint8_t*)"Blue\r\n"); break;
														case 'y': UART0_OutString((uint8_t*)"Yellow\r\n"); break;
														case 'c': UART0_OutString((uint8_t*)"Cyan\r\n"); break;
														case 'p': UART0_OutString((uint8_t*)"Purple\r\n"); break;
														case 'w': UART0_OutString((uint8_t*)"White\r\n"); break;
												}

												UART0_OutString((uint8_t*)"In color Wheel State.\r\n");
												CurrentState = Mode2_MCU1_SelectColor;
										}
										break;

			}//end of switch


			
    }//end of while

}//end of main

void SystemInit(){

	PLL_Init();
	PortF_Init();
	UART0_Init();
	UART4_Init();
}

//used for mode 1 color selection and keep track of current color
void LedColor(char color){
    switch(color){

        case 'd':
						currentColor = ColorDark;
						PortF_SetRGB(0,0,0);
            UART0_OutString((uint8_t*)"Color changed to Dark\r\n");
            break;

        case 'r':
						currentColor = ColorRed;
						PortF_SetRGB(100,0,0);
            UART0_OutString((uint8_t*)"Color changed to Red\r\n");
            break;

        case 'g':
						currentColor = ColorGreen;
						PortF_SetRGB(0,0,100);
            UART0_OutString((uint8_t*)"Color changed to Green\r\n");
            break;

        case 'b':
						currentColor = ColorBlue;
						PortF_SetRGB(0,100,0);
            UART0_OutString((uint8_t*)"Color changed to Blue\r\n");
            break;
				
				case 'y':
						currentColor = ColorYellow;
						PortF_SetRGB(100,0,100);
						UART0_OutString((uint8_t*)"Color changed to Yellow\r\n");
						break;
				case 'c':
						currentColor = ColorCyan;
						PortF_SetRGB(0,100,100);
						UART0_OutString((uint8_t*)"Color changed to Cyan\r\n");
						break;
				case 'p':
						currentColor = ColorPurple;
						PortF_SetRGB(100,100,0);
						UART0_OutString((uint8_t*)"Color changed to Purple\r\n");
						break;
				case 'w':
						currentColor = ColorWhite;
						PortF_SetRGB(100,100,100);
						UART0_OutString((uint8_t*)"Color changed to White\r\n");
						break;
        default:
            UART0_OutString((uint8_t*)"Invalid color\r\n");
            break;
    }
}
//used for mode 1 to change brighness
void LedBrightness(uint8_t brightness){
	
	switch(currentColor){
		case ColorDark:
			PortF_SetRGB(0,0,0);
			UART0_OutString((uint8_t*)"Dark Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorRed:
			PortF_SetRGB(brightness,0,0);
			UART0_OutString((uint8_t*)"Red Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorGreen:
			PortF_SetRGB(0,0,brightness);
			UART0_OutString((uint8_t*)"Green Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorBlue:
			PortF_SetRGB(0,brightness,0);
			UART0_OutString((uint8_t*)"Blue Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorYellow:
			PortF_SetRGB(brightness,0,brightness);
			UART0_OutString((uint8_t*)"Yellow Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorCyan:
			PortF_SetRGB(0,brightness,brightness);
			UART0_OutString((uint8_t*)"Cyan Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorPurple:
			PortF_SetRGB(brightness,brightness,0);
			UART0_OutString((uint8_t*)"Purple Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
		case ColorWhite:
			PortF_SetRGB(brightness,brightness,brightness);
			UART0_OutString((uint8_t*)"White Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"brightness.\r\n");
			break;
	}
}
//==================================Mode 2 Function helpers=================================
//return char of color
char getColor(uint8_t c){
	//clamp to 0 if greater than 7
	if(c > 7) c = 0;
	
	char Colors[] = {'d','r','g','b','y','c','p','w'};
	
	return Colors[c];
	
}
//turn on led depending on char input
void Mode2SetLED(char response){
	
	switch(response){
		case 'd': PortF_SetRGB(0,0,0);break;
		case 'r': PortF_SetRGB(100,0,0);break;
		case 'g': PortF_SetRGB(0,0,100);break;
		case 'b': PortF_SetRGB(0,100,0);break;
		case 'y': PortF_SetRGB(100,0,100);break;
		case 'c': PortF_SetRGB(0,100,100);break;
		case 'p': PortF_SetRGB(100,100,0);break;
		case 'w': PortF_SetRGB(100,100,100);break;
	}
}
uint8_t ColorToIndex(char c){
    switch(c){
        case 'd': return 0;
        case 'r': return 1;
        case 'g': return 2;
        case 'b': return 3;
        case 'y': return 4;
        case 'c': return 5;
        case 'p': return 6;
        case 'w': return 7;
        default:  return 0;
    }
}
//general purpose delay
void DelayMs(uint32_t ms){
    volatile uint32_t count;
    
    while(ms--){
        count = 12500;   // ~1ms at 50MHz
        while(count--){
        }
    }
}
void GPIOPortF_Handler(void){
    uint32_t status = GPIO_PORTF_MIS_R;
    GPIO_PORTF_ICR_R = status;

    if(status & SW1){
        DelayMs(15);
        if(CurrentState == Mode2_MCU1_SelectColor){
            SENDCOLOR = true;
        }
    }

    if(status & SW2){
        DelayMs(15);
        if(CurrentState == Mode2_MCU1_SelectColor){
            WheelColor++;
            if(WheelColor >= 8){
                WheelColor = 0;
            }
            UpdateColorDisplay = true;
        }
    }
}


