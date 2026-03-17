#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART0.h"
#include "UART5.h"
#include "PortF.h"
#include "stdint.h"
#include "stdbool.h"
#include "States.h"

//mode 3 stuff
#define CHAT_MAX 20
char Mode3TxBuffer[CHAT_MAX + 1];
uint8_t Mode3TxIndex = 0;
char Mode3RxBuffer[CHAT_MAX + 1];
uint8_t Mode3RxIndex = 0;

// Function prototypes
void SystemInit(void);
void DelayMs(uint32_t ms);
void setLed(char color);
char getColor(uint8_t c);
void PrintColorName(char c);
void ResetToIdle(void);

//mode 3
void Mode3_ReceiveMessageMCU2(void);
void Mode3_SendMessageMCU2(void);

// Flags
volatile uint8_t WheelColor = 0;
volatile bool SENDCOLOR = false;
volatile bool UpdateColorDisplay = false;
volatile bool Mode3ExitRequest = false;

// Start in idle
volatile State currentState = MCU_IDLE;

// Received char from MCU1
char mcuListen;

int main(void){

    SystemInit();
    DelayMs(100);

    UART0_OutCRLF();
    UART0_OutString((uint8_t*)"Welcome to CECS 447 Project 2 - UART\r\n");
    UART0_OutString((uint8_t*)"MCU2\r\n");
    UART0_OutString((uint8_t*)"Waiting for command from MCU1 ...\r\n");

    while(1){

        switch(currentState){

            case MCU_IDLE:
                // Wait for command from MCU1
                mcuListen = UART5_InChar();

                if(mcuListen == '2'){
                    WheelColor = 0;
                    SENDCOLOR = false;
                    UpdateColorDisplay = false;

                    UART0_OutCRLF();
                    UART0_OutString((uint8_t*)"Mode 2 MCU2\r\n");
                    UART0_OutString((uint8_t*)"Waiting for color code from MCU1 ...\r\n");

                    currentState = MODE2_WAIT_COLOR;
                }
								else if (mcuListen == '3'){
									currentState = MODE3_ENTER;
								}
                break;

					case MODE2_WAIT_COLOR:
							mcuListen = UART5_InChar();

							if(mcuListen == '^'){
									ResetToIdle();
							}
							else{
									setLed(mcuListen);

									UART0_OutString((uint8_t*)"Current color: ");
									PrintColorName(mcuListen);

									switch(mcuListen){
											case 'd': WheelColor = 0; break;
											case 'r': WheelColor = 1; break;
											case 'g': WheelColor = 2; break;
											case 'b': WheelColor = 3; break;
											case 'y': WheelColor = 4; break;
											case 'c': WheelColor = 5; break;
											case 'p': WheelColor = 6; break;
											case 'w': WheelColor = 7; break;
											default:  WheelColor = 0; break;
									}

									UART0_OutString((uint8_t*)"In color Wheel State.\r\n");
									UART0_OutString((uint8_t*)"Please press sw2 to go through the colors in\r\n");
									UART0_OutString((uint8_t*)"the following color wheel: Dark, Red, Green,\r\n");
									UART0_OutString((uint8_t*)"Blue, Yellow, Cyan, Purple, White.\r\n");
									UART0_OutString((uint8_t*)"Once a color is selected, press sw1 to send\r\n");
									UART0_OutString((uint8_t*)"the color to MCU1.\r\n");
									UART0_OutString((uint8_t*)"Current color: ");
									PrintColorName(getColor(WheelColor));

									currentState = MODE2_SELECT_COLOR;
							}
							break;

								case MODE2_SELECT_COLOR:
										// non-blocking check for exit command from MCU1
										if(UART5_Available()){
												mcuListen = UART5_InChar();

												if(mcuListen == '^'){
														ResetToIdle();
														break;
												}
										}

										// SW2 was pressed: update displayed color
										if(UpdateColorDisplay){
												UpdateColorDisplay = false;

												char c = getColor(WheelColor);
												setLed(c);

												UART0_OutString((uint8_t*)"Current color: ");
												PrintColorName(c);
										}

										// SW1 was pressed: send selected color to MCU1
										if(SENDCOLOR){
												SENDCOLOR = false;

												char c = getColor(WheelColor);
												UART5_OutChar(c);

												UART0_OutString((uint8_t*)"Current color: ");
												PrintColorName(c);
												UART0_OutString((uint8_t*)"Waiting for color code from MCU1 ...\r\n");

												currentState = MODE2_WAIT_COLOR;
										}
										break;
										case MODE3_ENTER:
												
												Mode3ExitRequest = false;
												Mode3TxIndex = 0;
												Mode3RxIndex = 0;
												UART0_OutCRLF();
												UART0_OutString((uint8_t*)"Mode 3 MCU2: Chat Room\r\n");
												UART0_OutString((uint8_t*)"Press sw1 at any time to exit the chat room.\r\n");
												UART0_OutCRLF();
												UART0_OutString((uint8_t*)"Waiting for a message from MCU1 ...\r\n");

												currentState = MODE3_LISTEN;
												break;
										case MODE3_LISTEN:
											if(Mode3ExitRequest){
													Mode3ExitRequest = false;
													UART5_OutChar('^');
													UART0_OutCRLF();
													UART0_OutString((uint8_t*)"Exiting Mode 3...\r\n");
											}
											else {
												Mode3_ReceiveMessageMCU2();
												
											}
												
										break;
										case MODE3_TALK:
											if(Mode3ExitRequest){
													Mode3ExitRequest = false;
													UART5_OutChar('^');
													UART0_OutCRLF();
													UART0_OutString((uint8_t*)"Exiting Mode 3...\r\n");
													ResetToIdle();
											}
											else{
													Mode3_SendMessageMCU2();
											}
										break;
										
										
        }//end of switch
    }//end of while
}//end of main

