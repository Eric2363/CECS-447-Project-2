#include <stdint.h>
#include <stdbool.h>

// --------------------
// ASCII Control Codes
// --------------------
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F
#define NULL 0

#define UART4_NVIC_EN   0x10000000
#define RB_SIZE         64
// --------------------
// Terminal Colors
// --------------------

// Basic Colors
#define COLOR_BLACK        "\033[30m"
#define COLOR_RED          "\033[31m"
#define COLOR_GREEN        "\033[32m"
#define COLOR_YELLOW       "\033[33m"
#define COLOR_BLUE         "\033[34m"
#define COLOR_MAGENTA      "\033[35m"
#define COLOR_CYAN         "\033[36m"
#define COLOR_WHITE        "\033[37m"

// Bright Colors
#define COLOR_BRIGHT_BLACK   "\033[90m"
#define COLOR_BRIGHT_RED     "\033[91m"
#define COLOR_BRIGHT_GREEN   "\033[92m"
#define COLOR_BRIGHT_YELLOW  "\033[93m"
#define COLOR_BRIGHT_BLUE    "\033[94m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_BRIGHT_CYAN    "\033[96m"
#define COLOR_BRIGHT_WHITE   "\033[97m"

// --------------------
// UART4 Functions
// --------------------
void UART4_Init(void);

uint8_t UART4_InChar(void);
void UART4_OutChar(uint8_t data);

void UART4_OutString(uint8_t *pt);
void UART4_InString(uint8_t *bufPt, uint16_t max);

uint32_t UART4_InUDec(void);
void UART4_OutUDec(uint32_t n);

void UART4_OutCRLF(void);

bool UART4_Available(void);