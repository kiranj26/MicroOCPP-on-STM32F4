#include <Arduino.h>
#include "driver/uart.h"
#include "esp_log.h"

// Define UART pins and configuration
#define TXD_PIN (GPIO_NUM_4) // GPIO4 as TX
#define RXD_PIN (GPIO_NUM_5) // GPIO5 as RX
#define UART_PORT_NUM UART_NUM_1
#define BUF_SIZE (1024)

void setup() {
    // Initialize Serial Monitor for debugging
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for Serial to connect
    }
    Serial.println("ESP32-C3 UART Communication Test");

    // UART configuration
    uart_config_t uart_config = {
        .baud_rate = 115200,            // Baud rate
        .data_bits = UART_DATA_8_BITS, // Data bits: 8
        .parity = UART_PARITY_DISABLE, // No parity
        .stop_bits = UART_STOP_BITS_1, // Stop bits: 1
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // No flow control
        .rx_flow_ctrl_thresh = 122,    // RX flow control threshold (not used here)
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));

    // Set UART pins (TX, RX, RTS, CTS)
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Install UART driver and create buffer
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0));

    Serial.println("UART initialized successfully");
}

void loop() {
    char rx_buffer[128]; // Buffer for incoming data
    const char *tx_message = "Hello from ESP32-C3\r\n";

    // Transmit data to STM32
    int tx_bytes = uart_write_bytes(UART_PORT_NUM, tx_message, strlen(tx_message));
    Serial.printf("Sent to STM32: %s (%d bytes)\n", tx_message, tx_bytes);

    // Check for incoming data
    int rx_bytes = uart_read_bytes(UART_PORT_NUM, (uint8_t *)rx_buffer, sizeof(rx_buffer) - 1, 100 / portTICK_PERIOD_MS);
    if (rx_bytes > 0) {
        rx_buffer[rx_bytes] = '\0'; // Null-terminate received string
        Serial.printf("Received from STM32: %s\n", rx_buffer);
    } else {
        Serial.println("No data received from STM32");
    }

    delay(2000); // Delay for 2 seconds
}
