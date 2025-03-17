#define TARGET_TEMP 50

void setup() {
  // put your setup code here, to run once:

}

float readTemp(){

}

/*
 * Read get current temperature and measuring 
*/
bool isTemp(float currTemp){
  if(currTemp < TARGET_TEMP){
    return false;
  }
  return true;
}

/* Turn the pump on
 * Wait N amount of time
 * Measure the temperature
 * Check against target temp 
*/
void raiseTempLoop(){

}
/*
 * Turn the pump on
*/
void turnPumpOn(){

}
/*
 * Turn the pump off
*/
void turnPumpOff(){

}

void loop() {
  // put your main code here, to run repeatedly:
  float currTemp = readTemp();
}
