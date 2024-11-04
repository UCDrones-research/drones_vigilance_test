#include <FastLED.h>           // Include FastLED library
#include <AnalogRTCLibrary.h>  // Include the real time clock library
#include <RTClib.h>            // Include other real time clock library
//#include <LiquidCrystal_I2C.h> // Inlucde the LCD library
#include <Wire.h>  // For I2C communication
#include <SPI.h>   // Include SPI library
#include <SD.h>                // Include the SD library
#include <DS3231.h>
#include <SD.h>

#define NUM_LEDS 8    // Number of LEDs in the chain
#define DATA_PIN 6    // Data pin for LED strip control
#define BUTTON_PIN 2  // data pin for the button press
#define LED_PIN 13    // data pin for the blinking led 

File myFile;
RTC_DS3231 rtc;

CRGB leds[NUM_LEDS];  // Array to hold LED color data for the led strip

int l_delay = 2000 / 15; // the delay between the lights in the led strip
int loopCount = 0; // counts the number of times the loops 
int count_count = 0; // variable that loops over the elements in the array myVals
const int myVals[] = {22, 22, 45, 60, 60, 30, 150, 30, 30, 60, 90, 300, 22, 22, 45, 60, 60, 30, 150, 30, 30, 60, 90, 300}; // the number of loops before interuption
const int myValsLen = sizeof(myVals) / sizeof(myVals[0]); // gets the number of elements in myVals
const int testVals[] = {3, 3, 10, 14, 27}; // the number of loops before integration for an example
const int testValsLen = sizeof(testVals) / sizeof(testVals[0]);

// consistent blinking light variables
int LED_state = LOW;      // creating a variable for the blinking led
const long interval = 10;  // interval at which to blink (milliseconds)
unsigned long previousMillis = 0;  // will store last time LED was updated
long OnTime = 250;           // milliseconds of on-time
long OffTime = 750;          // milliseconds of off-time

void setup() { // setup
  Serial.begin(9600);

  //sets up the fast led
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialize LEDs

  pinMode(LED_BUILTIN, OUTPUT);  // setting the led as an output

  pinMode(BUTTON_PIN, INPUT);  // setting up the button
}

void loop() {

  // setting up the constant blinking led 
  unsigned long currentMillis = millis();
     
  if((LED_state == HIGH) && (currentMillis - previousMillis >= OnTime))
  {
    LED_state = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    digitalWrite(LED_BUILTIN, LED_state);  // Update the actual LED
  }
  else if ((LED_state == LOW) && (currentMillis - previousMillis >= OffTime))
  {
    LED_state = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    digitalWrite(LED_BUILTIN, LED_state);	  // Update the actual LED
  }

  //setting up the button press example
  if (digitalRead(BUTTON_PIN) == LOW) {  // if button is pressed
    fill_solid(leds, NUM_LEDS, CRGB::Purple);
    FastLED.show();
    loopCount = 0; //Restarting the loop count
  }

  else { //if the button isn't pressed
    for (int dot = 0; dot < NUM_LEDS-1; dot++) { // loops through each led in the led strip
      leds[dot] = CRGB::Green;  // Set the current LED to green
      FastLED.show();           // Update LEDs
      leds[dot] = CRGB::Black;  // Clear the current LED
      delay(l_delay);           // Wait for a short period before moving to the next LED
    }
    for (int dot = (NUM_LEDS - 1); dot >= 0; dot--) {
      leds[dot] = CRGB::Blue;   // Set the current LED to blue
      FastLED.show();           // Update LEDs
      leds[dot] = CRGB::Black;  // Clear the current LED
      delay(l_delay);           // Wait for a short period before moving to the next LED
    }
    loopCount++;              // counts the amount of times that the led has cycled/looped
    
    //will flash a purple light when the loop counting is the same as the values of the array
    if (loopCount == myVals[count_count]) {
          fill_solid(leds, NUM_LEDS, CRGB::Purple);
          FastLED.show();
          delay(1000); // delay for a full second
          fill_solid(leds, NUM_LEDS, CRGB::Black);


          count_count++; // increases the count of the loop
          loopCount = 0; // resets the loop count
    }
  }
}
