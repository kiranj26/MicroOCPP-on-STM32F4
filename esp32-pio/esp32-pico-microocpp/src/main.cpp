#include <Arduino.h>

// Define UART1 pins for loopback
#define TXD1 4  // GPIO4 as TX
#define RXD1 5  // GPIO5 as RX

void setup() {
    // Initialize the default UART (UART0) for debugging
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for Serial port to connect (required for native USB)
    }
    Serial.println("Starting ESP32-C3 UART Loopback Test...");

    // Initialize predefined UART1 for loopback
    Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
    Serial.println("UART1 initialized: TX(GPIO4), RX(GPIO5)");
}

void loop() {
    static uint32_t counter = 0;
    String messageToSend = "Hello from ESP32-C3! Count: " + String(counter) + "\n";

    // Send data via UART1 TX
    Serial1.print(messageToSend);
    Serial.print("[DEBUG] Sent: ");
    Serial.print(messageToSend);

    // Wait to receive data from UART1 RX
    while (Serial1.available()) {
        char receivedChar = Serial1.read(); // Read one character
        Serial.print(receivedChar);        // Print received data to debug UART
    }

    Serial.println(); // Add a newline for readability in debug prints
    counter++;
    delay(1000); // Send data every second
}
