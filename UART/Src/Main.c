#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART0.h"
#include "UART4.h"
#include "PortF.h"
#include "stdint.h"
#include "stdbool.h"
#include "States.h"

//mode 3 stuff
#define CHAT_MAX 20
char Mode3TxBuffer[CHAT_MAX + 1];// outgoing msg buffer
uint8_t Mode3TxIndex = 0;// current index in tx buffer
char Mode3RxBuffer[CHAT_MAX + 1];// incoming msg buffer
uint8_t Mode3RxIndex = 0;// current index in rx buffer

//Variables & flags
//access in ISR or main
volatile State CurrentState = MainMenu; //currentstate. starts in main menu
char USERINPUT; //var to get char from user
char mcu2Reply; // var to hold char from mcu2
volatile uint8_t WheelColor = 0; //current index of wheel color
volatile bool Mode3ExitRequest = false; // set by sw1 to exit mode 3
volatile bool SENDCOLOR = false; //set by sw2 to send color to mcu2
volatile bool UpdateColorDisplay = false; // set by sw2 to update color displayed on screen


//function prototypes
void SystemInit(void);
void DelayMs(uint32_t ms);
void LedColor(char color);
void LedBrightness(uint8_t brightness);
void Mode2SetLED(char response);
char getColor(uint8_t c);
uint8_t ColorToIndex(char c);