void SystemInit(void){
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
    uint32_t status = GPIO_PORTF_MIS_R;
    GPIO_PORTF_ICR_R = status;

    if(status & SW1){
        DelayMs(15);
        if(currentState == MODE2_SELECT_COLOR){
            SENDCOLOR = true;
        }
				else if (currentState == MODE3_LISTEN || currentState == MODE3_TALK){
					Mode3ExitRequest = true;
				}
    }

    if(status & SW2){
        DelayMs(15);
        if(currentState == MODE2_SELECT_COLOR){
            WheelColor++;
            if(WheelColor >= 8){
                WheelColor = 0;
            }
            UpdateColorDisplay = true;
        }
    }
}

char getColor(uint8_t c){
    if(c > 7) c = 0;

    char Colors[] = {'d','r','g','b','y','c','p','w'};
    return Colors[c];
}

void PrintColorName(char c){
    switch(c){
        case 'd': UART0_OutString((uint8_t*)"Dark\r\n");   break;
        case 'r': UART0_OutString((uint8_t*)"Red\r\n");    break;
        case 'g': UART0_OutString((uint8_t*)"Green\r\n");  break;
        case 'b': UART0_OutString((uint8_t*)"Blue\r\n");   break;
        case 'y': UART0_OutString((uint8_t*)"Yellow\r\n"); break;
        case 'c': UART0_OutString((uint8_t*)"Cyan\r\n");   break;
        case 'p': UART0_OutString((uint8_t*)"Purple\r\n"); break;
        case 'w': UART0_OutString((uint8_t*)"White\r\n");  break;
        default:  UART0_OutString((uint8_t*)"Unknown\r\n"); break;
    }
}

void setLed(char color){
    GPIO_PORTF_DATA_R &= ~LEDS;

    switch(color){
        case 'd':
            GPIO_PORTF_DATA_R &= ~LEDS;
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
            break;

        default:
            UART0_OutString((uint8_t*)"Invalid color\r\n");
            break;
    }
}
void ResetToIdle(void){
    SENDCOLOR = false;
    UpdateColorDisplay = false;
    WheelColor = 0;

    GPIO_PORTF_DATA_R &= ~LEDS;

    UART0_OutCRLF();
    UART0_OutString((uint8_t*)"Welcome to CECS 447 Project 2 - UART\r\n");
    UART0_OutString((uint8_t*)"MCU2\r\n");
    UART0_OutString((uint8_t*)"Waiting for command from MCU1 ...\r\n");

    currentState = MCU_IDLE;
}

// Mode 3 process incoming msg
void Mode3_ReceiveMessageMCU2(void){
    char c;

    if(UART5_Available()){
        c = UART5_InChar();

        if(c == '^'){
            UART0_OutCRLF();
            UART0_OutString((uint8_t*)"MCU1 exited Mode 3...\r\n");
            Mode3RxIndex = 0;
            ResetToIdle();
            return;
        }

        if(c != '\r'){
            if(Mode3RxIndex < CHAT_MAX){
                Mode3RxBuffer[Mode3RxIndex] = c;
                Mode3RxIndex++;
            }
        }
        else{
            Mode3RxBuffer[Mode3RxIndex] = 0;

            UART0_OutCRLF();
            UART0_OutString((uint8_t*)"Message from MCU1: ");
            UART0_OutString((uint8_t*)Mode3RxBuffer);
            UART0_OutCRLF();

            Mode3RxIndex = 0;
            currentState = MODE3_TALK;

            UART0_OutString((uint8_t*)"Please type a message end with a return\r\n");
            UART0_OutString((uint8_t*)"(less than 20 characters):\r\n");
        }
    }
}
void Mode3_SendMessageMCU2(void){
    char c;

    if(UART0_Available()){
        c = UART0_InChar();
        UART0_OutChar(c);

        if(c != '\r'){
            if(Mode3TxIndex < CHAT_MAX){
                Mode3TxBuffer[Mode3TxIndex] = c;
                Mode3TxIndex++;
            }
        }
        else{
            Mode3TxBuffer[Mode3TxIndex] = 0;

            UART5_OutString((uint8_t*)Mode3TxBuffer);
            UART5_OutChar('\r');

            UART0_OutCRLF();

            Mode3TxIndex = 0;
            currentState = MODE3_LISTEN;

            UART0_OutString((uint8_t*)"Waiting for a message from MCU1 ...\r\n");
        }
    }
}