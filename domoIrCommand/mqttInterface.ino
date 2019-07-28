/********************************** START RECONNECT*****************************************/
bool tryReconnectMqtt(void)
{
  DEBUG_PRINT("Attempting MQTT connection...")
  // Attempt to connect
  if (gMqttClient.connect(DEVICENAME, gConfMqttUsername, gConfMqttPassword)) 
  {
    DEBUG_PRINTLN(" connected")

    /* subscribe to the set topic */
    gMqttClient.subscribe(gAircon.setTopic);

    /* send our current state */
    sendMqttState();
    return true;
 } else {
    DEBUG_PRINT(" failed, rc=");
    DEBUG_PRINTLN(gMqttClient.state());
    return false;
  }
}

/********************************** START SEND STATE*****************************************/
void sendMqttState(void) {
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;

  JsonObject& airconRoot = jsonBuffer.createObject();

  airconRoot["Aircon"]["temperature"] = gAircon.temperature;
  airconRoot["Aircon"]["fan"] = gAircon.fan;
  airconRoot["Aircon"]["power"] = gAircon.power;
  airconRoot["Aircon"]["powerful"] = gAircon.powerful;
  airconRoot["Aircon"]["quiet"] = gAircon.quiet;
  airconRoot["Aircon"]["swingVertical"] = gAircon.swingVertical;
  airconRoot["Aircon"]["programMode"] = gAircon.programMode;
  
  char buffer[airconRoot.measureLength() + 1];
  airconRoot.printTo(buffer, sizeof(buffer));

  gMqttClient.publish(gAircon.stateTopic, buffer, true);
  
  jsonBuffer.clear();
  JsonObject& sensorRoot = jsonBuffer.createObject();
  sensorRoot["sensor"]["temperature"] = gTemperatureSensor.lastTemp;
  sensorRoot["sensor"]["humidity"] = "not yet available";

  char secondBuffer[sensorRoot.measureLength() + 1];
  sensorRoot.printTo(secondBuffer, sizeof(secondBuffer));

  gMqttClient.publish(gTemperatureSensor.stateTopic, secondBuffer, true);

  
}

/********************************** JSON PROCESSOR *****************************************/
bool processJson(char* message) {
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    DEBUG_PRINTLN("parseObject() failed")
    return false;
  }

  const char* res = NULL;
  String resAsString(res);
  if (res = root["power"] ) {
    gAircon.power = getOnOffStateFromString(resAsString);
    DEBUG_PRINT("Received power ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No power received")
  }

  if (res = root["temperature"])   {
    gAircon.temperature = atoi(res);
    DEBUG_PRINT("Received temperature ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No temperature received")
  }

  if (res = root["fan"]) {
    gAircon.fan = getFanStateFromString(resAsString);
    DEBUG_PRINT("Received fan ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No fan received")
  }

  if (res = root["powerful"]) {
    gAircon.powerful = getOnOffStateFromString(resAsString);
    DEBUG_PRINT("Received powerful ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No powerful received")
  }

  if (res = root["quiet"]) {
    gAircon.quiet = getOnOffStateFromString(resAsString);
    DEBUG_PRINT("Received quiet ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No quiet received")
  }

  if (res = root["swingVertical"]) {
    gAircon.swingVertical = getOnOffStateFromString(resAsString);
    DEBUG_PRINT("Received swingVertical ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No swingVertical received")
  }
  
   if (res = root["programMode"]) {
    gAircon.programMode = getProgramStateFromString(resAsString);
    DEBUG_PRINT("Received programMode ")
    DEBUG_PRINTLN(res)
  } else {
    DEBUG_PRINTLN("No programMode received")
  }
    
  return true;
}


/********************************** START CALLBACK*****************************************/
void airconCommandCallback(char* topic, byte* payload, unsigned int length) {
#ifdef SERIAL_DEBUG
  DEBUG_PRINTLN("Message arrived [" + );
  Serial.print(topic);
  Serial.print("] ");
#endif

  char *topicToTokenize = (char*) malloc (strlen(topic) + 1);
  strcpy(topicToTokenize, topic);

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
#ifdef SERIAL_DEBUG  
  Serial.println(message);
#endif

  /* first, check topic validity. If it is MQTT_PREFIX /room/MQTT_POWER, then go forward */
  char *token;

  /* get the first token */
  token = strtok(topicToTokenize, "/");

  // does it start with MQTT_PREFIX ?
  if (token == NULL ) {
    //no; leave
#ifdef SERIAL_DEBUG      
    Serial.println("No /");
#endif
    return;
  }

  if (strcmp(token, MQTT_PREFIX) != 0) {
    //no; leave
#ifdef SERIAL_DEBUG      
    Serial.println("wrong MQTT prefix");
#endif
    return;
  }
  token = strtok(NULL, "/");
  if (token == NULL) {
#ifdef SERIAL_DEBUG  
    Serial.println("wrong MQTT topic");
#endif
    return;
  }

#ifdef SERIAL_DEBUG  
  Serial.print("looking for ");
  Serial.println(token);
#endif

  token = strtok(NULL, "/");
  if (token == NULL) {
    #ifdef SERIAL_DEBUG
    Serial.println("wrong MQTT_SET");
    #endif
    return;
  }

  // does it end with MQTT_SET ?
  if (strcmp(token , MQTT_SET) != 0)  {
    #ifdef SERIAL_DEBUG
    Serial.println("Unknown power suffix");
    #endif
    return;
  }

  // there should not be any data left
  token = strtok(NULL, "/");
  if (token != NULL) {
    #ifdef SERIAL_DEBUG
    Serial.println("Topic too long");
    #endif 
    return;
  }

  // tell the Json processor which queue it should use
  // by passing index
  if (!processJson(message)) {
    return;
  }
  sendMqttState();
}
