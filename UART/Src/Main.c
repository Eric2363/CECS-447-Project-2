
//#include "tm4c123gh6pm.h"
//#include "PLL.h"
//#include "Uart.h"
//#include "PortF.h"
//#include "string.h"



//void SystemInit(void);
//void printMenu(void);
//void printColorMenu(void);

//#define MAX_LEN 40

//typedef enum states{
//	MainMenu,
//	Mode1,
//	Mode2,
//	Mode3,
//	Wait
//	
//}state;

//state State = MainMenu;
//int main(){

//	SystemInit();

//	UART_OutString((uint8_t*)COLOR_RESET);

//		
//	while(1){
//		
//		switch(State){
//			
//			case MainMenu: printMenu();break;
//			case Mode1: printColorMenu();break;
//			case Mode2: break;
//			case Mode3: break;
//			case Wait: break;
//		}

//		

//		



//	}
//	
//	

//	return 0;
//}


//void SystemInit(){

//	PLL_Init();
//	UART_Init(false,false);
//	PortF_Init();

//}
//void printMenu(){
//	//buffer to save user input string
//	uint8_t buffer[MAX_LEN];
//	UART_OutString((uint8_t*)"Welcome to CECS 447 Project 2 -UART\n\r");
//	UART_OutString((uint8_t*)"MCU-1\n\r");
//	UART_OutString((uint8_t*)"Main Menu\n\r");
//	UART_OutString((uint8_t*)"1. PC <-> MCU-1 LED Control.\n\r");
//	UART_OutString((uint8_t*)"2. MCU-1 <-> MCU-2 Color Wheel.\n\r");
//	UART_OutString((uint8_t*)"3. PC-1 <-> MCU-1 <-> MCU-2 <PC-2 Chat Room\n\r");
//	UART_OutString((uint8_t*)"Please choose a communication mode\n\r");
//	UART_OutString((uint8_t*)"(enter 1 or 2 or 3):");
//	//print menu then change state to wait to stop it printing again
//	
//	//get user input
//	UART_InString(buffer,sizeof(buffer) -1);
//	//compare what is in the buffer to a string
//	//change to Mode 1
//	if(strcmp((char*)buffer,"1") ==0){
//		State = Mode1;
//	}
//	//change to mode 2
//	if(strcmp((char*)buffer,"2") ==0){
//		State = Mode2;
//	}
//	//change to mode 3
//	if(strcmp((char*)buffer,"3") ==0){
//		State = Mode3;
//	}
//}
//void printColorMenu(){
//	  
//		uint8_t name[MAX_LEN];
//		UART_OutString((uint8_t*)"Color Options: Blue, Red, Green, White");
//		OutCRLF();
//		UART_OutString((uint8_t*)"Enter a color");
//		OutCRLF();
//		
//    UART_InString(name, sizeof(name) - 1);   // IMPORTANT: pass max-1
//                                        // prevents overflow bug
//		UART_OutString((uint8_t*)"You entered: ");
//		UART_OutString(name);
//		OutCRLF();
//		
//		if(strcmp((char*)name,"Blue")==0){
//			UART_OutString((uint8_t*)COLOR_BLUE);
//			GPIO_PORTF_DATA_R &=~LEDS;
//			GPIO_PORTF_DATA_R |= BLUE;
//			
//		}
//		if(strcmp((char*)name,"Red")==0){
//			UART_OutString((uint8_t*)COLOR_RED);
//			GPIO_PORTF_DATA_R &=~LEDS;
//			GPIO_PORTF_DATA_R |= RED;
//		}
//		if(strcmp((char*)name,"Green")==0){
//			UART_OutString((uint8_t*)COLOR_GREEN);
//			GPIO_PORTF_DATA_R &=~LEDS;
//			GPIO_PORTF_DATA_R |= GREEN;
//		}
//		if(strcmp((char*)name,"White")==0){
//			UART_OutString((uint8_t*)COLOR_WHITE);
//			GPIO_PORTF_DATA_R &=~LEDS;
//			GPIO_PORTF_DATA_R |= LEDS;
//		}
//		
//	
//}