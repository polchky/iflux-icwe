#include <Adafruit_NeoPixel.h>
#include <ProjectModule.h>


const int N_MODULES = 4;
const int N_STRIP_LEDS = 11;
const int N_RING_LEDS = 24;

const char ORDER_DELIMITER = '/';
const String ORDER_SETUP = "s";
const String ORDER_DEV = "d";
const String ORDER_RESET = "r";
const String ORDER_ADD = "a";
const String ORDER_CHECK = "c";

const String STATE_IDLE = "IDLE";
const String STATE_COMMIT = "COMMIT";
const String STATE_SELECTING = "SELECTING";
const String STATE_REPLAYING = "REPLAYING";
const String STATE_RECEIVING = "RECEIVING";

uint32_t colors[7] = {
  16711680,
  65280,
  255,
  16777215,
  65535,
  16776960,
  16711935
};

Adafruit_NeoPixel pixels[10] = {
  Adafruit_NeoPixel(N_STRIP_LEDS, 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_RING_LEDS, 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_STRIP_LEDS, 7, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_RING_LEDS, 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_STRIP_LEDS, 8, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_RING_LEDS, 9, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_STRIP_LEDS, 11, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_RING_LEDS, 10, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(N_RING_LEDS, 12, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(16, 13, NEO_GRB + NEO_KHZ800)
};

ProjectModule modules[N_MODULES] = {
  ProjectModule(pixels[0], pixels[1], 6),
  ProjectModule(pixels[2], pixels[3], 6),
  ProjectModule(pixels[4], pixels[5], 6),
  ProjectModule(pixels[6], pixels[7], 6)
};
uint32_t myIndex = 0;

String inputString = "";
String currentState = STATE_IDLE;

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

void setup() {
  uint32_t c[24] = {
    colors[0],
    colors[1],
    colors[2],
    colors[3],
    colors[4],
    colors[5],
    colors[6],
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };
  Serial.begin(9600);
  for (int i=0; i<4; i++){
    modules[i].init();
    modules[i].setBrightness(20, 20);
    modules[i].setRingDisplay(c);
  }
  //modules[0].setRingDisplay(c);
  
}

void loop() {
  // Update each module
  for (int i=0; i<N_MODULES; i++){
    //modules[i].update();
  }

}

/**
 * Checks the integrity of the last received message
 */
boolean checkInputString(){
  if(inputString.charAt(0) != 'l') {
    return false;
  }
  int originalLength = getInputStringNextPart().substring(1).toInt();
  return originalLength == inputString.length();
}

void executeOrder(){
  String orderType = getInputStringNextPart();
  switch(1){
    default: 
      announceFaultyOrder();
      break;
  }
}

/**
 * Called each time something is sent through the serial
 */
void serialEvent() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');
    inputString.remove(inputString.length() - 1, 1);
  }
  if(checkInputString() == true){
    executeOrder();
    Serial.println("ok!");
  } else {
    announceFaultyOrder();
  }
}







