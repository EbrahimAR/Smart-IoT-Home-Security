#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

// --- USB TETHERING MODE ---
#include <BlynkSimpleStream.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Servo.h>

// --- PIN MAPPING ---
// LCD Display (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

// Sensors
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define PIR_PIN 8
#define TRIG 9
#define ECHO 10
#define BUZZER 11
#define RELAY 12       // Smart Fan (AC Unit)
#define SERVO_PIN 6
#define NIGHT_LED 4   // LED for Night Light
#define LDR_PIN A0     // Light Sensor

Servo lockServo;
bool systemArmed = false;
BlynkTimer timer;

// --- CONTROL VARIABLES (For App Dashboard) ---
int lightThreshold = 400; // Default darkness level (Adjustable via Slider V5)
bool fanAutoMode = true;  // Default to Automatic (Adjustable via Switch V6)
bool fanManualState = false; // Manual State (Adjustable via Switch V7)

// --- BLYNK SYNCING (Receive Data from App) ---

// V0: Arm/Disarm System
BLYNK_WRITE(V0) {
  int state = param.asInt();
  if (state == 1) armSystem();
  else disarmSystem();
}

// V5: Light Sensitivity Slider (0-1023)
BLYNK_WRITE(V5) {
  lightThreshold = param.asInt(); 
}

// V6: Fan Mode Switch (0 = Manual, 1 = Auto)
BLYNK_WRITE(V6) {
  fanAutoMode = (param.asInt() == 1);
}

// V7: Force Fan Switch (0 = Off, 1 = On)
BLYNK_WRITE(V7) {
  fanManualState = (param.asInt() == 1);
}

// V8: Virtual Panic Button (Push)
BLYNK_WRITE(V8) {
  if (param.asInt() == 1) { // If button pressed
    Blynk.logEvent("intruder_alert", "REMOTE PANIC BUTTON PRESSED!");
    lcd.clear(); 
    lcd.print("REMOTE PANIC!");
    // Siren sound pattern
    for(int i=0; i<3; i++) {
        tone(BUZZER, 1000); delay(500);
        tone(BUZZER, 1500); delay(500);
    }
    noTone(BUZZER);
  }
}

// --- SYSTEM ACTIONS ---
void armSystem() {
  systemArmed = true;
  lockServo.write(90); // Lock door
  Blynk.virtualWrite(V0, 1); // Sync Button Color
  lcd.clear(); lcd.print("SYSTEM ARMED");
  delay(1000);
}

void disarmSystem() {
  systemArmed = false;
  lockServo.write(0); // Unlock door
  noTone(BUZZER);     // Stop Alarm
  Blynk.virtualWrite(V0, 0); // Sync Button Color
  lcd.clear(); lcd.print("SYSTEM DISARMED");
  delay(1000);
}

// --- MAIN INTELLIGENCE LOOP ---
void runSmartHome() {
  // 1. READ SENSORS
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int motion = digitalRead(PIR_PIN);
  int lightLevel = analogRead(LDR_PIN); // 0 (Dark) - 1023 (Bright)
  
  // Read Distance
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000);
  int distance = (duration == 0) ? 0 : duration * 0.034 / 2;

  // 2. SEND TELEMETRY TO APP
  if (!isnan(t)) {
    Blynk.virtualWrite(V3, t);
    Blynk.virtualWrite(V4, h);
  }
  Blynk.virtualWrite(V1, motion);
  Blynk.virtualWrite(V2, distance);

  // --- FEATURE 1: HYBRID SMART FAN CONTROL ---
  // If Auto Mode: Use Temperature Logic
  // If Manual Mode: Use App Switch V7
  if (fanAutoMode) {
    if (!isnan(t) && t > 19) digitalWrite(RELAY, HIGH);
    else digitalWrite(RELAY, LOW);
  } else {
    if (fanManualState) digitalWrite(RELAY, HIGH);
    else digitalWrite(RELAY, LOW);
  }

  // --- FEATURE 2: CALIBRATED NIGHT LIGHT ---
  // Uses V5 Slider value for sensitivity
  if (motion == HIGH && lightLevel < lightThreshold) { 
    digitalWrite(NIGHT_LED, HIGH);
  } else {
    digitalWrite(NIGHT_LED, LOW);
  }

  // --- FEATURE 3: SECURITY MONITORING ---
  if (systemArmed) {
    // Trigger if Motion detected OR Object closer than 20cm
    if (motion == HIGH || (distance > 0 && distance < 20)) {
       tone(BUZZER, 1000); // LOUD Alarm
       Blynk.logEvent("intruder_alert", "Security Breach!"); 
    } else {
       noTone(BUZZER);
    }
  }
  
  // --- LCD DASHBOARD ---
  // Padded with spaces to prevent ghosting
  static int pageCounter = 0;
  pageCounter++;
  if (pageCounter > 6) pageCounter = 0; 
  
  lcd.setCursor(0, 0);
  if (systemArmed) lcd.print("ARMED           "); 
  else lcd.print("DISARMED        ");          

  lcd.setCursor(0, 1);
  if (pageCounter < 3) {
    // Screen 1: Environment
    lcd.print("T:"); lcd.print((int)t);
    lcd.print(" H:"); lcd.print((int)h);
    lcd.print("% L:"); lcd.print(map(lightLevel,0,1023,0,9));
    lcd.print("  "); 
  } else {
    // Screen 2: Security status
    lcd.print("M:"); lcd.print(motion);
    lcd.print(" D:"); lcd.print(distance);
    lcd.print("cm     "); 
  }
}

void setup() {
  // USB Connection Speed
  Serial.begin(9600); 

  lcd.begin(16, 2);
  lcd.print("System Start...");

  // Start Blynk over USB
  Blynk.begin(Serial, BLYNK_AUTH_TOKEN);

  dht.begin();
  lockServo.attach(SERVO_PIN);
  lockServo.write(0); // Default Unlock

  // Set Pin Modes
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(NIGHT_LED, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  // Sync settings from App on startup (So Arduino remembers slider positions)
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);

  // Run the logic every 1000ms
  timer.setInterval(1000L, runSmartHome);
}

void loop() {
  Blynk.run();
  timer.run();
}