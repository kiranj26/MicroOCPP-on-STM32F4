# Dual UART Communication between ESP32-C3 DevKit v2 and STM32F030R8

---

## Project Overview

This project establishes **bidirectional UART communication** between an **ESP32-C3 DevKit v2** and an **STM32F030R8** microcontroller using **PlatformIO**. The ESP32-C3 is programmed using the **Arduino Framework**, while the STM32F030R8 utilizes the **STM32Cube HAL Framework**. This setup leverages the strengths of both microcontrollers, enabling robust data exchange and debugging capabilities.

---
## Demo Video

### Hardware
https://github.com/user-attachments/assets/be38b73d-9e1d-455a-bb69-684df0e44c3d


### Serial Terminal    
![alt text](<Screenshot 2025-01-26 at 7.51.03 PM.png>)
## Hardware Setup

### Voltage Compatibility

Both the **ESP32-C3** and **STM32F030R8** operate at **3.3V**, ensuring straightforward interfacing without the need for voltage level shifting.

### UART Pin Mapping

Establish UART communication by cross-connecting the TX and RX pins of both devices and ensuring a common ground.

| **ESP32-C3 DevKit v2** | **STM32F030R8**  | **Function**             |
|------------------------|-------------------|--------------------------|
| GPIO19 (UART1_TX)      | PA10 (USART1_RX)  | ESP32-C3 TX → STM32 RX   |
| GPIO18 (UART1_RX)      | PA9 (USART1_TX)   | ESP32-C3 RX ← STM32 TX   |
| GND                    | GND               | Common Ground            |

---

## Software Setup

### PlatformIO Project Structure

The project consists of two separate PlatformIO projects within the same workspace:

1. **ESP32C3_UART**: Contains firmware for the ESP32-C3 DevKit v2 using the Arduino framework.
2. **STM32F030_UART**: Contains firmware for the STM32F030R8 using the STM32Cube HAL framework.


### ESP32-C3 Firmware (Arduino Framework)

- **Purpose**: Initializes UART1, handles UART events, sends periodic heartbeat messages to STM32, and receives echoed messages and status updates.
- **Framework**: Arduino

### STM32F030R8 Firmware (STM32Cube HAL Framework)

- **Purpose**: Initializes USART1, handles UART interrupts, echoes received heartbeat messages back to ESP32-C3, sends confirmation messages, and periodic status updates.
- **Framework**: STM32Cube HAL

---

## Building and Uploading Firmware

### ESP32-C3 DevKit v2

1. **Connect the ESP32-C3** to your PC via USB.
2. **Open PlatformIO** in your IDE.
3. **Select the `ESP32C3_UART` Project**.
4. **Build the Project**:
   - Click on the **Build** button or run `PlatformIO: Build` from the command palette.
5. **Upload the Firmware**:
   - Click on the **Upload** button or run `PlatformIO: Upload`.
6. **Open the Serial Monitor**:
   - Click on the **Serial Monitor** button or run `PlatformIO: Monitor`.
   - Set the baud rate to **115200**.
   - **ESP32 Serial Terminal Output Placeholder**:
     ```
     Sent: Heartbeat
     Received: Heartbeat
     Received: Echo Sent
     Received: Status: OK
     Sent: Heartbeat
     Received: Heartbeat
     Received: Echo Sent
     Received: Status: OK
     ...
     ```

### STM32F030R8

1. **Connect the STM32F030R8** to your PC via ST-Link or USB (depending on your board).
2. **Open PlatformIO** in your IDE.
3. **Select the `STM32F030_UART` Project**.
4. **Build the Project**:
   - Click on the **Build** button or run `PlatformIO: Build` from the command palette.
5. **Upload the Firmware**:
   - Click on the **Upload** button or run `PlatformIO: Upload`.
6. **Monitor UART Communication**:
   - Use a secondary serial monitor or debug tools to verify UART communication.
   - Observe that the STM32 echoes back any data received from the ESP32-C3 and sends periodic status updates.

---

## Testing the Communication

### Initial Verification

1. **Power On Both Boards**:
   - Ensure both the **ESP32-C3** and **STM32F030R8** are connected to power sources.
2. **Check Serial Monitor (ESP32-C3)**:
   - Open the Serial Monitor for **ESP32C3_UART**.
   - You should see the initialization message and periodic heartbeat transmissions.
3. **Verify STM32 Echo**:
   - When the ESP32-C3 sends "Heartbeat", the STM32 should echo it back.
   - The ESP32-C3 Serial Monitor should display the received echoed message and confirmation.

### Bi-Directional Communication

