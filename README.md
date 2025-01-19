# **STM32F4 MicroOCPP Implementation**

## **Overview**

Integration of MicroOCPP on STM32F4 Nucleo-F446RE using ESP32-C3 as a Wi-Fi bridge. Implements UART communication, secure WebSocket connections, and Open Charge Point Protocol (OCPP) for EV chargers.

This repository demonstrates the implementation of **MicroOCPP** on an **STM32F4 microcontroller**, enabling **OCPP 1.6** compliance for **Electric Vehicle Supply Equipment (EVSE)**. It integrates key components like secure WebSocket communication, time synchronization, and FreeRTOS task management to provide a complete OCPP solution on embedded hardware.

The project is designed for:
- Embedded developers implementing **OCPP** in EV chargers.
- Researchers exploring **smart charging infrastructure**.
- IoT enthusiasts interested in embedded **real-time systems** for EVSE.

---

## **Features**

- **OCPP 1.6 Compliance**: Implements core functionalities like secure WebSocket communication and OCPP message handling.
- **TLS Security**: Secure WebSocket connections over **TLS 1.2**, ensuring data integrity and protection against threats.
- **Time Synchronization**: Uses an **NTP client** to synchronize time for accurate certificate validation.
- **FreeRTOS Integration**: Efficient task management with **FreeRTOS**, supporting multitasking for communication, logging, and hardware interaction.
- **Modular Design**: Highly structured project with separate modules for networking, OCPP logic, and hardware abstraction.
- **Cross-Platform Capability**: Portable code adaptable to other STM32 families or embedded platforms.
- **Scalability**: Supports integration with additional peripherals like Wi-Fi modules, SD cards, and cloud interfaces.

---

## **Technologies and Libraries**

### **Embedded Libraries**
1. **MicroOCPP**: Open-source lightweight OCPP library.
2. **Mongoose**: Provides WebSocket and HTTP server/client functionality.
3. **LWIP**: Lightweight TCP/IP stack for networking on resource-constrained devices.
4. **MbedTLS**: Enables secure communication via TLS.
5. **FreeRTOS**: Real-time operating system for multitasking.

### **Hardware Components**
- **Microcontroller**: STM32F4 Series (e.g., STM32F407VGT6).
- **Ethernet**: For networking using the STM32F4's Ethernet MAC.
- **UART**: Optional communication with Wi-Fi modules like ESP-01.

---

## **Repository Structure**

```
STM32F4_MicroOCPP
├── config                 # Configuration files
│   ├── lwipopts.h         # LWIP settings
│   ├── FreeRTOSConfig.h   # FreeRTOS configuration
│   ├── mbedtls_config.h   # TLS settings
├── drivers                # HAL/LL drivers
│   ├── ethernet.c         # Ethernet driver implementation
│   ├── ethernet.h
│   ├── uart.c             # UART driver for Wi-Fi (if applicable)
│   ├── uart.h
├── src                    # Application source code
│   ├── main.c             # Application entry point
│   ├── ocpp_integration.c # OCPP logic
│   ├── wifi_ethernet.c    # Networking setup
│   ├── ntp_client.c       # NTP synchronization
│   ├── tls_config.c       # TLS configuration
│   ├── logging.c          # Logging utilities
│   ├── tasks.c            # FreeRTOS tasks
├── include                # Header files
│   ├── ocpp_integration.h
│   ├── wifi_ethernet.h
│   ├── ntp_client.h
│   ├── tls_config.h
│   ├── logging.h
│   ├── tasks.h
├── third_party            # External libraries
│   ├── MicroOcpp/
│   ├── mongoose/
│   ├── lwip/
├── build                  # Build output (CMake-generated)
├── .github/workflows      # GitHub Actions for CI/CD
│   ├── build.yml          # CI/CD pipeline
├── CMakeLists.txt         # Build configuration
└── README.md              # Project documentation
```

---

## **Setup Instructions**

### **Hardware Requirements**
1. STM32F4 development board (e.g., STM32F407VG).
2. Ethernet connectivity or Wi-Fi module (e.g., ESP-01).
3. Debugging tools (e.g., ST-LINK, UART-to-USB converter).

### **Software Requirements**
1. **STM32CubeMX**: For hardware initialization.
2. **CMake**: Build system generator.
3. **Toolchain**: GCC ARM Embedded or STM32CubeIDE.
4. **Git**: To manage the repository and submodules.

---

### **Steps to Set Up the Project**

1. **Clone the Repository**
   Clone the repository with all submodules:
   ```bash
   git clone --recurse-submodules <repository-url>
   ```

2. **Configure Hardware Initialization**
   - Open **STM32CubeMX** and generate initialization code for:
     - Ethernet (with LWIP enabled).
     - UART (for optional Wi-Fi support).
     - FreeRTOS.
   - Export the generated code and integrate it into the `drivers/` directory.

3. **Build the Project**
   - Create a build directory:
     ```bash
     mkdir build && cd build
     ```
   - Generate the build system using CMake:
     ```bash
     cmake ..
     ```
   - Compile the project:
     ```bash
     cmake --build .
     ```

4. **Flash the Firmware**
   Use **STM32CubeProgrammer** or equivalent to flash the firmware onto the STM32F4.

5. **Monitor Logs**
   - Connect to the UART interface using tools like **PuTTY** or **minicom**.
   - View runtime logs and debug output.

---

## **Project Components**

### **Main Application (`src/main.c`)**
The entry point initializes peripherals, networking, and the MicroOCPP library. It also starts the FreeRTOS scheduler.

### **OCPP Logic (`src/ocpp_integration.c`)**
Handles OCPP message formatting, parsing, and WebSocket communication with the backend.

### **Networking Setup (`src/wifi_ethernet.c`)**
Configures Ethernet or UART-based Wi-Fi for internet access.

### **TLS Configuration (`src/tls_config.c`)**
Initializes MbedTLS for secure WebSocket communication.

### **NTP Client (`src/ntp_client.c`)**
Synchronizes the system clock using SNTP for accurate certificate validation.

### **FreeRTOS Tasks (`src/tasks.c`)**
Defines tasks for handling communication, logging, and hardware interactions.

---

## **Usage Scenarios**

### **1. EVSE Prototype**
- Create a prototype for an OCPP-compliant charging station.
- Test the device's compatibility with OCPP backends like **ChargePoint** or **OCPP Cloud**.

### **2. Embedded Research**
- Use the modular codebase to experiment with secure communication and IoT protocols.

### **3. Educational Use**
- Demonstrates advanced embedded concepts like multitasking, secure communication, and IoT integration.

---

## **Contributing**

We welcome contributions! Follow these steps to contribute:
1. Fork the repository and create a new branch.
2. Implement your changes with detailed comments.
3. Submit a pull request describing your changes.

---

## **License**

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

---

## **Acknowledgments**

- **MicroOCPP** by Matthias Akstaller for the lightweight OCPP implementation.
- **Mongoose** by Cesanta for WebSocket support.
- **STM32CubeMX** by STMicroelectronics for hardware initialization.

---

