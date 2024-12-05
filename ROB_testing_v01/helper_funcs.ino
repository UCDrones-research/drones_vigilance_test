void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void syncRTC_wifi(){
  // Get the current date and time from an NTP server 
  unsigned long timeZoneOffsetHours = -8;
  unsigned long unixTime = timeClient.getEpochTime()+timeZoneOffsetHours*3600;
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);
}

void waiting_anim(unsigned long currentMillis){
  unsigned long interval_waiting_blink = 500;
  if (currentMillis - previousMillis_led >= interval_waiting_blink) {
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
    digitalWrite(LED_RIGHT_PIN, ledState);
    digitalWrite(LED_LEFT_PIN,!ledState);
  }
  if (currentMillis - previousMillis_bln >= interval_bln) { 
    previousMillis_bln = currentMillis;

    if (dir==true){ //move right to left
      dot = dot+1;
      if (dot==NUM_LEDS-1){
        if (loopCount == 3) {
          dot = 0; //reset back to 0, so we continue moving right to left
          loopCount = 0;
          Serial.println("Waiting to Start");
        } else {
          dir = false;
        }
      }
    } else { //move left to right
      dot = dot-1;
      if (dot==0){
        dir = true;
        loopCount = loopCount + 1; // completed a loop
      }
    }
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[dot] = CRGB::Red;  // Set the current LED to green
    FastLED.show(); 
  }
}

void blinking(unsigned long currentMillis){
  if (currentMillis - previousMillis_led >= interval_led) {
    // save the last time you blinked the LED
    previousMillis_led = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
      interval_led = 4000;
      if (blink_counter % blink_rate == 0 || 
          blink_counter % blink_rate == 1 || 
          blink_counter % blink_rate == 2){
        interval_led = 80;
      }
    } 
    else {
      ledState = LOW;
      interval_led = 200;
      if (blink_counter % blink_rate == 0 || 
          blink_counter % blink_rate == 1 || 
          blink_counter % blink_rate == 2){
        interval_led = 80;
      }
      
      blink_counter++;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_RIGHT_PIN, ledState);
    if (wink_counter % wink_rate != 0){
      digitalWrite(LED_LEFT_PIN, ledState);
      
    }
    wink_counter++;
  }
}

void log_SD(String message){ // function to write within the sd card and the serial port
  RTC.getTime(currentTime); 
  unsigned long currentMillis = millis(); 
  int h = currentTime.getHour();
  int m = currentTime.getMinutes();
  int s = currentTime.getSeconds();
  deltaMillis_cur = (currentMillis-deltaMillis_pre) % 1000;
  char linee[13];
  sprintf(linee, "%02i:%02i:%02i.%03i",h,m,s,deltaMillis_cur);
  
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    Serial.println(String(linee) + " -- " + message);
    myFile.println(String(linee) + " -- " + message);
    myFile.close();
  }
}

void startup_anim(){
  // setting up the initial animation
  for (int a=0; a<3; a++) { // led strip will flash 3 times before starting the test
    fill_solid(leds, NUM_LEDS, CRGB::Blue); // flashing blue 
    FastLED.show();
    digitalWrite(LED_RIGHT_PIN, HIGH);
    digitalWrite(LED_LEFT_PIN,HIGH);
    delay(interval_bln*5);
    
    fill_solid(leds, NUM_LEDS, CRGB::Black); // turning off the strip
    FastLED.show();
    digitalWrite(LED_RIGHT_PIN,LOW);
    digitalWrite(LED_LEFT_PIN,LOW);
    delay(interval_bln*5);
  }
}