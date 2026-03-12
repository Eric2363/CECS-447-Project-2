//#include "tm4c123gh6pm.h"
//#include "PLL.h"
//#include "Uart.h"
//#include "PortF.h"

//void SystemInit(void);
//void printMenu(void);

//typedef enum states{
//	MainMenu,
//	Mode1,
//	Mode2,
//	Mode3
//} state;

//state State = MainMenu;

//int main(){

//	SystemInit();

//	UART_OutString((uint8_t*)COLOR_RESET);

//	printMenu();

//	while(1){

//		char c = UART_InChar();   // read one char from ring buffer

//		if(State == MainMenu){

//			if(c == '1'){
//				State = Mode1;
//				UART_OutString((uint8_t*)"\n\rMode 1 selected\n\r");
//			}

//			else if(c == '2'){
//				State = Mode2;
//				UART_OutString((uint8_t*)"\n\rMode 2 selected\n\r");
//			}

//			else if(c == '3'){
//				State = Mode3;
//				UART_OutString((uint8_t*)"\n\rMode 3 selected\n\r");
//			}
//		}

//		if(State == Mode1){

//			if(c == 'b' || c == 'B'){
//				GPIO_PORTF_DATA_R &= ~LEDS;
//				GPIO_PORTF_DATA_R |= BLUE;
//				UART_OutString((uint8_t*)COLOR_BLUE);
//				UART_OutChar(c);
//			}

//			if(c == 'r' || c == 'R'){
//				GPIO_PORTF_DATA_R &= ~LEDS;
//				GPIO_PORTF_DATA_R |= RED;
//				UART_OutString((uint8_t*)COLOR_RED);
//			}

//			if(c == 'g' || c == 'G'){
//				GPIO_PORTF_DATA_R &= ~LEDS;
//				GPIO_PORTF_DATA_R |= GREEN;
//				UART_OutString((uint8_t*)COLOR_GREEN);
//			}

//			if(c == 'w' || c == 'W'){
//				GPIO_PORTF_DATA_R &= ~LEDS;
//				GPIO_PORTF_DATA_R |= LEDS;
//				UART_OutString((uint8_t*)COLOR_WHITE);
//			}
//		}
//	}
//}

//void SystemInit(){

//	PLL_Init();
//	UART_Init(true,false);
//	PortF_Init();
//}

//void printMenu(){

//	UART_OutString((uint8_t*)"Welcome to CECS 447 Project 2\n\r");
//	UART_OutString((uint8_t*)"1 - LED Control\n\r");
//	UART_OutString((uint8_t*)"2 - Color Wheel\n\r");
//	UART_OutString((uint8_t*)"3 - Chat Room\n\r");
//	UART_OutString((uint8_t*)"Choose mode: ");
//}