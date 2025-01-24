#include "main.h"
#include "microocpp.h"
#include <stdio.h>
#include <string.h>

#define OCPP_CHARGE_BOX_ID "stm32-charger"

// UART Configuration
extern UART_HandleTypeDef huart2;

// Buffer for UART Communication
#define UART_BUFFER_SIZE 256
char uartBuffer[UART_BUFFER_SIZE];
uint16_t uartIndex = 0;

// Function Prototypes
void handleBackendMessage(const char *message);
void sendToBackend(const char *message);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    mocpp_initialize(
        nullptr,                // No direct backend connection
        OCPP_CHARGE_BOX_ID,     // Charge box ID
        "STM32 Charger",        // Model
        "My Company",           // Vendor
        MicroOcpp::FilesystemOpt::Use_InMemory // Use in-memory configuration
    );

    HAL_UART_Receive_IT(&huart2, (uint8_t *)&uartBuffer[uartIndex], 1);

    while (1) {
        mocpp_loop();
        HAL_Delay(10); // Main loop delay
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) { // Check UART instance
        if (uartIndex < UART_BUFFER_SIZE - 1) {
            if (uartBuffer[uartIndex] == '\n') { // End of message
                uartBuffer[uartIndex] = '\0';
                handleBackendMessage(uartBuffer); // Process backend message
                uartIndex = 0; // Reset buffer
            } else {
                uartIndex++;
            }
        } else {
            uartIndex = 0; // Reset on overflow
        }

        HAL_UART_Receive_IT(huart, (uint8_t *)&uartBuffer[uartIndex], 1);
    }
}

void handleBackendMessage(const char *message) {
    if (strstr(message, "RemoteStartTransaction")) {
        beginTransaction("1234567890"); // Start transaction with dummy idTag
    } else if (strstr(message, "RemoteStopTransaction")) {
        endTransaction(); // End current transaction
    }
}

void sendToBackend(const char *message) {
    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\n", 1, HAL_MAX_DELAY); // Add newline
}
