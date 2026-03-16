
#include "tm4c123gh6pm.h"
#include "PortF.h"
#include <stdint.h>




// Initialize PortF leds,switchs and PWM for LEDs
void PortF_Init(void){
	
    // Enable PWM1 and Port F clocks
    SYSCTL_RCGCPWM_R |= 0x02;   // PWM1
    SYSCTL_RCGCGPIO_R |= PORTF; // PortF
    while((SYSCTL_PRGPIO_R & PORTF) == 0){} // wait for port ready

    // Unlock Port F and allow changes to PF4-0
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x1F;

    // Configure PF1-3 as PWM alternate function 
    GPIO_PORTF_DIR_R   |= LEDS;     // set as output 
    GPIO_PORTF_DEN_R   |= LEDS;	//Enable
    GPIO_PORTF_AMSEL_R &= ~LEDS; //disable analog mode
    GPIO_PORTF_AFSEL_R |= LEDS;  // enable alternate function for PF1-3
    
			//enable pwm option
    GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & ~0x0000FFF0) | 0x00005550;

    // Configure switches PF4 & PF0 as inputs
    GPIO_PORTF_DIR_R &= ~SWITCHES;
    GPIO_PORTF_DEN_R |= SWITCHES;
    GPIO_PORTF_AMSEL_R &= ~SWITCHES;
    GPIO_PORTF_AFSEL_R &= ~SWITCHES;
    GPIO_PORTF_PUR_R |= SWITCHES;

    // Configure edge interrupts for switches (falling edge)
    GPIO_PORTF_IS_R &= ~SWITCHES;    // edge-sensitive
    GPIO_PORTF_IBE_R &= ~SWITCHES;   // not both edges
    GPIO_PORTF_IEV_R &= ~SWITCHES;   // falling edge
    GPIO_PORTF_ICR_R = SWITCHES;     // clear any prior flags
    GPIO_PORTF_IM_R |= SWITCHES;     // arm interrupts


    // interupt level 2
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | LEVEL2;
    NVIC_EN0_R |= 0x40000000; // Enable Port F IRQ

    //PWM generator setup 
    // Use PWM1 generators 2 and 3:
    // PF1 -> M1PWM5 -> PWM1 generator 2 output B CMPB
    // PF2 -> M1PWM6 -> PWM1 generator 3 output A CMPA
    // PF3 -> M1PWM7 -> PWM1 generator 3 output B CMPB

		// Gen2 PWM5: PF1
    PWM1_2_CTL_R  = 0;                 // disable Gen2 during config
    
    PWM1_2_GENB_R = 0x00000C08;        //LOW at LOAD, HIGH at CMPB-down
    PWM1_2_LOAD_R = PWM_LOAD;
    PWM1_2_CMPB_R = 0;                 // 0% duty
    PWM1_2_CTL_R |= 1;                 // enable Gen2

		// Gen3 PWM6 & PWM7: PF2 &PF3
    PWM1_3_CTL_R  = 0;                 // disable Gen3 during config
    PWM1_3_GENA_R = 0x000000C8;        // LOW at LOAD, HIGH at CMPA-down
    PWM1_3_GENB_R = 0x00000C08;        // LOW at LOAD, HIGH at CMPB-down
    PWM1_3_LOAD_R = PWM_LOAD;
    PWM1_3_CMPA_R = 0;                 // 0% duty
    PWM1_3_CMPB_R = 0;                 // 0% duty
    PWM1_3_CTL_R |= 1;                 // enable Gen3

    // Enable outputs: PWM5, PWM6, PWM7
    PWM1_ENABLE_R |= (1<<5) | (1<<6) | (1<<7);
}



// Set RBG LED brightness 
// red, blue, green: duty cycle 0=OFF, 100=FULL
void PortF_SetRGB(uint8_t red, uint8_t blue, uint8_t green)
{
    uint32_t period = (uint32_t)PWM_LOAD + 1;
    uint32_t cmpR, cmpB, cmpG;


    if(red == 0){
			cmpR = 0;
		}
    else if(red == 100){
			cmpR = PWM_LOAD - 1;
		}
    else{
        cmpR = (period * red) / 100;
        if(cmpR > PWM_LOAD - 1){
					cmpR = PWM_LOAD - 1;
				}
    }

    if(blue == 0){
			cmpB = 0;
		}
    else if(blue == 100){
			cmpB = PWM_LOAD - 1;
		}
    else{
        cmpB = (period * blue) / 100;
        if(cmpB > PWM_LOAD - 1) cmpB = PWM_LOAD - 1;
    }

    if(green == 0){
      cmpG = 0;
		}
    else if(green == 100){
			cmpG = PWM_LOAD - 1;
		}
    else{
        cmpG = (period * green) / 100;
        if(cmpG > PWM_LOAD - 1) cmpG = PWM_LOAD - 1;
    }

    // PF1 = Red, PF2 = Blue, PF3 = Green
    PWM1_2_CMPB_R = cmpR;
    PWM1_3_CMPA_R = cmpB;
    PWM1_3_CMPB_R = cmpG;
}