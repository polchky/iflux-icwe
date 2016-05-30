#include <Adafruit_NeoPixel.h>

const int N_STRIP_LEDS = 11;

const char ORDER_DELIMITER = '/';
const char ORDER_RING = 'r'; // Used
const char ORDER_SETUP = 's';
const char ORDER_DEV = 'd'; // used
const char ORDER_RESET = 'x';
const char ORDER_ADD = 'a';
const char ORDER_COMMIT = 'c'; // used

const int STATE_IDLE = 0; // used
const int STATE_COMMIT = 1; // used
const int STATE_SELECTING = 2; // used
const int STATE_REPLAYING = 3; // used
const int STATE_RECEIVING = 4;

int BRIGHTNESS = 20;

int nDevs = 7;
int currentState = STATE_IDLE;
String inputString = "";
unsigned long orderStart;
int deltaT = 50;

// Commit variables
uint32_t commitColor;
int commitModule;
int commitRingIndex;
int commitRingLength;
int nCommits;
unsigned long commitEndTime;

uint32_t colors[8] = {
  0,
  100,
  25500,
  6502500,
  25600,
  6502600,
  6528000,
  6528100
};

// Dev selection
uint16_t analogLastValue;
unsigned long analogLastChecked;
unsigned long analogLastMoved;
unsigned long analogTimeThreshold = 1000;
uint8_t selectedDev;

