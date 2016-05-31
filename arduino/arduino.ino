#include <Adafruit_NeoPixel.h>

const int N_STRIP_LEDS = 11;

const char ORDER_DELIMITER = '/';
const char ORDER_RING = 'r'; // used
const char ORDER_SETUP = 's';
const char ORDER_DEV = 'd'; // used
const char ORDER_RESET = 'x';
const char ORDER_ADD = 'a';
const char ORDER_COMMIT = 'c'; // used
const char ORDER_COMMITS = 't'; // used
const char ORDER_FINISHED = 'f'; // used

const uint8_t STATE_IDLE = 0; // used
const uint8_t STATE_COMMIT = 1; // used
const uint8_t STATE_SELECTING = 2; // used
const uint8_t STATE_REPLAYING = 3; // used
const uint8_t STATE_RECEIVING = 4; // used

int BRIGHTNESS = 100;
int DEFAULT_BRIGHTNESS = 100;

typedef struct
{
  uint8_t ringIndex: 5;
  uint8_t ringLength: 5;
  uint8_t strength: 4;
} Commit;

Commit commits[48][4];
Commit singleCommit;
uint8_t singleCommitDev;
uint8_t singleCommitModule;

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

uint8_t nDevs = 7;
uint8_t ledsPerDev = months.numPixels() / nDevs;
uint8_t remaining = months.numPixels() - (ledsPerDev * nDevs);
uint8_t selectedDev;
  
uint8_t currentState = STATE_IDLE;
String inputString = "";
unsigned long stateStart;
uint8_t DELTA_T = 50;
uint8_t N_COMMIT_REPS = 5;
uint8_t N_STEPS_RING = 40;
unsigned long commitMaxTime;
uint32_t nStepsPerRep;

// Commits replay variables
uint8_t timeIndex;



uint32_t colors[8] = {
  100,
  25600,
  6553600,
  25700,
  6553700,
  6579200,
  6579300,
  0
};

// Dev selection
uint16_t analogLastValue;
unsigned long analogLastMoved;
uint8_t ANALOG_THRESHOLD = 10;
unsigned long ANALOG_TIME_THRESHOLD = 1000;


uint32_t illum(uint32_t color, uint8_t newBrightness, uint8_t oldBrightness = DEFAULT_BRIGHTNESS ){
  return 
    ((((color & (255 << 16)) >> 16) / oldBrightness * newBrightness) << 16) +
    ((((color & (255 << 8)) >> 8) / oldBrightness * newBrightness) << 8) +
    ((color & (255)) / oldBrightness * newBrightness) ;
}

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
    for(uint8_t i=singleCommit.ringIndex; i < singleCommit.ringIndex + singleCommit.ringLength; i++){
      rings[singleCommitModule].setPixelColor(i, colors[singleCommitDev]);
    }
    rings[singleCommitModule].show();
  }
  switch (newState){
    case STATE_COMMIT:
      clearStrips();
      clearRing("months");
      clearRing("weeks");
      receiveCommit();
      break;
    case STATE_SELECTING:
      clearStrips();
      clearRing("weeks");
      clearRings();
      analogLastMoved = millis();
      break;
    case STATE_RECEIVING:
      for(uint8_t i=0; i<4; i++){
        for(uint8_t j=0; j<48; j++){
        }
      }
      break;
    case STATE_REPLAYING:
      clearRing("months");
      break;
    case STATE_IDLE:
      clearStrips();
      clearRing("weeks");
      clearRing("months");
      break;
    default: break;
  }
  stateStart = millis();
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
    case ORDER_COMMITS:
      storeCommits();
      break;
    case ORDER_FINISHED:
      switchState(STATE_REPLAYING);
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

