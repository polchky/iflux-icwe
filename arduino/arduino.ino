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

ProjectModule modules[N_MODULES] = {
  ProjectModule(4, N_STRIP_LEDS, 5, N_RING_LEDS),
  ProjectModule(7, N_STRIP_LEDS, 6, N_RING_LEDS),
  ProjectModule(8, N_STRIP_LEDS, 9, N_RING_LEDS),
  ProjectModule(11, N_STRIP_LEDS, 10, N_RING_LEDS)
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
  Serial.begin(9600);
}

void loop() {
  // Update each module
  for (int i=0; i<N_MODULES; i++){
    modules[i].update();
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







