#include <Arduino.h>
#include <HardwareSerial.h>

#define UART_BAUDRATE 115200
#define MAX_MSG_LEN 64  // Reduced buffer size for simplicity

HardwareSerial SerialSTM32(1); // RX=19, TX=18

char receivedMsg[MAX_MSG_LEN];
uint16_t msgIndex = 0;

void setup() {
    Serial.begin(115200);
    SerialSTM32.begin(UART_BAUDRATE, SERIAL_8N1, 19, 18);
    Serial.println("UART Communication Started");
}

void loop() {
    // Receive data and assemble into messages
    while (SerialSTM32.available()) {
        char c = SerialSTM32.read();

        if (c == '\r') {
            // Ignore carriage return
            continue;
        }

        if (c == '\n') {
            // End of message
            if (msgIndex > 0) {
                receivedMsg[msgIndex] = '\0'; // Null-terminate the string
                Serial.print("Received: ");
                Serial.println(receivedMsg);
                msgIndex = 0;
            }
        } else {
            if (msgIndex < (MAX_MSG_LEN - 1)) {
                // Add to buffer
                receivedMsg[msgIndex++] = c;
            } else {
                // Buffer overflow handling
                Serial.println("Error: Received message too long");
                msgIndex = 0;
            }
        }
    }

    // Send heartbeat every 2 seconds
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 2000) {
        String heartbeatMsg = "Heartbeat\r\n"; // Heartbeat without prefix
        SerialSTM32.print(heartbeatMsg); // Send without prefix
        Serial.println("Sent: Heartbeat"); // Local display without directional prefix
        lastSend = millis();
    }
}