//mode 3
void Mode3_SendMessageMCU1(void);
void Mode3_ReceiveMessageMCU1(void);

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
				
					//if user input is 1 then go to mode1
					if(USERINPUT == '1'){
						CurrentState = Mode1;
					}
					//if user input is 2 then go to mode 2
					else if (USERINPUT == '2'){
						CurrentState = Mode2_Enter;
					}
					//if user input is 3 then go to mode 3
					else if (USERINPUT == '3'){
						CurrentState = Mode3_MCU1_Enter;
					}
				break;
					//Mode 1 logic print menu and decide option
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
				
					//decide what option to do
					//choose led color
					if(USERINPUT == '1'){
						UART0_OutCRLF();
						UART0_OutString((uint8_t*)"Please Select a color from the following list:\r\n");
						UART0_OutString((uint8_t*)"d(dark), r(red), g(green), b(blue), y(yellow), c(cyan), p(purple), w(white)\r\n");
						UART0_OutCRLF(); // extra space
						//get user input
						USERINPUT = UART0_InChar();
						UART0_OutChar(USERINPUT);//echo
						char enter = UART0_InChar(); // check if user presses enter
						UART0_OutCRLF();
						if(enter == '\r'){
							LedColor(USERINPUT);// update led color
						}
						
					}
					//choose brightness for current led
					else if (USERINPUT == '2'){
						UART0_OutCRLF();
						UART0_OutString((uint8_t*)"Please enter a decimal number from 0 to 100 followed by a return\r\n");
						uint8_t brightness = UART0_InUDec();//get user input and get decimal number
						
						UART0_OutCRLF();
						
						LedBrightness(brightness);// update led brightness
						
						
						
					}// check if user input 3 then exit to main menue
					else if (USERINPUT == '3'){
						
						UART0_OutString((uint8_t*)"Exiting........\r\n");
						CurrentState = MainMenu;
						UART0_OutCRLF();
						

					}
				break;
					// Start of mode 2, print menu and current color
				case Mode2_Enter:
					
						SENDCOLOR = false; // set flag off, since not sending color data yet.
						UpdateColorDisplay = false;// update screen
						WheelColor = 0;// keep track of current color, start at dark

						// print menu
						UART0_OutCRLF();
						UART0_OutString((uint8_t*)"Mode 2 MCU1: press ^ to exit this mode\r\n");
						UART0_OutString((uint8_t*)"In color Wheel State.\r\n");
						UART0_OutString((uint8_t*)"Please press sw2 to go through the colors in\r\n");
						UART0_OutString((uint8_t*)"the following color wheel: Dark, Red, Green,\r\n");
						UART0_OutString((uint8_t*)"Blue, Yellow, Cyan, Purple, White.\r\n");
						UART0_OutString((uint8_t*)"Once a color is selected, press sw1 to send\r\n");
						UART0_OutString((uint8_t*)"the color to MCU2.\r\n");
						UART0_OutString((uint8_t*)"Current color: Dark\r\n");

						Mode2SetLED('d'); // update led

						UART4_OutChar('2');   // tell MCU2 to enter mode 2
						CurrentState = Mode2_MCU1_SelectColor;
						break;

						// select a color by pressing sw2 and update
						case Mode2_MCU1_SelectColor:
							// print current color everytime color updates
								if(UpdateColorDisplay){
										UpdateColorDisplay = false;
										
									// get current color wheel index and update led
										char c = getColor(WheelColor);
										Mode2SetLED(c);
									// print current color
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
								// if sw1 is pressed send current color to mcu 2
								if(SENDCOLOR){
										SENDCOLOR = false;
										// get char of current wheel color  and send to mcu 2
										char c = getColor(WheelColor);
										UART4_OutChar(c);

										UART0_OutString((uint8_t*)"Waiting for color code from MCU2 ...\r\n");
										CurrentState = Mode2_MCU1_WaitReply;
								}
								break;
								// wait for mcu 2 to send something
								case Mode2_MCU1_WaitReply:
									// this is a non blocking . if something is in buffer then read
										if(UART4_Available()){
												mcu2Reply = UART4_InChar();

												Mode2SetLED(mcu2Reply);

												// sync wheel position to received color
												WheelColor = ColorToIndex(mcu2Reply);

											// print new led color
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
										
										// start of mode 3
									case Mode3_MCU1_Enter:
											Mode3ExitRequest = false; // exit flag set by sw1
											Mode3TxIndex = 0;// current index in Tx buffer
											Mode3RxIndex = 0;// current index in Rx buffer
											UART0_OutCRLF();
											// print menu
											UART0_OutString((uint8_t*)"Mode 3 MCU1: Chat Room\r\n");
											UART0_OutString((uint8_t*)"Press sw1 at any time to exit the chat room.\r\n");
											UART0_OutCRLF();
											UART0_OutString((uint8_t*)"Please type a message end with a return\r\n");
											UART0_OutString((uint8_t*)"(less than 20 characters):\r\n");

											UART4_OutChar('3');   // tell MCU2 to enter mode 3

											CurrentState = Mode3_MCU1_Talk;
										
										
											break;
									// mcu1's turn to talk 
									case Mode3_MCU1_Talk:
												//non blocking uArt check
										    if(UART4_Available()){
														//read data from mcu2
														mcu2Reply = UART4_InChar();
														//check for exit char then exit if true
														if(mcu2Reply == '^'){
																UART0_OutCRLF();
																Mode3RxIndex = 0;
																Mode3TxIndex = 0;
																CurrentState = MainMenu;
																break;
														}
												}
												
												// if sw1 exit is pressed tell mcu2 to reset
												if(Mode3ExitRequest){
													Mode3ExitRequest = false;
													UART4_OutChar('^');
													UART0_OutCRLF();
													UART0_OutString((uint8_t*)"Exiting Mode 3...\r\n");
													Mode3RxIndex = 0;
													Mode3TxIndex = 0;
													CurrentState = MainMenu;
												}
												else {
													Mode3_SendMessageMCU1();
												}
												break;
									case Mode3_MCU1_Listen:
											if(Mode3ExitRequest){
													Mode3ExitRequest = false;
													UART4_OutChar('^');
													UART0_OutCRLF();
													UART0_OutString((uint8_t*)"Exiting Mode 3...\r\n");
													CurrentState = MainMenu;
											}
											else{
													Mode3_ReceiveMessageMCU1();
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
            UART0_OutString((uint8_t*)"Dark Led is on\r\n");
            break;

        case 'r':
						currentColor = ColorRed;
						PortF_SetRGB(100,0,0);
            UART0_OutString((uint8_t*)"Red Led is on\r\n");
            break;

        case 'g':
						currentColor = ColorGreen;
						PortF_SetRGB(0,0,100);
            UART0_OutString((uint8_t*)"Green Led is on\r\n");
            break;

        case 'b':
						currentColor = ColorBlue;
						PortF_SetRGB(0,100,0);
            UART0_OutString((uint8_t*)"Blue Led is on\r\n");
            break;
				
				case 'y':
						currentColor = ColorYellow;
						PortF_SetRGB(100,0,100);
						UART0_OutString((uint8_t*)"Yellow Led is on\r\n");
						break;
				case 'c':
						currentColor = ColorCyan;
						PortF_SetRGB(0,100,100);
						UART0_OutString((uint8_t*)"Cyan Led is on\r\n");
						break;
				case 'p':
						currentColor = ColorPurple;
						PortF_SetRGB(100,100,0);
						UART0_OutString((uint8_t*)"Purple Led is on\r\n");
						break;
				case 'w':
						currentColor = ColorWhite;
						PortF_SetRGB(100,100,100);
						UART0_OutString((uint8_t*)"White Led is on\r\n");
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
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorRed:
			PortF_SetRGB(brightness,0,0);
			UART0_OutString((uint8_t*)"Red Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorGreen:
			PortF_SetRGB(0,0,brightness);
			UART0_OutString((uint8_t*)"Green Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorBlue:
			PortF_SetRGB(0,brightness,0);
			UART0_OutString((uint8_t*)"Blue Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorYellow:
			PortF_SetRGB(brightness,0,brightness);
			UART0_OutString((uint8_t*)"Yellow Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorCyan:
			PortF_SetRGB(0,brightness,brightness);
			UART0_OutString((uint8_t*)"Cyan Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorPurple:
			PortF_SetRGB(brightness,brightness,0);
			UART0_OutString((uint8_t*)"Purple Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
			break;
		case ColorWhite:
			PortF_SetRGB(brightness,brightness,brightness);
			UART0_OutString((uint8_t*)"White Led is displayed at ");
			UART0_OutUDec(brightness);
			UART0_OutString((uint8_t*)"% brightness.\r\n");
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
// maps user color input to the index
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
//===============PortF Handler=====================
void GPIOPortF_Handler(void){
    uint32_t status = GPIO_PORTF_MIS_R;
    GPIO_PORTF_ICR_R = status;

    if(status & SW1){
        DelayMs(15);
        if(CurrentState == Mode2_MCU1_SelectColor){
            SENDCOLOR = true;
        }
				//if in mode 3 and sw1 pressed exit back to main 
				else if(CurrentState == Mode3_MCU1_Talk || CurrentState == Mode3_MCU1_Listen){
					Mode3ExitRequest = true;
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

//=================Mode3 Helpers=====================
// send data to mcu2
void Mode3_SendMessageMCU1(void){
	char c;

	// non blocking uart check if data in  ring buffer
	if(UART0_Available()){
		
		c = UART0_InChar();
		//make text purple for mcu 1
		UART0_OutString((uint8_t*)COLOR_MAGENTA);
		UART0_OutChar(c);
		UART0_OutString((uint8_t*)COLOR_RESET);

		// check if end of msg and if 20 char length
		if(c != '\r'){
			if(Mode3TxIndex < CHAT_MAX){
				Mode3TxBuffer[Mode3TxIndex] = c;
				Mode3TxIndex++;
			}
		}
		// send string
		else{
			Mode3TxBuffer[Mode3TxIndex] = 0;

			UART4_OutString((uint8_t*)Mode3TxBuffer);
			UART4_OutChar('\r');

			UART0_OutCRLF();

			Mode3TxIndex = 0;
			CurrentState = Mode3_MCU1_Listen;
		}
	}
}
// get data from mcu 2
void Mode3_ReceiveMessageMCU1(void){
	char c;

	// non blocking , check if data in buffer
	if(UART4_Available()){
		c = UART4_InChar();
		// check mcu2 exited mode
		if(c == '^'){
			UART0_OutCRLF();
			//UART0_OutString((uint8_t*)"MCU2 exited Mode 3...\r\n");
			CurrentState = MainMenu;
			Mode3RxIndex = 0;
			return;
		}

		// check for end of msg
		if(c != '\r'){
			if(Mode3RxIndex < CHAT_MAX){
				Mode3RxBuffer[Mode3RxIndex] = c;
				Mode3RxIndex++;
			}
		}
		else{
			Mode3RxBuffer[Mode3RxIndex] = 0;

			UART0_OutCRLF();
			//make mcu2 msgs green
			UART0_OutString((uint8_t*)COLOR_GREEN);
			UART0_OutString((uint8_t*)"Message from MCU2: ");
			UART0_OutString((uint8_t*)Mode3RxBuffer);
			UART0_OutString((uint8_t*)COLOR_RESET);
			UART0_OutCRLF();

			Mode3RxIndex = 0;
			CurrentState = Mode3_MCU1_Talk;

			UART0_OutString((uint8_t*)"Please type a message end with a return\r\n");
			UART0_OutString((uint8_t*)"(less than 20 characters):\r\n");
		}
	}
}
