
#include <FastLED.h>           // Include FastLED library
#include <SD.h>                // Include the SD library
#include <SPI.h>                // Include SPI library

// Include the RTC library
#include "RTC.h"

//Include the NTP library
#include <NTPClient.h>

#include <WiFiS3.h>

#include <WiFiUdp.h>
#include "arduino_secrets.h" 


#define BUTTON_PIN 2  // data pin for the button press
#define LED_LEFT_PIN 5  // Left Eye LED
#define DATA_PIN 6    // Data pin for LED strip control
#define LED_RIGHT_PIN 7 // Right Eye LED
#define SWITCH_PIN 10 //switch input

#define NUM_LEDS 8    // Number of LEDs in the chain

File myFile;
String filename; // global instance of filename

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);



unsigned long previousMillis_led = 0;
unsigned long interval_led = 1000;  // interval at which to blink (milliseconds)
int ledState = LOW;
int blink = LOW;
unsigned int blink_counter = 5;
int blink_rate = 13;
unsigned int wink_counter = 1;
int wink_rate = 26;


CRGB leds[NUM_LEDS];  // Array to hold LED color data for the led strip

unsigned long previousMillis_bln = 0;  // will store last time LED was updated
unsigned long interval_bln = 2000 / 15;

int l_delay = 2000 / 15; // the delay between the lights in the led strip
int loopCount = 0; // counts the number of times the loops 
int count_count = 0; // variable that loops over the elements in the array myVals
bool dir = true;
int dot = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 20;
int buttonState;
int lastButtonState = HIGH;

// array that contains the actual vals used for the test. 
//const int myVals[] = {22, 44, 89, 149, 209, 239, 389, 419, 449, 509, 599, 899, 921, 943, 988, 1048, 1108, 1138, 1288, 1318, 1348, 1408, 1498, 1798}; // the number of loops before interuption
const int myVals[] = {3, 6, 10, 15, 25}; // the number of loops before integration for an example
int myValsLen = sizeof(myVals) / sizeof(myVals[0]); // gets the number of elements in myVals

unsigned long deltaMillis_pre = 0;
unsigned long deltaMillis_cur = 0;


RTCTime currentTime;

bool start = false;

void setup(){
  pinMode(LED_RIGHT_PIN, OUTPUT);  // setting the led as an output
  pinMode(LED_LEFT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);  // setting up the button
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  digitalWrite(LED_RIGHT_PIN,HIGH);
  digitalWrite(LED_LEFT_PIN,HIGH);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialize LEDs
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show(); 
  

  Serial.begin(57600);
  while (!Serial);
  Serial.println("Initializing");
  connectToWiFi();
  RTC.begin();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  syncRTC_wifi();

  // Retrieve the date and time from the RTC and print them
  
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));

  // setting up the sd card initialization 
  if (!SD.begin(4)) {
    Serial.println("SD Card Initialization Failed");
    while(1);
  }
  Serial.println("SD Card Initialization done");

  // Print time (HH/MM/SS)
  RTC.getTime(currentTime); 
  int d = currentTime.getDayOfMonth();
  int h = currentTime.getHour();
  int m = currentTime.getMinutes();
  int s = currentTime.getSeconds();
  char linee[8];
  sprintf(linee, "t%02i%02i%02i",d,h,m);


  filename = String(linee) + ".txt";
  Serial.println("Creating: " + filename);
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile){
    Serial.println("Success");
    myFile.println("Experiment Logger");
    myFile.println(String(currentTime));
    myFile.println("-------------------");
    myFile.close();
  } else {
    Serial.println("Didn't Work");
  }


  RTCTime alarmTime;
  alarmTime.setSecond(0);

  // Make sure to only match on the seconds in this example - not on any other parts of the date/time
  AlarmMatch matchTime;
  matchTime.addMatchSecond();

  //sets the alarm callback
  RTC.setAlarmCallback(alarmCallback, alarmTime, matchTime);

  startup_anim();
  count_count = 0;
  loopCount = 0;
  dot = 0;
  Serial.println("Waiting to Start");

  while(!start){
    

    unsigned long currentMillis = millis(); 
    waiting_anim(currentMillis);

    int switch_test = digitalRead(SWITCH_PIN);

    if (switch_test==LOW){
      
      delay(80);
      log_SD("Starting Experiment");
      count_count = 0;
      loopCount = 0;
      dot = 0;
      dir = true;
      start = true;
    }
  }

}

void loop(){
  // setting up the constant blinking led 
  unsigned long currentMillis = millis(); 

  blinking(currentMillis);
  
  if (digitalRead(SWITCH_PIN) == HIGH) {  // if button is pressed
     NVIC_SystemReset(); // hard reset of the system
  }

  int newbutton = digitalRead(BUTTON_PIN);
  if (newbutton != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis()-lastDebounceTime) > debounceDelay){
    if (newbutton !=buttonState) {
      buttonState = newbutton;
      if (buttonState == LOW){
        log_SD("Sync");
      }
    }
  }
  lastButtonState = newbutton;


  // iterates over the lights in the led strip
  if (currentMillis - previousMillis_bln >= interval_bln) { 
    previousMillis_bln = currentMillis;
    

    if (dir==true){ //move right to left
      dot = dot+1;
      if (dot==NUM_LEDS-1){
        if (loopCount == myVals[count_count])
        {
          dot = 0; //reset back to 0, so we continue moving right to left
          count_count = count_count + 1; //increment which test we're on
          if (count_count == myValsLen){ // Reset count_count and loopCount to keep going indef
            count_count = 0;
            loopCount = 0;
            log_SD("Reset");
          }         
          log_SD("Skip");

        } else {
        dir = false;
        }
      }
    } else { //move left to right
      dot = dot-1;
      if (dot==0){
        dir = true;
        loopCount = loopCount + 1; // completed a loop
        log_SD("Loop");
      }
    }
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[dot] = CRGB::Green;  // Set the current LED to green
    FastLED.show(); 
  }
}

void alarmCallback(){
  deltaMillis_pre = millis();
}

