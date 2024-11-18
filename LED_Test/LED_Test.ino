#include <FastLED.h>           // Include FastLED library
#include <RTClib.h>            // Include other real time clock library
//#include <LiquidCrystal_I2C.h> // Inlucde the LCD library
#include <Wire.h>               // For I2C communication
#include <SPI.h>                // Include SPI library
#include <SD.h>                // Include the SD library
#define NUM_LEDS 8    // Number of LEDs in the chain
#define DATA_PIN 6    // Data pin for LED strip control
#define BUTTON_PIN 2  // data pin for the button press
#define LED_PIN 8    // data pin for the blinking led 

RTC_DS3231 rtc;
File myFile;
String filename; // idk what this does

CRGB leds[NUM_LEDS];  // Array to hold LED color data for the led strip

int l_delay = 2000 / 15; // the delay between the lights in the led strip
int loopCount = 0; // counts the number of times the loops 
int count_count = 0; // variable that loops over the elements in the array myVals
const int myVals[] = {22, 22, 45, 60, 60, 30, 150, 30, 30, 60, 90, 300, 22, 22, 45, 60, 60, 30, 150, 30, 30, 60, 90, 300}; // the number of loops before interuption
const int myValsLen = sizeof(myVals) / sizeof(myVals[0]); // gets the number of elements in myVals

// testing values
const int testVals[] = {3, 3, 10}; // the number of loops before integration for an example
int testValsLen = sizeof(testVals) / sizeof(testVals[0]); // number of elements in testVals

// consistent blinking light variables
int ledState = LOW;      // creating a variable for the blinking led
const long interval = 10;  // interval at which to blink (milliseconds)
unsigned long previousMillis_led = 0;  // will store last time LED was updated
unsigned long previousMillis_bln = 0;  // will store last time LED was updated
const long interval_led = 1000;  // interval at which to blink (milliseconds)
const long interval_bln = 160;
int i = 0;   // random variable for the initial animation

void setup() { // setup
  Serial.begin(9600);

  //sets up the fast led
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialize LEDs
  pinMode(LED_BUILTIN, OUTPUT);  // setting the led as an output
  pinMode(BUTTON_PIN, INPUT);  // setting up the button

// setting up the rtc
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      while (1) delay(10);
    }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  DateTime now = rtc.now(); // setting up the date and time
  
  Serial.println("Setup");
  
  // setting up the sd card initialization 
  if (!SD.begin(4)) {
    Serial.println("Initialization Failed");
    while(1);
  }
  Serial.println("Initialization done");
  
  // setting up the sd card and file
  filename = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + "_" + String(now.hour()) + "-" + String(now.minute()) + ".txt";
  myFile = SD.open(filename, FILE_WRITE); 
  if (myFile) {
  Serial.println("Writing");
  myFile.println("Testing");
  myFile.close();
  }

  // setting up the initial animation
  for (int a=0; a<3; a++) { // led strip will flash 3 times before starting the test
    fill_solid(leds, NUM_LEDS, CRGB::Blue); // flashing blue 
    FastLED.show();
    delay(l_delay*5);
    fill_solid(leds, NUM_LEDS, CRGB::Black); // turning off the strip
    FastLED.show();
    delay(l_delay*5);
    i++;
  }

  // printing the test starting time
  Serial.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Starting Test");
  myFile.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Starting Test");
}

void loop() {
  
  DateTime now = rtc.now(); // setting up the date and time
  unsigned long currentMillis = millis(); // setting up the constant blinking led 

  if (currentMillis - previousMillis_led >= interval_led) {
    // save the last time you blinked the LED
    previousMillis_led = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } 
    else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_PIN, ledState);
    i = i+1;
    String test1 = "Testing "+String(i);
    log_SD(test1);
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
      leds[dot] = CRGB::Green;   // Set the current LED to green
      FastLED.show();           // Update LEDs
      leds[dot] = CRGB::Black;  // Clear the current LED
      delay(l_delay);           // Wait for a short period before moving to the next LED
    }
    loopCount++;              // counts the amount of times that the led has cycled/looped
    
    //will flash a purple light when the loop counting is the same as the values of the array
    if (loopCount == testVals[count_count]) {
          fill_solid(leds, NUM_LEDS, CRGB::Purple);
          FastLED.show();
          delay(l_delay*5);
          fill_solid(leds, NUM_LEDS, CRGB::Black);
          count_count++; // increases the count of the loop
          loopCount = 0; // resets the loop count

          // time stamps the interuptions
          myFile.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Loop Skipped");
          Serial.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Loop Skipped");
    }

    // closes the sd card file when there is no more elements in the array
    if (count_count == testValsLen) { 
      // prints the time in which the test has ended
      myFile.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Test Finsished");
      Serial.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Test Finished");
      myFile.close(); // closes the sd file
      fill_solid(leds, NUM_LEDS, CRGB::White); FastLED.show(); // flashes solid white
    }
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
