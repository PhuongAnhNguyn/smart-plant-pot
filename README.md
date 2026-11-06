# 🌱 Smart Plant Pot

**An IoT-based smart plant monitoring system using ESP32-S3.**

## 📌 Overview

Smart Plant Pot is a personal IoT project designed to monitor environmental conditions and soil moisture.

The system uses an ESP32-S3 to collect sensor data, display measurements on an OLED screen, and provide a web dashboard accessible through Wi-Fi.

## ✨ Features

- Temperature monitoring
- Ambient humidity monitoring
- Soil moisture monitoring
- Real-time data display on an OLED screen
- Local web dashboard hosted by ESP32-S3

## 🛠️ Hardware Components

| Component | Purpose |
|---|---|
| ESP32-S3 | Main microcontroller |
| DHT11 | Temperature and humidity monitoring |
| Soil Moisture Sensor | Soil moisture measurement |
| SSD1306 OLED | Display sensor readings |
| Breadboard | Circuit prototyping |

## 💻 Technologies

- C++
- Arduino IDE
- ESP32 Wi-Fi
- HTTP Web Server
- I2C Communication
- Analog-to-Digital Conversion (ADC)

## 📂 Project Structure

```text
smart-plant-pot/
├── src/
│   └── smart_plant_pot.ino
├── docs/
│   └── circuit-diagram.md
└── README.md
```

## 🚧 Development Status

**Status: Under Development**

Currently working on hardware integration, sensor testing, and system improvements.

## 🔮 Future Improvements

- Calibrate soil moisture sensor readings
- Improve the web dashboard interface
- Add plant condition indicators
- Implement sensor data logging

## 👨‍💻 Author

Developed as a personal IoT learning project.
