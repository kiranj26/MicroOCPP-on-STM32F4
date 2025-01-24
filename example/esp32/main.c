#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

/* UART Configuration */
#define UART_RX_PIN 16
#define UART_TX_PIN 17
HardwareSerial uart(2); // Use Serial2 for communication with STM32

/* WebSocket Client Configuration */
WebSocketsClient webSocket;
bool isWebSocketConnected = false;

/* Function Prototypes */
void handleSTM32Message(const String &message);
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);

void setup() {
    Serial.begin(115200); // Debug output
    uart.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

    // Wi-Fi Setup
    Serial.println("[ESP32] Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n[ESP32] Wi-Fi connected.");

    // WebSocket Setup
    webSocket.begin("192.168.1.100", 8180, "/steve/websocket/CentralSystemService"); // Replace with your OCPP backend URL
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    // Handle WebSocket Events
    webSocket.loop();

    // Forward STM32 messages to backend
    if (uart.available()) {
        String message = uart.readStringUntil('\n');
        Serial.printf("[ESP32] Received from STM32: %s\n", message.c_str());
        if (isWebSocketConnected) {
            webSocket.sendTXT(message);
        } else {
            Serial.println("[ESP32] WebSocket not connected.");
        }
    }
}

/* WebSocket Event Handler */
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            isWebSocketConnected = true;
            Serial.println("[ESP32] WebSocket connected.");
            break;

        case WStype_TEXT:
            Serial.printf("[ESP32] Received from backend: %s\n", payload);
            uart.print((char *)payload); // Forward backend message to STM32
            uart.print("\n");
            break;

        case WStype_DISCONNECTED:
            isWebSocketConnected = false;
            Serial.println("[ESP32] WebSocket disconnected.");
            break;

        default:
            break;
    }
}