void clearRing(String ringName){
  if(ringName.equals("months")){
    for (int i=0; i<months.numPixels(); i++){
      months.setPixelColor(i, 0);
    }
    months.show();
  } else {
    for (int i=0; i<weeks.numPixels(); i++){
      weeks.setPixelColor(i, 0);
    }
    weeks.show();
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

void receiveCommit(){
    String nextCommit = getInputStringNextPart();
    singleCommitDev = nextCommit.substring(0,1).toInt();
    singleCommitModule = nextCommit.substring(1,2).toInt();
    singleCommit.ringIndex = nextCommit.substring(2,4).toInt();
    singleCommit.ringLength = nextCommit.substring(4,6).toInt();
    singleCommit.strength = nextCommit.substring(6,7).toInt() * 2;

    // Set commit max time
    nStepsPerRep = 14 + singleCommit.strength * 8 + N_STEPS_RING;
    commitMaxTime = millis() + DELTA_T * N_COMMIT_REPS * nStepsPerRep;
}

void doCommit(){
  // Check time
  if(millis() > commitMaxTime){
    switchState(STATE_IDLE);
    return;
  }

  int cStep = (millis() - stateStart) / DELTA_T;
  uint8_t localStep = cStep % nStepsPerRep;
  
  // blink dev on ring
  if(localStep > 14 + singleCommit.strength * 8){
    uint16_t illumFactor = (cStep % 10) * 30 + 130;
    if(illumFactor > 240){
      illumFactor = 490 - illumFactor;
    }
    for(int i=singleCommit.ringIndex; i < singleCommit.ringIndex + singleCommit.ringLength; i++){
      rings[singleCommitModule].setPixelColor(i, illum(colors[singleCommitDev], illumFactor));
    }
    rings[singleCommitModule].show();
  } 
  // show strip
  else{
    for(uint8_t i=0; i<11; i++){
      if(true){
        if((localStep - i % 8) / 3 == 0){
          strips[singleCommitModule].setPixelColor(i, colors[singleCommitDev]);
        } else {
          strips[singleCommitModule].setPixelColor(i, 0);
        }
      } else{
        strips[singleCommitModule].setPixelColor(i, 0);
      }
    }
    strips[singleCommitModule].show();
  }
  /*
  uint16_t illumFactor = (millis() - stateStart + 510) / 2 % 300;
  if(illumFactor > 150){
    illumFactor = 300 - illumFactor;
  }
  illumFactor += 100;
  for(int i=singleCommit.ringIndex; i < singleCommit.ringIndex + singleCommit.ringLength; i++){
    rings[singleCommitModule].setPixelColor(i, illum(colors[singleCommitDev], illumFactor));
  }
  rings[singleCommitModule].show();
*/
  // show strip
  /*
  int cStep = (millis() - stateStart) / deltaT;
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
  */
  
}




void checkAnalog(){
  if(currentState == STATE_SELECTING){
    return;
  }
  uint16_t val = 1023 - analogRead(4);
  if(val + ANALOG_THRESHOLD < analogLastValue ||  (val >= ANALOG_THRESHOLD) && val - ANALOG_THRESHOLD > analogLastValue){
    switchState(STATE_SELECTING);
    analogLastValue = val;
  }
}

void doSelect(){
  // Read and store value
  uint16_t val = 1023 - analogRead(4);
  selectedDev = val * nDevs / 1024;
  
  if(val + ANALOG_THRESHOLD < analogLastValue ||  (val >= ANALOG_THRESHOLD) && val - ANALOG_THRESHOLD > analogLastValue){
    //Serial.println(val); Serial.println(analogLastValue);Serial.println("");
    analogLastMoved = millis();
    analogLastValue = val;
  }
  
  // Display ring

  for(int i=0; i<months.numPixels(); i++){
    if(i < remaining){
      months.setPixelColor(i, 0);
    } else{
      uint8_t dev = (i - remaining) / ledsPerDev;
      if(dev == selectedDev && val != 0) {
        months.setPixelColor(i, illum(colors[dev], 255));
      } else{
        months.setPixelColor(i, illum(colors[dev], 50));
      }
    }
  }
  months.show();
  
  // Check for time threshold
  if(millis() - analogLastMoved >= ANALOG_TIME_THRESHOLD){
    if(val == 0){
      switchState(STATE_IDLE);
      //Serial.println("lol");
    } else{
      switchState(STATE_RECEIVING);
      //Serial.println("relol");
    }
  }
  
}

void doReceive(){
  uint16_t illumFactor = (millis() - stateStart + 510) / 2 % 500;
  if(illumFactor > 250){
    illumFactor = 500 - illumFactor;
  }
  for (uint8_t i=selectedDev * ledsPerDev + remaining; i<(selectedDev + 1) * ledsPerDev + remaining; i++){
    months.setPixelColor(i, illum(colors[selectedDev], illumFactor));
  }
  months.show();
}

void storeCommits(){
  Serial.println("ok");
  do{
    String nextCommit = getInputStringNextPart();
    uint8_t module = nextCommit.substring(0,1).toInt();
    uint8_t week = nextCommit.substring(1,3).toInt();
    uint8_t ringIndex = nextCommit.substring(3,5).toInt();
    uint8_t ringLength = nextCommit.substring(5,7).toInt();
    uint8_t commitStrength = nextCommit.substring(7,8).toInt() * 2;
    commits[week][module].ringIndex = ringIndex;
    commits[week][module].ringLength = ringLength;
    commits[week][module].strength = commitStrength;
  } while(inputString.length() > 0);
}

void doReplay(){
  
}

void setup() {
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println("all set up");
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
    case STATE_RECEIVING:
      doReceive();
      break;
    case STATE_REPLAYING:
      doReplay();
      break;
    default:
      break;
  }
  
}










