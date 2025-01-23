#include <HardwareSerial.h>

// Use UART1 with custom GPIO pin for TX
// HardwareSerial Serial1(1);

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  // Initialize UART1 with GPIO4 as TX (RX is not used)
  Serial1.begin(115200, SERIAL_8N1, 4, -1); // TX = GPIO4, RX not used (-1)

  Serial.println("ESP32-C3 UART1 Initialized with TX = GPIO4");
}

void loop() {
    // Send a fixed-length message
    const char *message = "Hello STM32!\n";
    Serial1.write(message, 12);
    Serial.println(message);

    // Wait 5 seconds
    delay(5000);
}
