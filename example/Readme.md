# STM32 and ESP32 Communication Example  
> **Progress: Work in Progress**  
This example demonstrates how to set up communication between an STM32-based EVSE and an ESP32 acting as a Wi-Fi gateway for OCPP (Open Charge Point Protocol). The STM32 handles core OCPP logic, while the ESP32 forwards messages to and from the backend.

---

## **Folder Structure**

```
examples/
├── stm32/
│   └── main.c   # STM32 code for OCPP logic
├── esp32/
│   └── main.c   # ESP32 code for Wi-Fi and WebSocket communication
└── readme.md    # Instructions on how to set up and run the example
```

---

## **How to Use**

### **Prerequisites**
1. **Hardware Requirements**:
   - STM32 development board (e.g., STM32 Nucleo F030R8).
   - ESP32 development board (e.g., ESP32 DevKit V1).
   - USB cables for flashing and debugging.
   - UART connection between STM32 and ESP32 (e.g., RX/TX wires).

2. **Software Requirements**:
   - STM32CubeIDE for STM32 development.
   - PlatformIO or Arduino IDE for ESP32 development.
   - An OCPP backend (e.g., [SteVe](https://github.com/RWTH-i5-IDSG/steve)).
   - UART terminal software (e.g., TeraTerm, PuTTY, or STM32CubeMonitor).

---

### **Steps to Set Up**

#### **1. Configure and Flash ESP32**
1. Open the `examples/esp32/main.c` file in your preferred IDE (PlatformIO or Arduino IDE).
2. Update the following variables in the code:
   - `WIFI_SSID`: Your Wi-Fi network SSID.
   - `WIFI_PASSWORD`: Your Wi-Fi network password.
   - `webSocket.begin`: Replace with your OCPP backend WebSocket URL.
3. Flash the ESP32 with the updated code.
4. Connect the ESP32 to the STM32 via UART:
   - ESP32 `TX` → STM32 `RX`.
   - ESP32 `RX` → STM32 `TX`.

#### **2. Configure and Flash STM32**
1. Open the `examples/stm32/main.c` file in STM32CubeIDE.
2. Ensure the correct UART pins are configured in the code (e.g., `USART2` for STM32 Nucleo boards).
3. Connect the STM32 to your computer and flash the board with the code.

#### **3. Backend Configuration**
1. Deploy or access an OCPP backend (e.g., SteVe).
2. Configure the backend to allow WebSocket communication for your ESP32’s WebSocket URL.
3. Verify the backend logs to ensure it detects connections.

#### **4. Run the Example**
1. Power on both the STM32 and ESP32 devices.
2. Use a UART terminal to monitor logs from:
   - STM32 (e.g., BootNotification, transactions).
   - ESP32 (e.g., WebSocket connection and forwarded messages).
3. Perform the following tests:
   - Initiate a transaction (e.g., `RemoteStartTransaction` from the backend).
   - Monitor the messages exchanged between STM32, ESP32, and the backend.

---

### **Expected Output**
1. **Backend to STM32**:
   - The backend sends a `RemoteStartTransaction` command via WebSocket.
   - ESP32 forwards the command to STM32 via UART.
   - STM32 processes the command and starts a transaction.

2. **STM32 to Backend**:
   - STM32 sends periodic `MeterValues` and status updates via UART to ESP32.
   - ESP32 forwards these messages to the backend via WebSocket.

---

### **Known Limitations**
- The example assumes a single connector setup.
- The UART configuration is hardcoded; adapt for different pins or speeds as needed.
- Advanced OCPP features (e.g., smart charging profiles) are not implemented in this example.

---

### **Next Steps**
1. Extend functionality to support multiple connectors.
2. Add TLS for secure WebSocket communication.
3. Test with different OCPP backend platforms.
