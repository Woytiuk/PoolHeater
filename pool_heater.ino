#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// === LCD Setup ===
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// === Optional Temp Sensor Setup ===
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define TARGET 30
#define UPPER 82//Upper threshold of the pool temp
#define LOWER 80//Lower threshold of the pool temp

// === Pump Control Setup ===
#define PUMP_PIN 6

// === Timer Settings ===
const unsigned long pumpOnDuration = 5000;  // 5 seconds ON
const unsigned long pumpOffDuration = 5000; // 5 seconds OFF

// === State Tracking ===
bool pumpOn = false;
unsigned long lastToggleTime = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  sensors.begin();  // OK if sensor not connected
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  lcd.print("Svarog Ready");
  delay(2000);
  lcd.clear();
}

float cToF(float celsius){
  return (celsius * 9/5) + 32;
}

void loop() {
  //unsigned long currentMillis = millis();

/*if (pumpOn && (currentMillis - lastToggleTime >= pumpOnDuration)) {
    pumpOn = false;
    lastToggleTime = currentMillis;
    digitalWrite(PUMP_PIN, LOW);
  } else if (!pumpOn && (currentMillis - lastToggleTime >= pumpOffDuration)) {
    pumpOn = true;
    lastToggleTime = currentMillis;
    digitalWrite(PUMP_PIN, HIGH);
  }
*/
  // real temp
  
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0); // Shows -127 if no sensor is connected
  float tempF = cToF(tempC);
  if (pumpOn && tempF > UPPER){
    pumpOn = false;
    digitalWrite(PUMP_PIN, LOW);
  } else if (!pumpOn && tempF < LOWER){
    pumpOn = true;
    digitalWrite(PUMP_PIN, HIGH);
  }
/* === Read in Temperature ===
  
  // === Pump Control Logic ===
  if (pumpOn && tempC > TARGET){
    pumpOn = false;
    digitalWrite(PUMP_PIN, LOW);
  } else if (!pumpOn && tempC < TARGET) {
    pumpOn = true;
    digitalWrite(PUMP_PIN, HIGH);
  }
*/
  // === LCD Output ===
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(tempC, 1);
  lcd.print("C|");
  lcd.print(tempF, 1);
  lcd.print("F ");

  lcd.setCursor(0, 1);
  lcd.print("Pump: ");
  lcd.print(pumpOn ? "ON " : "OFF");

  delay(500); // Update every half second
}
