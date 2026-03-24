#include "stdint.h"

//all menu option states
typedef enum States{
	MainMenu,
	Mode1,
	Mode2_Enter,
  Mode2_MCU1_SelectColor,
  Mode2_MCU1_WaitReply,
	Mode3_MCU1_Enter,
	Mode3_MCU1_Talk,
	Mode3_MCU1_Listen

} State;



extern volatile State CurrentState;
