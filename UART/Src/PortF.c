// PortF.c  -- PortF Init + PWM for PF1 (R), PF2 (B), PF3 (G)
// 0 => 0% off, 100 => 100% full

#include "tm4c123gh6pm.h"
#include "PortF.h"
#include <stdint.h>




// Initialize PortF GPIO (switches) and PWM for LEDs (PF1-PF3)
void PortF_Init(void){
    // Enable PWM1 and Port F clocks
    SYSCTL_RCGCPWM_R |= 0x02;   // PWM1
    SYSCTL_RCGCGPIO_R |= PORTF; // PortF
    while((SYSCTL_PRGPIO_R & PORTF) == 0){} // wait for port ready

    // Unlock Port F and allow changes to PF4-0
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x1F;

    // Configure PF1-3 as PWM alternate function (do AF before configuring PWM)
    GPIO_PORTF_DIR_R   |= LEDS;     // PF1-3 outputs (still OK for PWM)
    GPIO_PORTF_DEN_R   |= LEDS;
    GPIO_PORTF_AMSEL_R &= ~LEDS;
    GPIO_PORTF_AFSEL_R |= LEDS;     // enable alternate function for PF1-3
    // Route PF1/2/3 to M1PWM5/6/7 (PCTL = 0x5 for each)
    GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & ~0x0000FFF0) | 0x00005550;

    // Configure switches PF4 & PF0 as inputs (no AF)
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

    // NVIC setup for GPIOF (priority/enable) - keep your existing macro LEVEL2
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | LEVEL2;
    NVIC_EN0_R |= 0x40000000; // Enable Port F IRQ

    // --- PWM generator setup (PB4-style: LOW at LOAD, HIGH at CMP) ---
    // Use PWM1 generators 2 and 3:
    // PF1 -> M1PWM5 -> PWM1 generator 2 output B (CMPB)
    // PF2 -> M1PWM6 -> PWM1 generator 3 output A (CMPA)
    // PF3 -> M1PWM7 -> PWM1 generator 3 output B (CMPB)

    // Gen2 (PWM5, PF1)
    PWM1_2_CTL_R  = 0;                 // disable Gen2 during config
    PWM1_2_GENA_R = 0x00000000;        // not used for Gen2 (we use GENB)
    PWM1_2_GENB_R = 0x00000C08;        // PB4-style: LOW at LOAD, HIGH at CMPB-down
    PWM1_2_LOAD_R = PWM_LOAD;
    PWM1_2_CMPB_R = 0;                 // 0% duty -> CMP=0 (off in this setup)
    PWM1_2_CTL_R |= 1;                 // enable Gen2

    // Gen3 (PWM6/PWM7, PF2/PF3)
    PWM1_3_CTL_R  = 0;                 // disable Gen3 during config
    PWM1_3_GENA_R = 0x000000C8;        // PB4-style: LOW at LOAD, HIGH at CMPA-down
    PWM1_3_GENB_R = 0x00000C08;        // PB4-style: LOW at LOAD, HIGH at CMPB-down
    PWM1_3_LOAD_R = PWM_LOAD;
    PWM1_3_CMPA_R = 0;                 // 0% duty
    PWM1_3_CMPB_R = 0;                 // 0% duty
    PWM1_3_CTL_R |= 1;                 // enable Gen3

    // Enable outputs: PWM5, PWM6, PWM7
    PWM1_ENABLE_R |= (1<<5) | (1<<6) | (1<<7);
}

// Set single LED brightness (one LED at a time).
// led: 1=Red(PF1), 2=Blue(PF2), 3=Green(PF3)
// duty: 0..100 where 0==OFF, 100==FULL
void PortF_SetDuty(uint8_t led, uint8_t duty)
{
    if(duty > 100) duty = 100;

    uint32_t period = (uint32_t)PWM_LOAD + 1u;
    uint32_t cmp;

    if(duty == 0) {
        cmp = 0;                    // OFF (matches your working behavior)
    } else if(duty == 100) {
        cmp = PWM_LOAD - 1u;        // MAX brightness (avoid CMP == LOAD which turns off)
    } else {
        cmp = (period * duty) / 100u;
        if(cmp > PWM_LOAD - 1u) cmp = PWM_LOAD - 1u; // keep away from LOAD
    }

    // Force others OFF (CMP=0 is OFF in your current working setup)
    PWM1_2_CMPB_R = 0;
    PWM1_3_CMPA_R = 0;
    PWM1_3_CMPB_R = 0;

    if (led == 1)       PWM1_2_CMPB_R = cmp;
    else if (led == 2)  PWM1_3_CMPA_R = cmp;
    else if (led == 3)  PWM1_3_CMPB_R = cmp;
}