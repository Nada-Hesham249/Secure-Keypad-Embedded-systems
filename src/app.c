#include "app.h"
#include "Rcc.h"
#include "Gpio.h"
#include "Keypad.h"
#include "SevenSeg.h"
#include "Utils.h"

#define NO_KEY_PRESSED 0xFF
#define FAILURE_LIMIT 3
#define FAILURE_LED_ON_TIME_MS 2000

// Static variables
static uint8 input_buffer[PASSWORD_LENGTH];
static uint8 input_index = 0;
static uint8 failure_count = 0;

static void InitClocks(void) {
    Rcc_Init();
    Rcc_Enable(RCC_GPIOB);
    Rcc_Enable(RCC_GPIOC);
    Rcc_Enable(RCC_GPIOD);
}

static void InitLeds(void) {
    Gpio_Init(PROGRESS_LED0, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(PROGRESS_LED1, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(PROGRESS_LED2, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(PROGRESS_LED3, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(SUCCESS_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(Alarm_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);
}

static void TurnOffAllLeds(void) {
    Gpio_WritePin(PROGRESS_LED0, LOW);
    Gpio_WritePin(PROGRESS_LED1, LOW);
    Gpio_WritePin(PROGRESS_LED2, LOW);
    Gpio_WritePin(PROGRESS_LED3, LOW);
    Gpio_WritePin(SUCCESS_LED, LOW);
    Gpio_WritePin(Alarm_LED, LOW);
}

static void TurnOnProgressLed(uint8 position) {
    switch (position) {
        case 0:
            Gpio_WritePin(PROGRESS_LED0, HIGH);
            break;
        case 1:
            Gpio_WritePin(PROGRESS_LED1, HIGH);
            break;
        case 2:
            Gpio_WritePin(PROGRESS_LED2, HIGH);
            break;
        case 3:
            Gpio_WritePin(PROGRESS_LED3, HIGH);
            break;
        default:
            break;
    }
}

static void ResetInputState(void) {
    input_index = 0;
}

static void ClearInputBuffer(void) {
    for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
        input_buffer[i] = 0;
    }
}

static void ResetForNewAttempt(void) {
    Gpio_WritePin(PROGRESS_LED0, LOW);
    Gpio_WritePin(PROGRESS_LED1, LOW);
    Gpio_WritePin(PROGRESS_LED2, LOW);
    Gpio_WritePin(PROGRESS_LED3, LOW);
    Gpio_WritePin(SUCCESS_LED, LOW);
    Gpio_WritePin(Alarm_LED, LOW);
    ResetInputState();
}

static void HandleFailureLimitReached(void) {
    SevenSeg_Display(FAILURE_LIMIT);
    Gpio_WritePin(Alarm_LED, HIGH);
    delay_ms(FAILURE_LED_ON_TIME_MS);

    Gpio_WritePin(Alarm_LED, LOW);
    failure_count = 0;
    SevenSeg_Display(failure_count);
    ClearInputBuffer();
    ResetInputState();
    TurnOffAllLeds();
}

static void WaitForKeyRelease(void) {
    while (Keypad_Scan() != NO_KEY_PRESSED) {
        delay_ms(10);
    }
}

static uint8 IsPasswordComplete(void) {
    return (input_index == PASSWORD_LENGTH);
}

static uint8 IsPasswordCorrect(void) {
    for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
        if (input_buffer[i] != CORRECT_PASSWORD[i]) {
            return 0;
        }
    }

    return 1;
}

static void HandleCorrectPassword(void) {
    Gpio_WritePin(SUCCESS_LED, HIGH);

    while (1) {
        // Keep system in success state.
    }
}

static void HandleWrongPassword(void) {
    failure_count++;
    SevenSeg_Display(failure_count);

    if (failure_count >= FAILURE_LIMIT) {
        HandleFailureLimitReached();
        return;
    }

    ResetForNewAttempt();
}

static uint8 IsValidKeyInput(uint8 key) {
    return (key != NO_KEY_PRESSED) && (input_index < PASSWORD_LENGTH);
}

static void StoreKeyAndUpdateProgress(uint8 key) {
    input_buffer[input_index] = key;
    input_index++;
    TurnOnProgressLed(input_index - 1);
}

static void ProcessKeypadInput(void) {
    uint8 key = Keypad_Scan();

    if (!IsValidKeyInput(key)) {
        return;
    }

    StoreKeyAndUpdateProgress(key);
    WaitForKeyRelease();
}

static void ProcessPasswordCheck(void) {
    if (!IsPasswordComplete()) {
        return;
    }

    if (IsPasswordCorrect()) {
        HandleCorrectPassword();
    } else {
        HandleWrongPassword();
    }
}

void App_Init(void) {
    InitClocks();
    InitLeds();
    TurnOffAllLeds();

    Keypad_Init();
    SevenSeg_Init();
    SevenSeg_Display(failure_count);

    ClearInputBuffer();
    ResetInputState();
}

void App_Run(void) {
    while (1) {
        ProcessKeypadInput();
        ProcessPasswordCheck();
        delay_ms(100);
    }
}