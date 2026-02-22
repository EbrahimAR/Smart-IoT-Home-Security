# 🏠🔒 Smart IoT Home Security System

## 📌 Project Overview
Traditional locks are reactive—if a breach happens while you are away, the system fails silently. This project transforms a physical lock into an active, cloud-connected guardian.

The **Smart IoT Home Security System** is a hybrid security solution built on the **Arduino Mega 2560** and connects to the internet via **USB Serial communication (Blynk Stream mode)** instead of a Wi-Fi module.

The Arduino connects directly to a computer using USB, and the computer provides internet access for cloud communication through the **Blynk IoT platform**.

---

## 🧠 System Architecture
  - Arduino Mega 2560 connected via USB
  - BlynkSimpleStream library used
  - PC acts as internet bridge

---

## ✨ Key Features
- 🔐 Hybrid Control System
  - Arm/Disarm remotely using Blynk App (V0)
  - Servo-controlled locking mechanism
  - Real-time state synchronization with dashboard

- 📡 Real-Time Environmental Monitoring
  - Temperature (DHT11 → V3)
  - Humidity (DHT11 → V4)
  - Motion Status (PIR → V1)
  - Distance Monitoring (Ultrasonic → V2)
  - Light Level Monitoring (LDR)

- 🚨 Intruder Detection
    When the system is Armed:

    - Triggers if:
      - Motion detected (PIR)
      - OR object detected within 20cm (Ultrasonic)

    - On breach:
      - Loud buzzer alarm
      - Push notification via:

    ```bash
    Blynk.logEvent("intruder_alert", "Security Breach!");
    ```

    - 💡 Smart Night Light (LDR + Motion Based)
      - Adjustable darkness threshold (V5 Slider)
      - LED activates only when:
        - Motion detected
        - AND room is dark

    - 🌬 Hybrid Smart Fan Control (Relay Based)
    Two Modes:

    - 🔄 Auto Mode (V6 = ON)
      - Fan turns ON if temperature > 19°C
      - Turns OFF otherwise

    - 🎮 Manual Mode (V6 = OFF)
      - Controlled directly via Switch V7

- 🚨 Virtual Panic Button (V8)
When pressed:
  - Sends Blynk event notification
  - Triggers siren sound pattern
  - Displays "REMOTE PANIC!" on LCD

- 📟 Live LCD Dashboard
Displays:

  - Screen 1:
      - Temperature
      - Humidity
      - Light Level

  - Screen 2:
      - Motion status
      - Distance reading

Automatically alternates every few cycles.

---

## 🧰 Hardware Components

  🧠 Microcontroller
  - **Arduino Mega 2560**

  🌡 Sensors
  - DHT11 (Temperature & Humidity)
  - PIR Motion Sensor
  - HC-SR04 Ultrasonic Sensor
  - LDR (Light Sensor)

  🔊 Outputs
  - Servo Motor (Door Lock)
  - 5V Active Buzzer
  - 16x2 LCD Display
  - Relay Module (Smart Fan / AC)
  - LED (Night Light)

---

- 🔌 Connectivity Mode

    This version uses:
    ```bash
    #include <BlynkSimpleStream.h>
    ```

    Connection initialized with:
    ```bash
    Serial.begin(9600);
    Blynk.begin(Serial, BLYNK_AUTH_TOKEN);
    ```

    How It Works
    1. Arduino connects to PC via USB.
    2. Blynk runs through USB Serial stream.
    3. PC provides internet connectivity.
    4. Blynk Cloud communicates through the PC.

    ⚠️ Important:
    The Arduino must remain connected to a powered computer for remote functionality.

---

## 🛠 Getting Started
  1️⃣ Install Required Libraries
  - Blynk
  - LiquidCrystal
  - DHT
  - Servo

  2️⃣ Update Credentials
Edit these lines:
```bash
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

  3️⃣ Upload Code
  - Select **Arduino Mega 2560**
  - Set baud rate to 9600
  - Upload sketch
  - Open Serial Monitor (if required by Blynk setup)

---

## 🧠 Engineering Design Highlights
  ✅ USB Stream Architecture
  - Eliminates need for Wi-Fi modules
  - Reduces hardware complexity
  - Stable Serial communication at 9600 baud

  ✅ Non-Blocking Logic
  - Uses `BlynkTimer`
  - No blocking delays in main loop
  - Continuous sensor monitoring

---

## ⚠️ Limitations of USB Version
  - Requires constant PC connection
  - Not standalone
  - Not portable
  - No operation during PC shutdown

---

## 🚀 Future Upgrade Path
To make the system standalone:
  - Upgrade to ESP8266 or ESP32
  - Add Wi-Fi-based Blynk connection
  - Add battery backup
  - Add ESP32-CAM for photo capture
  - Add Alexa / Google Assistant integration

---

## 👨‍💻 Author

Ebrahim Abdul Raoof

[LinkedIn](https://www.linkedin.com/in/ebrahim-ar/)

[GitHub](https://github.com/EbrahimAR)

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](https://github.com/EbrahimAR/E-Sports-Results-Tracker/blob/main/LICENSE) for details.#
