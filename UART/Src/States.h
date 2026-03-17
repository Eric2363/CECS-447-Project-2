#include "stdint.h"

typedef enum States{
	MainMenu,
	Mode1,
	Mode2_Enter,
  Mode2_MCU1_SelectColor,
  Mode2_MCU1_WaitReply,

} State;



extern volatile State CurrentState;
