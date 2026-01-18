#define DISABLE_ENS160_I2C_HOOK   // Disable ENS160 internal I2C hook to avoid conflicts

#include <Wire.h> // Already integrated in Arduino
#include <Adafruit_AHTX0.h> // Download Link: https://github.com/adafruit/Adafruit_AHTX0
#include <ScioSense_ENS160.h> // Download Link: https://github.com/sciosense/ENS160_driver
#include <Servo.h> // Already integrated in Arduino
#include <U8x8lib.h> // Download Link: https://github.com/olikraus/u8glib

U8X8_SSD1306_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);  // OLED driver (text-only, low memory)

// PINS
#define SERVO_PIN   9            // Servo control pin
#define BTN_TOGGLE  2            // Button input (pull-up)
#define LED_RED     7            // Warning LED
#define BUZZER_PIN  6            // Buzzer output

#define VENT_OPEN   100          // Servo angle when vent is open
#define VENT_CLOSE  0            // Servo angle when vent is closed

// SENSORS
Adafruit_AHTX0 aht;              // Temperature & humidity sensor
ScioSense_ENS160 ens160(0x53);   // ENS160 air quality sensor (I2C address 0x53)
Servo ventServo;                 // Servo object

// VARIABLES
int tempC = 0;                   // Temperature in Celsius
int humidity = 0;                // Relative humidity (%)
uint16_t eco2 = 0;               // Equivalent CO2 (ppm)
uint8_t aqi = 0;                 // Air Quality Index (ENS160)

const char* co2Label = "SAFE";   // Text label for CO2 status

bool ventOpen = false;           // Vent state (open/closed)
bool lastButton = HIGH;          // Previous button state (for edge detection)

unsigned long lastSample = 0;    // Last sensor sampling time
unsigned long lastOLED = 0;      // Last OLED refresh time


// CENTERED TEXT FUNCTION
void centerText(int row, const char* text) {
  int len = strlen(text);        // Length of text
  int col = (16 - len) / 2;      // Calculate center column (16 chars wide)
  if (col < 0) col = 0;          // Safety clamp
  display.drawString(col, row, text);  // Draw centered text
}


// SETUUUUPPPPPPPPP
void setup() {

  delay(300);                    // Power stabilization delay

  Wire.begin();                  // Start I2C bus
  Wire.setClock(100000);          // Standard I2C speed (stable for ENS160)

  pinMode(BTN_TOGGLE, INPUT_PULLUP);  // Button uses internal pull-up
  pinMode(LED_RED, OUTPUT);           // LED output
  pinMode(BUZZER_PIN, OUTPUT);        // Buzzer output

  ventServo.attach(SERVO_PIN);   // Attach servo
  ventServo.write(VENT_CLOSE);   // Start with vent closed

  // OLED INITIALIZATION
  display.begin();               // Initialize OLED
  display.clear();               // Clear display buffer
  display.setFont(u8x8_font_chroma48medium8_r);  // Default font

  centerText(2, "AIR QUALITY");  // Intro screen title
  centerText(4, "MONITOR");
  delay(900);

  display.clear();
  centerText(2, "Initializing");  // Startup message
  for (int i=0; i<6; i++) {
    display.drawString(4+i, 4, ".");  // Loading animation
    delay(220);
  }

  // READY SCREEN
  display.clear();
  display.drawString(4, 3, "SYSTEM READY");  // System ready message
  delay(600);
  display.clear();               // Clear intro remnants

  // SENSOR STARTUP
  if (!aht.begin()) {            // Initialize AHT20
    display.clear();
    centerText(3, "AHT ERROR!");
    while (1);                   // Halt if sensor fails
  }

  if (!ens160.begin()) {         // Initialize ENS160
    display.clear();
    centerText(3, "ENS160 ERR!");
    while (1);                   // Halt if sensor fails
  }

  ens160.setMode(ENS160_OPMODE_STD);  // Standard operating mode
  delay(1800);                   // ENS160 warm-up time
}


// LOOOOOOPPPPPPPP
void loop() {
  unsigned long now = millis();  // Current time

  // READ SENSORS (1 Hz)
  if (now - lastSample >= 1000) {
    lastSample = now;            // Update sampling timestamp

    sensors_event_t humEv, tempEv;
    aht.getEvent(&humEv, &tempEv);  // Read temperature & humidity

    tempC = (int)tempEv.temperature;
    humidity = (int)humEv.relative_humidity;

    ens160.set_envdata(tempC, humidity);  // Compensate ENS160 readings
    ens160.measure(true);                 // Trigger measurement
    ens160.measureRaw(true);              // Raw measurement (recommended)

    if (ens160.available()) {
      eco2 = ens160.geteCO2();             // Read CO2
      aqi  = ens160.getAQI();              // Read AQI
    }

    // CO2 Classification
    if (eco2 <= 800)       co2Label = "SAFE";    // Good air
    else if (eco2 <=1500)  co2Label = "POOR";    // Moderate air
    else                   co2Label = "DANGER";  // Dangerous air

    // Warning Logic
    if (eco2 > 1500) {                  // Emergency ventilation
      digitalWrite(LED_RED, HIGH);
      tone(BUZZER_PIN, 2000);
      ventOpen = true;
      ventServo.write(VENT_OPEN);
    }
    else if (eco2 > 800) {              // Warning state
      digitalWrite(LED_RED, HIGH);
      tone(BUZZER_PIN, 1200, 200);
    }
    else {                              // Safe state
      digitalWrite(LED_RED, LOW);
      noTone(BUZZER_PIN);
    }

    // Button State (manual control only when SAFE)
    if (eco2 <= 800) {
      bool btn = digitalRead(BTN_TOGGLE);
      if (btn == LOW && lastButton == HIGH) {
        ventOpen = !ventOpen;            // Toggle vent
      }
      lastButton = btn;
      ventServo.write(ventOpen ? VENT_OPEN : VENT_CLOSE);
    }
    else {
      ventServo.write(VENT_OPEN);        // Forced ventilation when unsafe
    }
  }

  // OLED UPDATE (2 FPS)
  if (now - lastOLED >= 500) {
    lastOLED = now;                      // Update OLED timer

    display.setFont(u8x8_font_amstrad_cpc_extended_r);  // Header font
    centerText(0, "AIR QUALITY");         // Header

    display.setFont(u8x8_font_chroma48medium8_r);       // Data font

    char buf[20];

    // Temperature display
    sprintf(buf, "%3d C   ", tempC);
    display.drawString(0, 2, "Temp:");
    display.drawString(7, 2, buf);

    // Humidity display
    sprintf(buf, "%3d %%  ", humidity);
    display.drawString(0, 3, "Hum :");
    display.drawString(7, 3, buf);

    // CO2 display
    sprintf(buf, "%4u ppm ", eco2);
    display.drawString(0, 4, "CO2 :");
    display.drawString(7, 4, buf);

    // Air quality label
    sprintf(buf, "%-7s ", co2Label);
    display.drawString(0, 6, "Air :");
    display.drawString(7, 6, buf);

    // Vent status display
    display.drawString(0, 7, "Vent:");
    if (eco2 <= 800)
        display.drawString(7, 7, ventOpen ? "OPEN   " : "CLOSED ");
    else if (eco2 <= 1500)
        display.drawString(7, 7, "FORCED  ");
    else
        display.drawString(7, 7, "EMERG!  ");
  }
}