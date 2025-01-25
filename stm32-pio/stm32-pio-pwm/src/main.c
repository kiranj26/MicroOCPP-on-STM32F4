#include "stm32f0xx_hal.h"

// Pin Definitions
#define LED_PIN           GPIO_PIN_8
#define LED_GPIO_PORT     GPIOA

// Function Prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);

// Timer Handle
TIM_HandleTypeDef htim1;

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM1_Init();

    // Start PWM on TIM1 Channel 1
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    while (1) {
        // Gradually increase and decrease the duty cycle
        for (int duty = 0; duty <= 100; duty++) {
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty); // Set PWM duty cycle
            HAL_Delay(20); // Smooth fade
        }
        for (int duty = 100; duty >= 0; duty--) {
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty); // Set PWM duty cycle
            HAL_Delay(20); // Smooth fade
        }
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure HSI oscillator and PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12; // 48MHz
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIOA Clock
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA8 for TIM1 Channel 1 (PWM output)
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM1; // Alternate function for TIM1_CH1
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

static void MX_TIM1_Init(void) {
    TIM_OC_InitTypeDef sConfigOC = {0};

    // Enable TIM1 Clock
    __HAL_RCC_TIM1_CLK_ENABLE();

    // Configure TIM1
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 479;                // Prescaler for 100 kHz timer clock
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 100;                   // 100 steps for PWM resolution
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&htim1);

    // Configure TIM1 Channel 1 for PWM
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;                       // Start with 0% duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
}
