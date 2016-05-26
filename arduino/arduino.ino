#include <Adafruit_NeoPixel.h>

#define NSTRIPS 10
#define NPIXELS 16

const char ORDER_DELIMITER = '/';
const String ORDER_SETUP = "s";
const String ORDER_DEV = "d";
const String ORDER_RESET = "r";
const String ORDER_ADD = "a";

const String STATE_IDLE = "IDLE";
const String STATE_COMMIT = "COMMIT";
const String STATE_SELECTING = "SELECTING";
const String STATE_REPLAYING = "REPLAYING";
const String STATE_RECEIVING = "RECEIVING";

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strips[NSTRIPS] = {
  Adafruit_NeoPixel(NPIXELS, 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 7, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 8, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 9, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 10, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 11, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 12, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NPIXELS, 13, NEO_GRB + NEO_KHZ800)
};

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
  Serial.begin(9600);
  for(uint16_t p=0; p<NSTRIPS; p++){
    strips[p].begin();
    strips[p].setBrightness(10);
    strips[p].show();
  }
}

void loop() {
  uint32_t green = strips[0].Color(10,150,0);
  uint32_t black = strips[0].Color(0,0,0);
  uint32_t color;
  for(uint16_t p=0; p<NSTRIPS; p++) {
    for(uint16_t q=0; q<NPIXELS; q++){
      if(q<=p){
        color = green;
      } else{
        color = black;
      }
      strips[p].setPixelColor(q, color);
    }
    strips[p].show();
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
  switch(orderType){
    case ORDER_SETUP:
      
      break;
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
  }
  if(checkInputString() == true){
    executeOrder();
    Serial.println("ok!");
    //Serial.println(getInputStringNextPart());
    //Serial.println(inputString);
  } else {
    announceFaultyOrder();
  }
}







