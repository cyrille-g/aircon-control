

/* -------------------------------- WEBSERVER HANDLERS ------------------------*/

String getSelection(String name, int min, int max, int selected, setting_t* list) {
  String ret = "<select name=\"" + name + "\">";
  for (int i = min; i <= max; i++) {
    ret += "<option ";
    if (list[i].value == selected)
      ret += "selected ";
    ret += "value = " + String(list[i].value) + " > " + String(list[i].name) + "</option>";
  }
  return ret += "</select><br\>";
}

void handleRoot() {
  gTemperatureSensor.pSensor->requestTemperatures(); // Send the command to get temperatures

  float tempC = gTemperatureSensor.pSensor->getTempC(gTemperatureSensor.ds18DeviceAddress);
  String resp("");
  resp += "<head><title>";
  resp += DEVICENAME;
  resp += "</title></head><body><h1>";
  resp += DEVICENAME;
  resp += " interface</h1><div><form method=\"POST\" action=\"command\">";

  if (tempC == DEVICE_DISCONNECTED_C)
  {
    tempC = -1.0;
    resp += "-- Current room temperature unavailable -- <BR><BR>";
  } else {
    resp += "Current room temperature: " + String(tempC, 2) + "<BR><BR>";
  }
  gTemperatureSensor.lastTemp = tempC;

  resp += "Power: " + getSelection("power", 0, 1, gAircon.power, gOnOffStates);
  resp += "Temperature: <select name=\"temperature\">";
  for (int i = DAIKIN_MIN_TEMP; i <= DAIKIN_MAX_TEMP; i++) {
    resp += "<option ";
    if (i == gAircon.temperature)
      resp += "selected ";
    resp += ">" + String(i) + "</option>";
  }
  resp += "</select><br\>";
  resp += "Mode: " + getSelection("mode", 0, 5, gAircon.programMode, gProgramStates);
  resp += "Fan speed: " + getSelection("fan", 0, 5, gAircon.fan, gFanSpeedStates);
  resp += "Powerful Mode: " + getSelection("powerful", 0, 1, gAircon.powerful, gOnOffStates);
  resp += "Quiet Mode: " + getSelection("quiet", 0, 1, gAircon.quiet, gOnOffStates);
  resp += "Vertical Swing: " + getSelection("swingv", 0, 1, gAircon.swingVertical, gOnOffStates);
  resp += "<BR><BR><input type=\"submit\">" \
          "</form></div>" \
          "</body>" \
          "</html>";
  gWebServer.send(200, "text/html", resp);
}


void handleCommmand() {
  String argName;
  unsigned long arg;
  for (uint8_t i = 0; i < gWebServer.args(); i++) {
    argName = gWebServer.argName(i);
    arg = atoi(gWebServer.arg(i).c_str());
    if (argName == "temperature") {
      gAircon.temperature = arg;
    }
    else if (argName == "fan") {
      gAircon.fan = arg;
    }
    else if (argName == "power") {
      gAircon.power = arg;
    }
    else if (argName == "powerful") {
      gAircon.powerful = arg;
    }
    else if (argName == "quiet") {
      gAircon.quiet = arg;
    }
    else if (argName == "swingv") {
      gAircon.swingVertical = arg;
    }
    else if (argName == "mode") {
      gAircon.programMode = arg;
    }
    DEBUG_PRINT(argName);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN(arg);
  }
  setAircon();
  handleRoot();
}

void handleNotFound() {
  gWebServer.send(404, "text/plain", "404 File Not Found");
}

void handleReset (void) {
  gWebServer.send(200, "text/plain", "Resetting in 1s");
  delay(1000);
  ESP.reset();
}

void handleLog() {

  String resp("");

  resp += "<html>" \
          "<head><title>" ;
  resp += DEVICENAME;
  resp += "</title></head><body><h1>";
  resp += DEVICENAME;
  resp += " logs</h1>" ;
#ifdef WEB_DEBUG
  extractLog(&resp,true);
#endif
  resp += "</body></html>";
  gWebServer.send(200, "text/html", resp);


}
