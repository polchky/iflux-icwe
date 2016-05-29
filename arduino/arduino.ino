#include <Adafruit_NeoPixel.h>

const int N_STRIP_LEDS = 11;

const char ORDER_DELIMITER = '/';
const char ORDER_RING = 'r'; // Used
const char ORDER_SETUP = 's';
const char ORDER_DEV = 'd'; // used
const char ORDER_RESET = 'x';
const char ORDER_ADD = 'a';
const char ORDER_COMMIT = 'c'; // used

const int STATE_IDLE = 0; // Used
const int STATE_COMMIT = 1; // used
const int STATE_SELECTING = 2;
const int STATE_REPLAYING = 3;
const int STATE_RECEIVING = 4;

int BRIGHTNESS = 20;

int nDevs = 0;
int currentState = STATE_IDLE;
String inputString = "";
unsigned long timer = 0;
int dStep = 100;

// Commit variables
int commitDev;
int nCommits;
int commitModule;

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

Adafruit_NeoPixel weeks = Adafruit_NeoPixel(16, 0, false, 13, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel months = Adafruit_NeoPixel(24, 0, false, 12, NEO_GRB + NEO_KHZ800);



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
  if(currentState == STATE_COMMIT){
    clearStrips();
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
  timer = 0;
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

void setup() {
  Serial.begin(9600);
  
   for (int i=0; i<4; i++){
    strips[i].begin();
    strips[i].setBrightness(BRIGHTNESS);
    strips[i].show();
    
    rings[i].begin();
    rings[i].setBrightness(BRIGHTNESS);
    rings[i].show();
  }
  Serial.println("all set up");
  strips[1].setPixelColor(1, 255);strips[1].show();
}

void loop() {
  checkSerial();
  switch(currentState){
    case STATE_COMMIT:
      doCommit();
      break;
    default:
      break;
  }
  
}