1. **Send Commands from ESP32-C3**:
   - Modify the ESP32-C3 `loop()` function to send different messages or commands if desired.
2. **STM32 Responds Accordingly**:
   - Update the STM32 firmware to perform actions based on received commands (e.g., toggle another LED).
   - Ensure that the STM32 sends acknowledgments or responses back to the ESP32-C3.

---

## Troubleshooting

### No Communication

- **Check Wiring**:
  - Ensure that **TX** of ESP32-C3 is connected to **RX** of STM32 and vice versa.
  - Verify that both devices share a common ground.
  
- **Baud Rate Mismatch**:
  - Confirm that both UART configurations are set to **115200** baud rate.
  
- **Incorrect Pin Assignments**:
  - Double-check the GPIO pins used for UART on both devices.
  
- **Driver Issues**:
  - Ensure that necessary USB drivers for both boards are installed on your PC.

### Incorrect or Garbled Data

- **Parity and Stop Bits**:
  - Ensure both devices use **8 data bits**, **no parity**, and **1 stop bit** (**8N1**).
  
- **Flow Control**:
  - Confirm that hardware flow control is disabled on both UARTs.
  
- **Signal Integrity**:
  - Use shorter wires to minimize noise and interference.

### Firmware Issues

- **Infinite Loops**:
  - If LEDs toggle rapidly, it may indicate an error in the firmware. Check the `Error_Handler` implementations.
  
- **Interrupt Conflicts**:
  - Ensure that UART interrupts do not conflict with other peripheral interrupts.
  
- **Buffer Overflows**:
  - Monitor buffer sizes and indices to prevent data loss.

### Debugging Tools

- **Serial Monitor**:
  - Utilize the Serial Monitor on the ESP32-C3 for real-time debugging messages.
  
- **Logic Analyzer or Oscilloscope**:
  - Visualize UART signals to verify correct data transmission and reception.
  
- **LED Indicators**:
  - Use integrated LEDs to indicate data transmission, reception, and error states.

---

## Future Enhancements

### Bi-Directional Command Protocol

- **Define Command Sets**:
  - Establish a set of predefined commands for controlling actions between ESP32-C3 and STM32.
  
- **Implement Parsing Mechanism**:
  - On the STM32, parse incoming commands and execute corresponding actions (e.g., toggling LEDs, reading sensors).
  
- **Acknowledgments**:
  - Have the STM32 send acknowledgment messages back to the ESP32-C3 to confirm command execution.

### Data Integrity Checks

- **Checksum or CRC**:
  - Implement checksums or CRCs to ensure data integrity during transmission.
  
- **Error Handling**:
  - Develop mechanisms to handle corrupted or incomplete data packets gracefully.

### Utilize ESP32-C3's Wi-Fi Capabilities

- **Remote Control**:
  - Use Wi-Fi to send commands to the ESP32-C3, which then relays them to the STM32.
  
- **Data Logging**:
  - Transmit sensor data from STM32 to ESP32-C3 for logging or cloud integration.
  
- **Over-The-Air (OTA) Updates**:
  - Implement OTA firmware updates for the STM32 via the ESP32-C3's Wi-Fi connection.

### Power Management

- **Sleep Modes**:
  - Implement sleep modes on both microcontrollers to conserve power when idle.
  
- **Efficient Power Supply**:
  - Optimize power distribution, especially if the setup is battery-powered.

### Security Enhancements

- **Data Encryption**:
  - Encrypt UART communication to protect sensitive data.
  
- **Authentication Mechanisms**:
  - Implement authentication to ensure that only authorized devices can communicate.

---

## References

- **PlatformIO Documentation**:
  - [PlatformIO Official Documentation](https://docs.platformio.org/)
  
- **ESP32-C3 Documentation**:
  - [Espressif ESP32-C3 Series Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
  - [ESP32 Arduino Core GitHub](https://github.com/espressif/arduino-esp32)
  
- **STM32F030R8 Documentation**:
  - [STM32F0 Series Reference Manual](https://www.st.com/resource/en/reference_manual/rm0360-stm32f0-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
  - [STM32Cube HAL Documentation](https://www.st.com/en/embedded-software/stm32cubef0.html)
  
- **UART Communication Tutorials**:
  - [UART Communication Between Two Microcontrollers](https://www.electronicshub.org/uart-communication/)
  - [ESP32 UART Communication](https://randomnerdtutorials.com/esp32-serial-communication-arduino-ide/)
  
- **PlatformIO STM32 Boards**:
  - [PlatformIO STM32 Board List](https://platformio.org/boards?query=stm32)

---

**Author:** Kiran Jojare

---
