<div align="center">
  <h1>Smart Greenhouse Monitoring & Control System</h1>
  <p>
    <img src="https://img.shields.io/badge/Microcontroller-ESP32-blueviolet?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32">
    <img src="https://img.shields.io/badge/Sensor-DHT11-green?style=for-the-badge&logo=temperature&logoColor=white" alt="DHT11">
    <img src="https://img.shields.io/badge/Sensor-MQ--2-orange?style=for-the-badge&logo=gas&logoColor=white" alt="MQ-2">
    <img src="https://img.shields.io/badge/Actuators-Relay%20Modules-blue?style=for-the-badge&logo=plug&logoColor=white" alt="Relays">
    <img src="https://img.shields.io/badge/Cloud-Firebase-critical?style=for-the-badge&logo=firebase&logoColor=white" alt="Firebase">
    <img src="https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
    <img src="https://img.shields.io/badge/Frontend-HTML%20CSS%20JS-brightgreen?style=for-the-badge&logo=html5&logoColor=white" alt="Web">
  </p>
  ---
  <p>
    <a href="#-overview">Overview</a> •
    <a href="#-key-features">Key Features</a> •
    <a href="#-setup-guide">Setup Guide</a> •
    <a href="#-web-dashboard">Web Dashboard</a> •
    <a href="#-screenshots">Screenshots</a>
  </p>
  ---
</div>
<br>

## Overview
**`Smart Greenhouse System`** is an **automated monitoring and control system for 4 independent growing zones (gardens)**, integrating:
- **Environmental sensors**: Temperature, air humidity (DHT11), CO₂ level (simulated from MQ-2)
- **Device control**: LED grow lights, ventilation fans, and irrigation systems per garden
- **Cloud connectivity**: Real-time data with **Firebase Realtime Database**
- **Web interface**: Remote monitoring & control via browser (HTML, CSS, JavaScript)
- **4 independent zones**: Each garden has its own sensors and actuators

> **Use case**: High-tech agriculture, smart greenhouses, precision farming.

<br>

## Key Features

| Feature | Description |
|--------|-------------|
| **4 Independent Gardens** | Each with dedicated sensors & 3 actuators (light, fan, irrigation) |
| **Real-time Monitoring** | Temperature, humidity, CO₂ updated every 2 seconds to Firebase |
| **Remote Control** | Toggle lights, fans, and irrigation via web or mobile |
| **CO₂ Alert System** | Logs warnings when CO₂ exceeds threshold (>2000 ppm) |
| **Auto-Reconnect** | Automatically reconnects to WiFi & Firebase on disconnection |
| **User-Friendly Web UI** | Displays data, control buttons, and syncs state in real-time |
| **Scalable Design** | Clean code structure for easy addition of gardens or sensors |

<br>

- **ESP32**: Collects sensor data → Sends to Firebase → Receives control commands
- **Firebase**: Stores sensor data and actuator states
- **Web Dashboard**: Displays real-time data and enables manual control

<br>

## Setup Guide

### Hardware Requirements
| Component | Quantity | Notes |
|----------|---------|-------|
| ESP32 DevKit | 1 | WiFi + Bluetooth enabled |
| DHT11 | 1 | Temperature & humidity sensor |
| MQ-2 (Gas Sensor) | 1 | Simulates CO₂ detection |
| 5V Relay Module | 4 | Controls lights, fans, pumps |
| LED Grow Lights | 4 | Artificial lighting |
| DC Fans | 4 | Ventilation |
| Water Pump / Solenoid Valve | 4 | Automated irrigation |
| 5V Power Supply | 1 | Powers relays & ESP32 |

<br>

### Software Requirements
- **Arduino IDE**
- **Libraries** (install via Library Manager):
  ```bash
  - Firebase-ESP32
  - DHT sensor library
  - WiFi (built-in)
<br>

## Image
![Image](https://github.com/user-attachments/assets/c60908a0-57e7-4871-b8b9-b14a2a66370d)
![Image](https://github.com/user-attachments/assets/387f5181-d598-43ed-b794-b938c8067907)
![Image](https://github.com/user-attachments/assets/f27eab05-1a58-4f7a-b423-68fe59facb66)
![Image](https://github.com/user-attachments/assets/2b153dd6-45ae-45f5-bb86-b0aa9e42a642)
![Image](https://github.com/user-attachments/assets/5b3ca8d4-6c07-42b9-b140-f721a633ba4a)
![Image](https://github.com/user-attachments/assets/915c6603-efad-4ec7-bd9b-a5d309e4a487)
![Image](https://github.com/user-attachments/assets/cdb4f8ce-4a4a-415d-a0a2-e2839d8b8504)

## Chart 
![Image](https://github.com/user-attachments/assets/212785f4-5bef-4c6f-a705-1adf44ef096)
<br>

## Demo Video
[![Demo Video](https://github.com/user-attachments/assets/f1a7e4cf-0209-403c-af77-dd950d47fb90)](https://www.youtube.com/watch?v=LrGv_cqwNVA)

<div align="center">
  <p><strong>© 2025 – HCMUTE Project </strong></p>
  <p><i>Nguyễn Phạm Huy Hoàng - 22161125</i> |
  <p><em>Precision time. Environmental awareness. Green House.</em></p>
</div>