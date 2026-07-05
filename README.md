# Edge AI Engine Fault Detection System

## Overview

The Edge AI Engine Fault Detection System is an embedded machine learning application that performs real-time engine fault prediction using an STM32 microcontroller. The system demonstrates the deployment of a TensorFlow Lite model on resource-constrained hardware for on-device inference, eliminating the need for cloud-based prediction while maintaining low latency.

The trained model predicts the Engine Malfunction Indicator (MIL) status from engine sensor parameters. Prediction results are transmitted over UART to an ESP32 module, which uploads the engine health status to the ThingSpeak cloud platform for remote monitoring. A web dashboard provides real-time visualization of engine status, service history, uptime analysis, and location-based service recommendations.

---

## Key Features

- Real-time engine fault prediction using Edge AI
- TensorFlow Lite model deployment on STM32 using X-CUBE-AI
- Low-latency inference on embedded hardware
- UART communication between STM32 and ESP32
- Cloud connectivity through ThingSpeak
- Interactive web dashboard for engine health monitoring
- Service history management
- Vehicle uptime analytics
- Google Maps integration for nearby service center recommendations
- Modular software architecture for embedded AI applications

---

## System Architecture

```
                  Engine Sensor Data
                          │
                          ▼
            Machine Learning Model Training
                    (TensorFlow/Keras)
                          │
                          ▼
              TensorFlow Lite Conversion
                          │
                          ▼
            STM32 + X-CUBE-AI Deployment
                 (Edge AI Inference)
                          │
                 UART Communication
                          │
                          ▼
                       ESP32
                 Wi-Fi Communication
                          │
                          ▼
                  ThingSpeak Cloud
                          │
                          ▼
               Web Monitoring Dashboard
```

---

## Technology Stack

### Machine Learning

- TensorFlow
- TensorFlow Lite
- Scikit-learn
- NumPy
- Pandas

### Embedded Systems

- STM32 Nucleo Development Board
- STM32CubeIDE
- STM32 X-CUBE-AI
- ESP32
- UART Communication

### Web Technologies

- HTML5
- React
- Tailwind CSS
- Chart.js
- JavaScript

### Cloud Platform

- ThingSpeak

### Development Tools

- Python
- Arduino IDE
- STM32CubeProgrammer
- Git
- GitHub

---

## Repository Structure

```
Edge-AI-Engine-Fault-Detection-System/

├── dataset/
│
├── ml/
│   ├── train_stm32_mil.py
│   └── artifacts/
│
├── stm32/
│   └── Core/
│       └── Src/
│           └── main.c
│
├── esp32/
│   └── obd_health_logger.ino
│
├── dashboard/
│   └── index.html
│
├── docs/
│   ├── Procedure.pdf
│   └── Program.pdf
│
├── README.md
├── requirements.txt
├── LICENSE
└── .gitignore
```

---

## Workflow

1. Train the machine learning model using the engine dataset.
2. Convert the trained model into TensorFlow Lite format.
3. Deploy the TensorFlow Lite model to the STM32 microcontroller using STM32 X-CUBE-AI.
4. Perform real-time inference on embedded hardware.
5. Transmit prediction results to the ESP32 through UART.
6. Upload engine status to the ThingSpeak cloud.
7. Monitor engine health through the web dashboard.

---

## Installation

### Clone the Repository

```bash
git clone https://github.com/Prasanna51225/Edge-AI-Engine-Fault-Detection-System-.git
cd Edge-AI-Engine-Fault-Detection-System-
```

### Install Python Dependencies

```bash
pip install -r requirements.txt
```

### STM32 Deployment

1. Open the STM32 project in STM32CubeIDE.
2. Import the TensorFlow Lite model using STM32 X-CUBE-AI.
3. Build and flash the firmware to the STM32 board.

### ESP32 Deployment

1. Open the Arduino project.
2. Configure Wi-Fi credentials and ThingSpeak API keys.
3. Upload the firmware to the ESP32.

### Dashboard

Open `dashboard/index.html` in a web browser after configuring the required cloud parameters.

---

## Documentation

Detailed implementation, setup instructions, and source code are available in:

- `docs/Procedure.pdf`
- `docs/Program.pdf`

---

## Hardware Requirements

- STM32 Nucleo Development Board
- ESP32 Development Board
- ST-Link Programmer
- USB Cables

---

## Software Requirements

- Python 3.8 or later
- STM32CubeIDE
- STM32CubeProgrammer
- Arduino IDE
- ThingSpeak Account

---

## Project Components

### Machine Learning

Responsible for training the classification model, preprocessing input features, and generating TensorFlow Lite artifacts for embedded deployment.

### STM32 Firmware

Executes the TensorFlow Lite model using STM32 X-CUBE-AI, processes normalized input features, performs inference, and transmits prediction results over UART.

### ESP32 Firmware

Receives inference results from STM32, establishes Wi-Fi connectivity, and uploads engine fault information to the ThingSpeak cloud platform.

### Web Dashboard

Displays real-time engine health information, service history, uptime statistics, and cloud data visualization through an interactive interface.

---

## Future Enhancements

- Integration with live OBD-II hardware
- CAN Bus communication support
- TinyML model optimization
- Multi-class engine fault classification
- MQTT-based communication
- Mobile application support
- Predictive maintenance analytics
- Edge model update mechanism

---


This project is distributed under the MIT License.
