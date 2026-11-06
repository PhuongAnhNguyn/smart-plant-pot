Smart Plant Pot 🌱

An IoT-based plant monitoring system built with ESP32-S3.

Overview

Smart Plant Pot is a mini IoT project designed to monitor environmental conditions and soil moisture to support plant care.

The system uses an ESP32-S3 to collect sensor data, display measurements on an OLED screen, and provide a web dashboard accessible through Wi-Fi.

Features

Monitor ambient temperature and humidity using DHT11.

Measure soil moisture using an analog soil moisture sensor.

Display sensor readings on an OLED screen.

Host a local web dashboard using ESP32 Wi-Fi Access Point.

View real-time sensor data through a browser.

Hardware Components

ESP32-S3

DHT11 temperature and humidity sensor

Analog soil moisture sensor

0.96-inch OLED display (SSD1306)

Breadboard and jumper wires

Software & Technologies

Arduino IDE

C++

ESP32 Wi-Fi

HTTP Web Server

I2C

Analog-to-Digital Conversion (ADC)

Project Structure

smart-plant-pot/
├── src/
│   └── smart_plant_pot.ino
├── docs/
│   ├── circuit-diagram.png
│   └── project-photos/
├── README.md
├── LICENSE
└── .gitignore

Current Status

Project under development.

Future Improvements

Calibrate soil moisture readings.

Improve the web dashboard interface.

Add plant condition indicators.

Implement data logging and historical monitoring.

Author

Developed as a personal IoT learning project.