Adafruit_NeoPixel rings[4] = {
  Adafruit_NeoPixel(24, 6, false, 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(24, 6, false, 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(24, 6, false, 9, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(24, 6, false, 10, NEO_GRB + NEO_KHZ800)
};

Adafruit_NeoPixel strips[4] = {
  Adafruit_NeoPixel(N_STRIP_LEDS, 0, true, 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_STRIP_LEDS, 0, true, 7, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_STRIP_LEDS, 0, true, 8, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_STRIP_LEDS, 0, true, 11, NEO_GRB + NEO_KHZ800)  
};

Adafruit_NeoPixel weeks = Adafruit_NeoPixel(16, 0, false, 12, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel months = Adafruit_NeoPixel(24, 0, false, 13, NEO_GRB + NEO_KHZ800);



void announceFaultyOrder(){
  Serial.println("resend");
}

String getInputStringNextPart(){
  String nextPart = "";
  do{
    char currentChar = inputString.charAt(0);
    inputString.remove(0, 1);
    if(currentChar == ORDER_DELIMITER) {
      return nextPart;
    } else {
      nextPart += currentChar;
    }
  } while(inputString.length() > 0);
}

void switchState(int newState){
  switch (newState){
    case STATE_COMMIT:
      clearStrips();
      commitColor = colors[getInputStringNextPart().toInt()];
      commitModule = getInputStringNextPart().toInt();
      commitRingIndex = getInputStringNextPart().toInt();
      commitRingLength = getInputStringNextPart().toInt();
      nCommits = getInputStringNextPart().toInt();
      commitEndTime = 2;
      break;
    case STATE_SELECTING:
      clearStrips();
      clearWeeks();
      clearRings();
      analogLastMoved = millis();
      break;
    case STATE_REPLAYING:
      Serial.println("replaying");
      break;
    case STATE_IDLE:
      clearStrips();
      clearTimeRings();
      break;
    default: break;
  }
  
  currentState = newState;
}

/**
 * Checks the integrity of the last received message
 */
boolean checkInputString(){
  inputString.remove(inputString.length() - 1, 1);
  if(inputString.charAt(0) != 'l') {
    return false;
  }
  String orderLength = getInputStringNextPart();
  orderLength.remove(0,1);
  return orderLength.toInt() == inputString.length();
}

void executeOrder(){
  orderStart = millis();
  char order = getInputStringNextPart().charAt(0);
  switch(order){
    case ORDER_RING:
      setRingDisplay();
      break;
    case ORDER_DEV:
      nDevs = getInputStringNextPart().toInt();
      break;
    case ORDER_COMMIT:
      switchState(STATE_COMMIT);
      break;
    default: 
      //announceFaultyOrder();
      break;
  }
}

void setRingDisplay(){
  int ringIndex = getInputStringNextPart().toInt();
  String positions = getInputStringNextPart();
  for (int i=0; i<rings[ringIndex].numPixels(); i++){
    rings[ringIndex].setPixelColor(i, colors[positions.substring(i, i+1).toInt()]);
  }
  rings[ringIndex].show();
}

void checkSerial() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');
    if(checkInputString() == true){
      executeOrder();
    } else {
      announceFaultyOrder();
    }
  }
}

void clearWeeks(){
  for (int i=0; i<weeks.numPixels(); i++){
    weeks.setPixelColor(i, 0);
  }
  weeks.show();
}

void clearRings(){
  for (int i=0; i<4; i++){
    for (int j=0; j<rings[i].numPixels(); j++){
      rings[i].setPixelColor(j,0);
    }
    rings[i].show();
  }
}

/*
 * Switches off all the strips and central rings pixels
 */
void clearStrips(){
  for (int i=0; i<4; i++){
    for (int j=0; j<strips[i].numPixels(); j++){
      strips[i].setPixelColor(j, 0);
    }
    strips[i].show();
  }
}

void doCommit(){
  int cStep = (millis() - orderStart) / deltaT;
  // Set strip
  for (int i=0; i<strips[commitModule].numPixels(); i++){
    if(((8 - i + cStep) % 12) / 4 == 0){
      strips[commitModule].setPixelColor(i, commitColor);
    } else{ 
      strips[commitModule].setPixelColor(i, 0);
    }
  }
  strips[commitModule].show();
  // Set ring
  
}

uint32_t illum(uint32_t color, uint8_t brightness){
  return 
    (color & (255 << 16)) / BRIGHTNESS * brightness +
    (color & (255 << 8)) / BRIGHTNESS * brightness + 
    (color & (255) ) / BRIGHTNESS;
}

void clearTimeRings(){  
  for (int j=0; j<weeks.numPixels(); j++){
    weeks.setPixelColor(j, 0);
  }
  weeks.show();
  for (int j=0; j<months.numPixels(); j++){
    months.setPixelColor(j, 0);
  }
  months.show();
}

void checkAnalog(){
  if(currentState == STATE_SELECTING || (analogLastChecked + 500) > millis()){
    return;
  }
  analogLastChecked = millis();
  uint16_t val = 1023 - analogRead(4);
  if(val + 1 < analogLastValue ||  (val != 0) && val - 1 > analogLastValue){
    switchState(STATE_SELECTING);
  }
  analogLastValue = val;
}

void doSelect(){
  
  // Read and store value
  uint16_t val = 1023 - analogRead(4);
  selectedDev = val * nDevs / 1024;
  
  if(val + 1 < analogLastValue || (val != 0) && val - 1 > analogLastValue){
    analogLastMoved = millis();
    analogLastValue = val;
  }
  
  // Check for time threshold
  if(millis() - analogLastMoved >= analogTimeThreshold){
    Serial.println(val);
    if(val == 0){
      switchState(STATE_IDLE);
      Serial.println("idle");
    } else{
      switchState(STATE_REPLAYING);
      Serial.println(selectedDev);
    }
  }
  
  // Display ring
  uint8_t nLeds = months.numPixels() / nDevs;
  uint8_t remaining = months.numPixels() - (nLeds * nDevs);

  for(int i=0; i<months.numPixels(); i++){
    if(i < remaining){
      months.setPixelColor(i, 0);
    } else{
      uint8_t dev = i / nLeds;
      if(dev == selectedDev && val != 0) {
        months.setPixelColor(i, illum(colors[dev], 255));
      } else{
        months.setPixelColor(i, colors[dev]);
      }
    }
  }
  months.show();
  
}

void setup() {
  Serial.begin(9600);
  uint32_t test = 6528100;
  Serial.println((test & (255)) );
   for (int i=0; i<4; i++){
    strips[i].begin();
    strips[i].setBrightness(BRIGHTNESS);
    strips[i].show();
    
    rings[i].begin();
    rings[i].setBrightness(BRIGHTNESS);
    rings[i].show();
  }
  months.begin();
  months.setBrightness(BRIGHTNESS);
  months.show();

  weeks.begin();
  weeks.setBrightness(BRIGHTNESS);
  weeks.show();
  analogLastValue = 1023 - analogRead(4);
  analogLastChecked = millis();
  Serial.println("all set up");
}

void loop() {
  checkSerial();
  checkAnalog();
  switch(currentState){
    case STATE_COMMIT:
      doCommit();
      break;
    case STATE_SELECTING:
      doSelect();
      break;
    default:
      break;
  }
  
}










