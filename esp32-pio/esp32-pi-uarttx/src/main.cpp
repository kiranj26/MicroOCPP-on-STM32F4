#include <Arduino.h>

// UART Configuration
#define UART_NUM UART_NUM_1
#define UART_TX_PIN 18  // GPIO18 assigned to UART1_TX
#define UART_RX_PIN 19  // GPIO19 assigned to UART1_RX
#define UART_BAUD_RATE 115200
#define UART_BUFFER_SIZE 1024

// Buffer to store incoming data
uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile bool data_received = false;

// UART Event Task Handle
TaskHandle_t uart_task_handle;

// Function to handle UART events
void uart_event_task(void *pvParameters) {
    uart_event_t event;
    while (1) {
        // Waiting for UART event.
        if (xQueueReceive(uart_get_ringbuf_handle(UART_NUM), (void *)&event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                // Read the data from UART
                int len = uart_read_bytes(UART_NUM, uart_rx_buffer, sizeof(uart_rx_buffer) - 1, 100 / portTICK_PERIOD_MS);
                if (len > 0) {
                    // Null-terminate the received data
                    uart_rx_buffer[len] = '\0';
                    
                    // Print received data to Serial Monitor for debugging
                    Serial.print("Received from UART1: ");
                    Serial.println((char*)uart_rx_buffer);
                    
                    // Optionally, process the data or send a response
                }
            }
            // Handle other event types if necessary
        }
    }
    vTaskDelete(NULL);
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    while (!Serial); // Wait for Serial Monitor to open

    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    // Configure UART1 with specified parameters
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    // Install UART driver with buffer size
    uart_driver_install(UART_NUM, UART_BUFFER_SIZE * 2, UART_BUFFER_SIZE * 2, 10, NULL, 0);
    
    // Create UART event task
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, &uart_task_handle);
    
    // Send initial message to UART1
    const char* init_msg = "ESP32-C3 UART Initialized\r\n";
    uart_write_bytes(UART_NUM, init_msg, strlen(init_msg));
}

void loop() {
    // Example: Send data to UART1 every 5 seconds
    static unsigned long last_send = 0;
    if (millis() - last_send > 5000) { // 5 seconds interval
        const char* msg = "Hello from ESP32-C3\r\n";
        uart_write_bytes(UART_NUM, msg, strlen(msg));
        Serial.println("Sent to UART1: Hello from ESP32-C3");
        last_send = millis();
    }
    
    // Additional logic can be added here
}
