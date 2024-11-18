#include <SPI.h>
#include <SD.h>
#include <FastLED.h>           // Include FastLED library

#define LED_PIN 8    // data pin for the blinking led 
#define NUM_LEDS 8    // Number of LEDs in the chain
#define DATA_PIN 6    // Data pin for LED strip control

File myFile;
String filename;

int i = 0;
int ledState = LOW;  // ledState used to set the LED
unsigned long previousMillis_led = 0;  // will store last time LED was updated
unsigned long previousMillis_bln = 0;  // will store last time LED was updated
const long interval_led = 1000;  // interval at which to blink (milliseconds)
const long interval_bln = 160;


CRGB leds[NUM_LEDS];  // Array to hold LED color data for the led strip
int l_delay = 2000 / 15; // the delay between the lights in the led strip
int loopCount = 0; // counts the number of times the loops 
int count_count = 0; // variable that loops over the elements in the array myVals
bool dir = true;
int dot = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial){
    ;
  }

  Serial.println("Initializing");
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialize LEDs

  for (int a=0; a<3; a++) { // flashes the led strip 3 times blue before starting the test
      fill_solid(leds, NUM_LEDS, CRGB::Blue); // flashing blue 
      FastLED.show();
      delay(1000);
      fill_solid(leds, NUM_LEDS, CRGB::Black); // turning off the strip
      FastLED.show();
      delay(1000);
  }

  if (!SD.begin(4)){
    Serial.println("init failed");
    while(1);
  }
  Serial.println("Init Succeed");

  filename = "Time.txt";
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    Serial.println("Writing");
    myFile.println("Testing");
    myFile.close();
  }
}


void loop() {
  
  // setting up the constantly blinking light
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis_led >= interval_led) {
    // save the last time you blinked the LED
    previousMillis_led = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_PIN, ledState);
    i = i+1;
    String test1 = "Testing "+String(i);
    log_SD(test1);
  }

  if (currentMillis - previousMillis_bln >= interval_bln) {
    previousMillis_bln = currentMillis;

    if (dir==true){
      dot = dot+1;
      if (dot==NUM_LEDS-1){
        dir = false;
      }
    } else {
      dot = dot-1;
      if (dot==0){
        dir = true;
      }
    }
    FastLED.clear();
    leds[dot] = CRGB::Green;  // Set the current LED to green
    FastLED.show(); 
  }

}

void log_SD(String message){
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    Serial.println(message);
    myFile.println(message);
    myFile.close();
  }
}
