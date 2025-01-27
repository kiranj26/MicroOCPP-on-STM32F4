#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdbool.h>

#define RXBUF_SIZE 128
#define TXBUF_SIZE 256 // Increased buffer size
#define MESSAGE_BUFFER_SIZE 64

UART_HandleTypeDef huart1;

volatile uint8_t rxBuf[RXBUF_SIZE];
volatile uint16_t rxHead = 0, rxTail = 0;
volatile uint8_t txBuf[TXBUF_SIZE];
volatile uint16_t txHead = 0, txTail = 0;
volatile bool txBusy = false;
uint8_t rxByte;

// Message buffering for complete messages
volatile uint8_t messageBuffer[MESSAGE_BUFFER_SIZE];
volatile uint16_t messageIndex = 0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void UART_Transmit_Data(const char *str);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    HAL_UART_Receive_IT(&huart1, &rxByte, 1);

    // LED blink at startup
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(200);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    // Send startup message
    UART_Transmit_Data("STM32 Ready\r\n");

    while (1) {
        // Process received bytes into complete messages
        while (rxHead != rxTail) {
            uint8_t c = rxBuf[rxTail];
            rxTail = (rxTail + 1) % RXBUF_SIZE;

            if (c == '\r') {
                // Ignore carriage return
                continue;
            }

            if (c == '\n') {
                // End of message
                if (messageIndex > 0) {
                    messageBuffer[messageIndex] = '\0'; // Null-terminate

                    // Check if the message is a heartbeat
                    if (strcmp((char*)messageBuffer, "Heartbeat") == 0) {
                        // Echo back the heartbeat without prefix
                        UART_Transmit_Data("Heartbeat\r\n");
                    }

                    // Optionally, send a confirmation message
                    UART_Transmit_Data("Echo Sent\r\n"); // Echo Sent without prefix

                    // Reset message index
                    messageIndex = 0;
                }
            } else {
                if (messageIndex < (MESSAGE_BUFFER_SIZE - 1)) {
                    // Add to buffer
                    messageBuffer[messageIndex++] = c;
                } else {
                    // Buffer overflow handling
                    UART_Transmit_Data("Error: Msg too long\r\n");
                    messageIndex = 0;
                }
            }
        }

        // Send status every 3 seconds to stagger with heartbeat
        static uint32_t lastSend = 0;
        if (HAL_GetTick() - lastSend > 3000) { // 3000ms
            UART_Transmit_Data("Status: OK\r\n"); // Status message without prefix
            lastSend = HAL_GetTick();
        }

        // Toggle LED every 500ms
        static uint32_t lastToggle = 0;
        if (HAL_GetTick() - lastToggle > 500) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            lastToggle = HAL_GetTick();
        }
    }
}

// --- UART Interrupt Callbacks ---
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        rxBuf[rxHead] = rxByte;
        uint16_t nextHead = (rxHead + 1) % RXBUF_SIZE;
        if (nextHead != rxTail) { // Prevent buffer overflow
            rxHead = nextHead;
        } else {
            // Buffer overflow handling (optional)
            // Implement logging or additional handling here if needed
        }
        HAL_UART_Receive_IT(&huart1, &rxByte, 1);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        txTail = (txTail + 1) % TXBUF_SIZE; // Move to next byte
        if (txTail != txHead) {
            // Cast away volatile to match function signature
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)&txBuf[txTail], 1);
        } else {
            txBusy = false; // TX buffer empty
        }
    }
}

// --- Helper Functions ---
static void UART_Transmit_Data(const char *str) {
    for (uint16_t i = 0; i < strlen(str); i++) {
        uint16_t nextHead = (txHead + 1) % TXBUF_SIZE;
        if (nextHead == txTail) {
            // Buffer is full, handle overflow
            // For simplicity, skip sending this byte or implement a counter
            break;
        }
        txBuf[txHead] = str[i];
        txHead = nextHead;
    }
    if (!txBusy) {
        txBusy = true;
        HAL_UART_Transmit_IT(&huart1, (uint8_t*)&txBuf[txTail], 1); // Cast away volatile
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

static void MX_USART1_UART_Init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart1);
}

void SysTick_Handler(void) {
    HAL_IncTick();
}
