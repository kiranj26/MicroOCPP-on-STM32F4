#include "main.h"
#include "lwip.h"
#include "microocpp.h"
#include <stdio.h>
#include <string.h>

/* Network Configuration */
#define OCPP_BACKEND_URL   "ws://192.168.1.100:8180/steve/websocket/CentralSystemService"
#define OCPP_CHARGE_BOX_ID "stm32-charger"

/* UART for Communication with ESP32 */
extern UART_HandleTypeDef huart2;

#define UART_RX_BUFFER_SIZE 256
char uartRxBuffer[UART_RX_BUFFER_SIZE];
volatile uint16_t uartRxIndex = 0;

/* Function Prototypes */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_LWIP_Init(void);
void logMessage(const char *message);
void handleBackendMessage(const char *message);
void sendToBackend(const char *message);

/* Callback Prototypes */
float getEnergyMeterReading(void);
bool isConnectorPlugged(void);
void setSmartChargingCurrent(float limit);

int main(void) {
    /* Initialize Hardware */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_LWIP_Init();

    /* Initialize OCPP */
    logMessage("[STM32] Initializing Micro OCPP...\r\n");
    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, "STM32 Charger", "My Company");
    setEnergyMeterInput(getEnergyMeterReading);
    setConnectorPluggedInput(isConnectorPlugged);
    setSmartChargingCurrentOutput(setSmartChargingCurrent);

    /* Enable UART Receive Interrupt */
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&uartRxBuffer[uartRxIndex], 1);

    while (1) {
        /* Process OCPP Logic */
        mocpp_loop();

        /* Allow or Disallow Charging */
        if (ocppPermitsCharge()) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Energize EV plug
        } else {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // De-energize EV plug
        }

        /* Perform Other Tasks */
        HAL_Delay(10);
    }
}

/* UART Receive Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) { // UART2 (ESP32)
        if (uartRxIndex < UART_RX_BUFFER_SIZE - 1) {
            if (uartRxBuffer[uartRxIndex] == '\n') { // Message Complete
                uartRxBuffer[uartRxIndex] = '\0';   // Null-terminate string
                handleBackendMessage(uartRxBuffer); // Process message
                uartRxIndex = 0;                    // Reset buffer index
            } else {
                uartRxIndex++; // Continue storing bytes
            }
        } else {
            uartRxIndex = 0; // Buffer overflow, reset
        }

        // Re-enable UART Receive Interrupt
        HAL_UART_Receive_IT(huart, (uint8_t *)&uartRxBuffer[uartRxIndex], 1);
    }
}

/* Process Backend Message Received via ESP32 */
void handleBackendMessage(const char *message) {
    logMessage("[STM32] Received message from backend:\r\n");
    logMessage(message);

    // Example: Handle specific OCPP operations
    if (strstr(message, "RemoteStartTransaction")) {
        beginTransaction("1234567890"); // Start a transaction with a dummy idTag
        logMessage("[STM32] RemoteStartTransaction processed.\r\n");
    } else if (strstr(message, "RemoteStopTransaction")) {
        endTransaction();
        logMessage("[STM32] RemoteStopTransaction processed.\r\n");
    }
}

/* Send Message to Backend via ESP32 */
void sendToBackend(const char *message) {
    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\n", 1, HAL_MAX_DELAY); // Add newline for message delimiter
}

/* Logging Helper */
void logMessage(const char *message) {
    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

/* Energy Meter Reading Callback */
float getEnergyMeterReading(void) {
    return 1234.5f; // Example static value. Replace with actual ADC reading or sensor data.
}

/* Connector Plugged Status Callback */
bool isConnectorPlugged(void) {
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET; // Example GPIO check
}

/* Smart Charging Limit Callback */
void setSmartChargingCurrent(float limit) {
    char buffer[50];
    sprintf(buffer, "[STM32] Smart Charging Limit: %.2f\n", limit);
    logMessage(buffer);
}

/* HAL Configuration Functions */
void SystemClock_Config(void) {
    // System clock setup (auto-generated via STM32CubeMX)
}

void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pin for EV plug control */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure GPIO pin for connector detection */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}

void MX_LWIP_Init(void) {
    // Ethernet stack setup (auto-generated via STM32CubeMX)
}
