void debugSetup(void) {
  STAILQ_INIT(&gLogQueue);
  gWeblogCount = 0;
}

void togglePin(int pin,int pinVal)
{
  if (pinVal == LOW)
    pinVal = HIGH;
  else
    pin  = LOW;

  digitalWrite(pin,pinVal);
  
}


void toggleOnboard(void)
{
  if (gOnboardLedState == LOW)
    gOnboardLedState = HIGH;
  else
    gOnboardLedState  = LOW;

  digitalWrite(LED_CARTE,gOnboardLedState);
  
}
/********************************** PIN SETUP   ****************************************/
void pinSetup(void) {
  // set pin2 to output mode 
 
  pinMode(D2, OUTPUT);     
   
  // onboard led ON while starting
  pinMode(LED_CARTE, OUTPUT);
  digitalWrite(LED_CARTE,LOW);
}

void ntpSetup(void)
{
  /******************************* NTP setup **************************************/
  // NTP and time setup.
  // this one sets the NTP server as gConftimeServer with gmt+1
  // and summer/winter mechanic (true)
  NTP.begin(gConftimeServer, 1, true);
  // set the interval to 20s at first. On success, change that value 
  NTP.setInterval(20);

  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
      gNtpEvent = event;
      gSyncEventTriggered = true;
      });
}

void webServerSetup(void)
{
  gWebServer.on("/", handleRoot);
  gWebServer.on("/command", handleCommmand);
  gWebServer.on("/log", handleLog);
  gWebServer.onNotFound(handleNotFound);

  gHttpUpdater.setup(&gWebServer);
  
  gWebServer.begin();
} 
 
void mqttSetup(void)
{  
  DEBUG_PRINTLN("Wifi connected, reaching for MQTT server")
  gMqttWaitingCounter = MQTT_WAIT_RECONNECT_COUNTER;
  gMqttClient.setServer(gConfMqttServerIp, gConfMqttPort);
  gMqttClient.setCallback(airconCommandCallback);
}

void setAircon(void)
{
    daikinAircon.setTemp(gAircon.temperature);
    daikinAircon.setFan(gAircon.fan);
    daikinAircon.setPower(gAircon.power);
    daikinAircon.setPowerful(gAircon.powerful);
    daikinAircon.setQuiet(gAircon.quiet);
    daikinAircon.setSwingVertical(gAircon.swingVertical);
    daikinAircon.setMode(gAircon.programMode);   
    daikinAircon.send();
}


void OtaSetup(void)
{
  ArduinoOTA.setPort(OTAPORT);
  ArduinoOTA.setHostname(DEVICENAME);

  // authentication by default
  ArduinoOTA.setPassword(OTAPASSWORD);

  ArduinoOTA.onStart([]() {
    DEBUG_PRINTLN("Starting OTA")
    gOtaUpdating = true;
  });
  
  ArduinoOTA.onEnd([]() {
    DEBUG_PRINTLN("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_PRINTLN(progress)
  });
 
  ArduinoOTA.onError([](ota_error_t error) {
  DEBUG_PRINT("Error ")
  DEBUG_PRINTLN(error)
  
  if (error == OTA_AUTH_ERROR) 
    DEBUG_PRINTLN("Auth Failed")
  else if (error == OTA_BEGIN_ERROR) 
    DEBUG_PRINTLN("Begin Failed")
  else if (error == OTA_CONNECT_ERROR) 
    DEBUG_PRINTLN("Connect Failed")
  else if (error == OTA_RECEIVE_ERROR)
    DEBUG_PRINTLN("Receive Failed")
  else if (error == OTA_END_ERROR) 
    DEBUG_PRINTLN("End Failed")

  });

  ArduinoOTA.begin();
}


/********************************** START SETUP AIRCON *****************************************/

void airconSetup(void) {
    daikinAircon.begin();
    setAircon();
}


/********************************** START SETUP DS18B20  *****************************************/

void temperatureSensorSetup(void) {

  gTemperatureSensor.pSensor->begin();
 if (!gTemperatureSensor.pSensor->getAddress(gTemperatureSensor.ds18DeviceAddress, 0))
    Serial.println("Unable to find address for Device 0"); 
  
  gTemperatureSensor.pSensor->setResolution(gTemperatureSensor.ds18DeviceAddress, 9);
  DEBUG_PRINT("Device 0 Resolution: ")
  DEBUG_PRINTLN(gTemperatureSensor.pSensor->getResolution(gTemperatureSensor.ds18DeviceAddress))

  float tempC = gTemperatureSensor.pSensor->getTempC(gTemperatureSensor.ds18DeviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    DEBUG_PRINTLN("Error: Could not read temperature data")
    tempC=-1.0;
  } else {
    DEBUG_PRINT("Temp C: ")
    DEBUG_PRINTLN(tempC)
  }
  gTemperatureSensor.lastTemp= tempC;
}

 

/********************************** START SETUP WIFI*****************************************/
void wifiSetup(void) {

   wifiMulti.addAP(WIFISSID1, WIFIPWD1);
   wifiMulti.addAP(WIFISSID2 ,WIFIPWD2);
  // We start by connecting to a WiFi network
   while (wifiMulti.run() != WL_CONNECTED)
   { 
    // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(200);
   }  

//  while (retCon != WL_CONNECTED) {
//    WiFi.printDiag(Serial);
//    DEBUG_PRINT("Wifi status: ")
//    switch (retCon) {
//      case  WL_CONNECTED: DEBUG_PRINTLN(" WL_CONNECTED"); break;
//      case  WL_NO_SSID_AVAIL: DEBUG_PRINTLN(" WL_NO_SSID_AVAIL"); break;
//      case  WL_CONNECT_FAILED: DEBUG_PRINTLN(" WL_CONNECT_FAILED (maybe wrong pwd)"); break;
//      case  WL_IDLE_STATUS: DEBUG_PRINTLN(" WL_IDLE_STATUS"); break;
//      default: DEBUG_PRINTLN("WL_DISCONNECTED"); break;
//    }
//
//    delay(200);
//    retCon = WiFi.status() ;
//  }
  DEBUG_PRINT("Connected to ");
  DEBUG_PRINT(WiFi.SSID());              // Tell us what network we're connected to
  DEBUG_PRINT(" IP address:\t");
  DEBUG_PRINTLN(WiFi.localIP()); 


  if (!MDNS.begin(DEVICENAME)) {   // Start the mDNS responder for DEVICENAME.local
    DEBUG_PRINTLN("Error setting up MDNS responder")
  }
  
  DEBUG_PRINT("Device answers to ")
  DEBUG_PRINT(DEVICENAME)
  DEBUG_PRINTLN(".local")
  
}



/********************************** SETUP ****************************************/
void setup(void) {

  // serial
  Serial.begin(115200);

  //debug
  debugSetup();

  //wifi not starting fix
  WiFi.mode(WIFI_OFF);
  // setup pins
  pinSetup();
  wifiSetup();
  ntpSetup();
  webServerSetup();
  mqttSetup();  
  OtaSetup();
  DEBUG_PRINTLN("WiFi connected")
  DEBUG_PRINT("IP address: ")
  DEBUG_PRINTLN(WiFi.localIP())
  airconSetup();
  temperatureSensorSetup();

  //led off once setup is done
  digitalWrite(LED_CARTE,HIGH);

}
