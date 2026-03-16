/*
Group 4
Description: PortF switches and LED config

F4 - F3 - F2 - F1 - F0
SW1 - G  - B  - R  - SW2
*/

#include "PortF.h"


void PortF_Init(){
	
	//turn on portF clk
	SYSCTL_RCGCGPIO_R |= PORTF;
	
	//wait for PortF clk to init
	while((SYSCTL_RCGCGPIO_R & PORTF) != PORTF);
	
	// unlock Port F
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	
	// allow changes to PF4–PF0
	GPIO_PORTF_CR_R |= 0x1F;
	
	
	//Configure LEDS as output
	GPIO_PORTF_DIR_R |= LEDS;
	GPIO_PORTF_DEN_R |= LEDS;
	GPIO_PORTF_AMSEL_R &=~ LEDS;
	GPIO_PORTF_AFSEL_R &=~ LEDS;
	
	// Congigure Switches as input
	GPIO_PORTF_DIR_R &=~ SWITCHES;
	GPIO_PORTF_DEN_R |= SWITCHES;
	GPIO_PORTF_AMSEL_R &=~ SWITCHES;
	GPIO_PORTF_AFSEL_R &=~ SWITCHES;
	GPIO_PORTF_PUR_R |= SWITCHES;
	
	//Configure edge interupt for switches
	
	GPIO_PORTF_IS_R &=~ SWITCHES; // Edge interupt
	GPIO_PORTF_IBE_R &=~ SWITCHES; // Noth both edges
	GPIO_PORTF_IEV_R &=~ SWITCHES; // Falling edge
	GPIO_PORTF_ICR_R = SWITCHES; // Clear and previouse interupt flags
	GPIO_PORTF_IM_R |= SWITCHES;// Arm interupt
		
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | LEVEL2;	// Set priority level 2
	NVIC_EN0_R |= 0x40000000; // Enable PortF Handler interupt
	
}
