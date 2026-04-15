#ifndef APP_H
#define APP_H

#include "Std_Types.h"

// LED definitions
#define PROGRESS_LED0  GPIO_B, 0
#define PROGRESS_LED1  GPIO_B, 1
#define PROGRESS_LED2  GPIO_B, 2
#define PROGRESS_LED3  GPIO_B, 3
#define SUCCESS_LED    GPIO_B, 4

// Correct password
#define CORRECT_PASSWORD "1234"
#define PASSWORD_LENGTH 4

// Function prototypes
void App_Init(void);
void App_Run(void);

#endif // APP_H