#include "stdint.h"

typedef enum States{
	MainMenu,
	Mode1,
	Mode2,
	Mode2_SelectColor,
	Mode2_WaitForMCU2
} State;



extern volatile State CurrentState;
