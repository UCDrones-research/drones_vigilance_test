#include <FastLED.h>           // Include FastLED library
#include <RTClib.h>            // Include other real time clock library
//#include <LiquidCrystal_I2C.h> // Inlucde the LCD library
#include <Wire.h>               // For I2C communication
#include <SPI.h>                // Include SPI library
#include <SD.h>                // Include the SD library
#define NUM_LEDS 8    // Number of LEDs in the chain
#define DATA_PIN 6    // Data pin for LED strip control
#define BUTTON_PIN 2  // data pin for the button press
#define LED_PIN 7    // data pin for the blinking led 

RTC_DS3231 rtc;
File myFile;
String filename; // global instance of filename

CRGB leds[NUM_LEDS];  // Array to hold LED color data for the led strip

int l_delay = 2000 / 15; // the delay between the lights in the led strip
int loopCount = 0; // counts the number of times the loops 
int count_count = 0; // variable that loops over the elements in the array myVals
bool dir = true;
int dot = 0;

// array that contains the actual vals used for the test. 
const int myVals[] = {22, 44, 89, 149, 209, 239, 389, 419, 449, 509, 599, 899, 921, 943, 988, 1048, 1108, 1138, 1288, 1318, 1348, 1408, 1498, 1798}; // the number of loops before interuption
int myValsLen = sizeof(myVals) / sizeof(myVals[0]); // gets the number of elements in myVals

// half test
const int halfVals[] = {22, 44, 89, 149, 209, 239, 389, 419, 449, 509, 599, 899}; // the number of loops before interuption
int halfValsLen = sizeof(halfVals) / sizeof(halfVals[0]); // gets the number of elements in myVals

// testing values
const int testVals[] = {3, 6, 10, 15, 25}; // the number of loops before integration for an example
int testValsLen = sizeof(testVals) / sizeof(testVals[0]); // number of elements in testVals

// consistent blinking light variables
int ledState = LOW;      // creating a variable for the blinking led
unsigned long previousMillis_led = 0;  // will store last time LED was updated
unsigned long previousMillis_bln = 0;  // will store last time LED was updated
const long interval_led = 1000;  // interval at which to blink (milliseconds)
const long interval_bln = 160;
int i = 0;   // random variable for the initial animation

// String TestSubject = "kyr"; //Not to exceed 3 char

void setup() { // setup
  Serial.begin(9600);
  Serial.println("Setup");

  //sets up the fast led
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialize LEDs
  pinMode(LED_PIN, OUTPUT);  // setting the led as an output
  pinMode(BUTTON_PIN, INPUT);  // setting up the button

  digitalWrite(LED_PIN,LOW);
  
  // setting up the sd card initialization 
  if (!SD.begin(4)) {
    Serial.println("SD Card Initialization Failed");
    while(1);
  }
  Serial.println("SD Card Initialization done");
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  } else {
    Serial.println("RTC Init Complete");
  }

  DateTime now = rtc.now(); // setting up the date and time
  
  filename = String(now.month()) + String(now.day()) + String(now.hour()) + String(now.minute()) + ".txt";
  
  Serial.println(filename);
  //filename = "exmp12.txt";
  myFile = SD.open(filename, FILE_WRITE); 
  if (myFile) {
  Serial.println("Writing");
  myFile.println("Testing");
  myFile.close();
  } else {
    Serial.println("Didn't work");
  }

  // setting up the initial animation
  for (int a=0; a<3; a++) { // led strip will flash 3 times before starting the test
    fill_solid(leds, NUM_LEDS, CRGB::Blue); // flashing blue 
    FastLED.show();
    digitalWrite(LED_PIN, HIGH);
    delay(l_delay*5);
    
    fill_solid(leds, NUM_LEDS, CRGB::Black); // turning off the strip
    FastLED.show();
    digitalWrite(LED_PIN,LOW);
    delay(l_delay*5);
    
    i++;
  }
  i = 0;
  count_count = 0;
  loopCount = 0;

  String SD_L = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Starting Test";
  log_SD(SD_L);
}

void loop() {
  DateTime now = rtc.now(); // setting up the date and time
  
  // setting up the constant blinking led 
  unsigned long currentMillis = millis(); 

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
  }

  //setting up the button press example
  if (digitalRead(BUTTON_PIN) == LOW) {  // if button is pressed
    /*
    fill_solid(leds, NUM_LEDS, CRGB::Purple);
    FastLED.show();
    loopCount = 0; //Restarting the loop count
    count_count = 0;
    dot = 0;
    dir = true;
    */
    NVIC_SystemReset(); // hard reset of the system
  }

  else { //if the button isn't pressed
    
    // iterates over the lights in the led strip
    if (currentMillis - previousMillis_bln >= interval_bln) { 
    previousMillis_bln = currentMillis;

    if (dot==0){
      if (loopCount==0){
        if (count_count==0){
          DateTime now = rtc.now(); // setting up the date and time
          String resetN = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "-" + String(currentMillis) + " -- Reset";
          log_SD(resetN);
        } 
      }
    }

    if (dir==true){ //move right to left
      dot = dot+1;
      if (dot==NUM_LEDS-1){
        if (loopCount == halfVals[count_count])
        {
          dot = 0; //reset back to 0, so we continue moving right to left
          count_count = count_count + 1; //increment which test we're on
          if (count_count == myValsLen){ // Reset count_count and loopCount to keep going indef
            count_count = 0;
            loopCount = 0;
            //DateTime now = rtc.now(); // setting up the date and time
            //unsigned long nMillis = millis();
            //String resetN = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "-" + String(nMillis) + " -- Reset";
            //log_SD(resetN);
          }
          DateTime now = rtc.now(); // setting up the date and time
          unsigned long nMillis = millis();
          String skipN = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "-" + String(nMillis) + " -- Skip";
          log_SD(skipN);
        } else {
        dir = false;
        }
      }
    } else { //move left to right
      dot = dot-1;
      if (dot==0){
        dir = true;
        loopCount = loopCount + 1; // completed a loop
        
        String loop_out1 = "Loop: " + String(loopCount) + "/" + String(myVals[count_count]);
        log_SD(loop_out1);
      }
    }
    FastLED.clear();
    leds[dot] = CRGB::Green;  // Set the current LED to green
    FastLED.show(); 
  }

  /*
    //will flash a purple light when the loop counting is the same as the values of the array
    if (loopCount == halfVals[count_count]) {
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
    if (count_count == halfValsLen) { 
      // prints the time in which the test has ended
      myFile.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Test Finsished");
      Serial.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + "--" + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + " -- Test Finished");
      myFile.close(); // closes the sd file
      fill_solid(leds, NUM_LEDS, CRGB::White); FastLED.show(); // flashes solid white
    }
    */
  }
}

void log_SD(String message){ // function to write within the sd card and the serial port
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    Serial.println(message);
    myFile.println(message);
    myFile.close();
  }
}
