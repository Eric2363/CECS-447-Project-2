
////module Test


////#define UART_TEST
////#define PORTF_TEST
//#define MENU_TEST
////#define MODE_ONE_TEST
////#define MODE_TWO_TEST
////#define MODE_THREE_TEST
////#define PWM_TEST

////Includes
//#include "tm4c123gh6pm.h"
//#include "stdint.h"
//#include "PLL.h"
//#include "string.h"

//#if defined (UART_TEST)
//	#include "Uart.h"
//#endif

//#if  defined (PORTF_TEST)
//	#include "PortF.h"
//#endif

//#if defined (MENU_TEST)
//	#include "Uart.h"
//#endif


////Function prototypes

//void System_Init(void);
//void DelayMs(uint32_t ms);

////Uart test
//#if defined (UART_TEST)
//	void Uart_Test(void);
//#endif

////PortF test
//#if defined (PORTF_TEST)
//	void PortF_Test(void);
//#endif

////Menu Test
//#if defined (MENU_TEST)
//	void Menu_Test(void);
//#endif

////Mode 1 Test
//#if defined (MODE_ONE_TEST)
//	void Mode_One_Test(void);
//#endif
////Main test loop

//int main(void){
//	System_Init();
//	
//	while(1){
//		//Uart Test
//		#if defined (UART_TEST)
//			Uart_Test();
//		#endif
//		// PortF Test
//		#if defined (PORTF_TEST)
//			PortF_Test();
//		#endif
//		//Menu Test
//		#if defined(MENU_TEST)
//			Menu_Test();
//		#endif
//	}
//}

//void System_Init(void){
//	
//	PLL_Init();
//	
//	#if defined(MENU_TEST)
//	UART_Init(false,false);
//	#endif
//}

//#if defined (UART_TEST)
//void Uart_Test(){
//	UART_Init(false,false);
//	UART_OutString("TEST\r\n");
//}
//#endif

//	
//#if defined (PORTF_TEST)
//void PortF_Test(){
//	
//	PortF_Init();
//	
//	GPIO_PORTF_DATA_R &=~LEDS;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |= RED;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |= BLUE;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |=GREEN;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |=CYAN;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |=YELLOW;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |=PURPLE;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |=LEDS;
//	DelayMs(500);
//	GPIO_PORTF_DATA_R &=~LEDS;
//	GPIO_PORTF_DATA_R |=LEDS;
//	
//}
//#endif

//#if defined(MENU_TEST)
//	typedef enum{
//	MAINMENU,
//	MODE1,
//	MODE2,
//	MODE3
//	}state;
//	
//	//current menu state
//	state currentState = MAINMENU;
//	
//void Menu_Test(){
//	
//	//store a string of max length 40
//	uint8_t buffer[40];
//	//Choose menu options
//	while(1){
//		
//		switch(currentState){
//			
//			case MAINMENU:
//				//print main menu
//				OutCRLF();
//				UART_OutString((uint8_t*)"Main Menu Test\n\r");
//			
//				//get user input string and put into buffer
//				UART_InString(buffer,sizeof(buffer)-1);
//			
//				if(strcmp((char*)buffer,"1") == 0){
//						OutCRLF();
//					currentState = MODE1;
//				}
//				if(strcmp((char*)buffer,"2") == 0){
//						OutCRLF();
//					currentState = MODE2;
//				}
//				if(strcmp((char*)buffer,"3") == 0){
//						OutCRLF();
//					currentState = MODE3;
//				}
//				break;
//			case MODE1:
//				UART_OutString((uint8_t*)"Mode 1 Menu Test\n\r");
//				//get user input string and put into buffer
//				UART_InString(buffer,sizeof(buffer)-1);
//				OutCRLF();
//				
//				if(strcmp((char*)buffer,"1") == 0){
//					UART_OutString((uint8_t*) "Color Select mode\n\r");
//					OutCRLF();
//					//select color
//					
//				}
//				if(strcmp((char*)buffer,"2") == 0){
//					UART_OutString((uint8_t*) "Brightness mode\n\r");
//					OutCRLF();
//					//setupt brightness
//				}
//				if(strcmp((char*)buffer,"3") == 0){
//					UART_OutString((uint8_t*) "Exiting.....\n\r");
//					OutCRLF();
//					//exit back to main menu
//					currentState = MAINMENU;
//				}
//				break;
//			case MODE2:
//				UART_OutString((uint8_t*)"Mode 2 Menu Test\n\r");
//				//get user input string and put into buffer
//				UART_OutString((uint8_t*)"Color wheel mode\n\r");
//			//will use sw2 to interate and choose color of led in round robin.

//				//will need to be handled in UART interupt
//				UART_OutString((uint8_t*)"Press ^ to exit mode\n\r");
//			//get user input
//				UART_InString(buffer,sizeof(buffer)-1);
//				OutCRLF();
//				if(strcmp((char*)buffer,"^") == 0){
//					currentState = MAINMENU;
//				}
//				
//				break;
//			case MODE3: break;
//			
//		}
//	}
//	//Get user input
//	UART_InString(buffer,sizeof(buffer)-1);
//	OutCRLF();
//	
//	

//}
//#endif
////General purpose delay function
//void DelayMs(uint32_t ms){
//    volatile uint32_t count;
//    
//    while(ms--){
//        count = 12500;   // ~1ms at 50MHz
//        while(count--){
//        }
//    }
//}
//	

