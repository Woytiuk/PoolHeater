#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// === LCD Setup ===
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// === Temperature Sensor Setup ===
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float upperThreshold = 82;
float lowerThreshold = 80;

// === Pump Control Setup ===
#define PUMP_PIN 6

// === State Tracking ===
bool pumpOn = false;
unsigned long lastPumpOn = 0;
unsigned long totalPumpMillis = 0;
unsigned long lastSave = 0;

void loadPumpTime() {
  unsigned long storedTotal;
  EEPROM.get(100, storedTotal);
  if (!isnan(storedTotal)) {
    totalPumpMillis = storedTotal;
  } else {
    totalPumpMillis = 0;
  }
}

void savePumpTime(unsigned long sessionMillis) {
  unsigned long stored;
  EEPROM.get(100, stored);
  if (isnan(stored)) stored = 0;
  unsigned long averaged = (stored + sessionMillis) / 2;
  EEPROM.put(100, averaged);
  totalPumpMillis = averaged;
}

void resetPumpTime() {
  totalPumpMillis = 0;
  EEPROM.put(100, totalPumpMillis);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  sensors.begin();
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  loadPumpTime();

  lcd.print("Svarog Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Handle serial input from the Flask app
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.startsWith("SET")) {
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      float minT = input.substring(firstSpace + 1, secondSpace).toFloat();
      float maxT = input.substring(secondSpace + 1).toFloat();
      lowerThreshold = minT;
      upperThreshold = maxT;
      Serial.println("Thresholds updated.");
    } else if (input == "READ") {
      Serial.println(totalPumpMillis / 60000); // minutes
    } else if (input == "RESET") {
      resetPumpTime();
      Serial.println("Pump time reset.");
    }
  }

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  //Needed as a fail safe for the temp sensor
  if (tempC == -127.0) {
    lcd.setCursor(0, 0);
    lcd.print("Temp Sensor Err ");
    lcd.setCursor(0, 1);
    lcd.print("Pump: ");
    lcd.print(pumpOn ? "ON " : "OFF");
    delay(1000);
    return; // Skip rest of loop
  }
  if (!pumpOn && tempF < lowerThreshold) {
    pumpOn = true;
    digitalWrite(PUMP_PIN, HIGH);
    lastPumpOn = millis();
  } else if (pumpOn && tempF > upperThreshold) {
    pumpOn = false;
    digitalWrite(PUMP_PIN, LOW);
    unsigned long sessionMillis = millis() - lastPumpOn;
    savePumpTime(sessionMillis);
  }

  if (millis() - lastSave > 300000) {
    savePumpTime(0);
    lastSave = millis();
  }

  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(tempC, 1);
  lcd.print("C|");
  lcd.print(tempF, 1);
  lcd.print("F");

  lcd.setCursor(0, 1);
  lcd.print("Pump: ");
  lcd.print(pumpOn ? "ON " : "OFF");

  delay(500);
}