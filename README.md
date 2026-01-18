# Air-Quality-Monitor-with-Ventilation-Control-Arduino-Project-
An Arduino-based air quality monitoring system using the ENS160 gas sensor and AHT20 humidity/temperature sensor, featuring automatic ventilation control, safety alerts, and a compact OLED user interface.

## ⭐ Features

- ENS160 CO₂ monitoring (SAFE / POOR / DANGER)
- AHT20 temperature & humidity sensing
- Automatic emergency ventilation when CO₂ is high
- Manual vent toggle when air is safe
- Warning LED and audible buzzer alerts
- 128×64 U8x8 OLED display
- Servo-driven vent/door mechanism
- Battery-friendly (portable system)

---

## 🛠 Hardware Components

| Component | Description |
|----------|-------------|
| **Arduino Uno / Nano** | Main microcontroller |
| **ENS160 Gas Sensor** | CO₂, TVOC, AQI sensing (I²C) |
| **AHT20 Sensor** | Temperature & humidity (I²C) |
| **128×64 OLED (U8x8)** | Display interface |
| **SG90 / MG90S Servo** | Vent/door actuator |
| **Red LED** | Visual alert |
| **Buzzer** | Audible safety alarm |
| **Push Button** | Manual vent control |
| **Power Bank / USB 5V** | Stable power source |

---

## 🔌 Wiring Diagram (I²C Shared Bus)

| Module | Arduino Pin |
|--------|-------------|
| ENS160 SDA | A4 |
| ENS160 SCL | A5 |
| AHT20 SDA | A4 |
| AHT20 SCL | A5 |
| OLED SDA | A4 |
| OLED SCL | A5 |
| Servo Signal | D9 |
| Button | D2 (INPUT_PULLUP) |
| LED | D7 |
| Buzzer | D6 |

**Note:** All I²C devices share A4/A5 using parallel connection.

---

## 🧠 System Logic

### **CO₂ Threshold Logic**
| CO₂ Level | Status | Vent Control | Alerts |
|----------|--------|--------------|--------|
| **≤ 800 ppm** | SAFE | Manual button allowed | No alerts |
| **801–1500 ppm** | POOR | Forced ventilation | LED on, soft buzzer |
| **> 1500 ppm** | DANGER | Emergency forced vent | LED on, loud buzzer |

---

## 📺 Display Interface (OLED 128×64)

Displayed values:
- Temperature (°C)
- Humidity (%)
- CO₂ concentration (ppm)
- Air quality label (SAFE / POOR / DANGER)
- Vent mode (OPEN / CLOSED / FORCED / EMERG)

---

## 🪛 How to Upload the Code

1. Install Arduino IDE
2. Select **Arduino Uno / Nano**
3. Install required libraries:
   - Adafruit AHTX0
   - U8x8lib
   - Servo
   - ScioSense ENS160
4. Upload `.ino` file
5. Power using USB or 5V power bank

---

## ⚡ Recommended Power Source

- **5V USB power bank** (best & stable)
- **Phone charger adapter (5V)**  
- Avoid **9V rectangular batteries** — too weak for servo & sensors

---

## 📝 License

This project is open-source under the **MIT License**.  
You may modify, improve, or use it for research and education.

---

## 💬 Author

Developed by **[Your Name]**  
Electronics Engineering Student  
Passionate about Applied Sciences, Sensors, and Embedded Systems

---

## ⭐ Star This Repo!

If this project helped you, consider starring it on GitHub!

