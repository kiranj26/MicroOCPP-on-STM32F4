#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

// Wi-Fi Credentials
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// WebSocket Backend Configuration
#define OCPP_BACKEND_URL "wss://your-backend-url/steve/websocket/CentralSystemService"
#define OCPP_AUTH_KEY "SecureAuthKey" // Basic Authentication key

// CA Certificate
const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
... (your CA certificate here) ...
-----END CERTIFICATE-----
)EOF";

// UART Configuration
#define UART_RX_PIN 16
#define UART_TX_PIN 17
HardwareSerial uart(2);

// WebSocket Client
WebSocketsClient webSocket;
bool isWebSocketConnected = false;

// WebSocket Event Handler
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            isWebSocketConnected = true;
            Serial.println("[ESP32] WebSocket connected.");
            break;
        case WStype_TEXT:
            Serial.printf("[ESP32] Received from backend: %s\n", payload);
            uart.print((char *)payload); // Forward to STM32
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

    // Time Synchronization for TLS
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    while (time(nullptr) < 8 * 3600 * 2) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n[ESP32] Time synchronized for certificate validation.");

    // WebSocket Setup
    webSocket.beginSSL("your-backend-url", 443, "/steve/websocket/CentralSystemService");
    webSocket.setAuthorization("Basic", OCPP_AUTH_KEY); // Basic Authentication
    webSocket.setCACert(ca_cert);
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();

    // Forward STM32 messages to backend
    if (uart.available()) {
        String message = uart.readStringUntil('\n');
        Serial.printf("[ESP32] Forwarding to backend: %s\n", message.c_str());
        if (isWebSocketConnected) {
            webSocket.sendTXT(message);
        } else {
            Serial.println("[ESP32] WebSocket not connected.");
        }
    }
}
