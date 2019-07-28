#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <ESP8266WiFiMulti.h> 
//#include <ESP8266WiFi.h>
//#include <WiFiUdp.h>
#include <ESP8266mDNS.h> 

#include <ArduinoOTA.h>
#include <NtpClientLib.h>

#include "settings.h"
#include "globalVars.h"


/********************************** START MAIN LOOP*****************************************/
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    delay(5);
    DEBUG_PRINTLN("WIFI Disconnected. Attempting reconnection")
    wifiSetup();
    //led off once setup is done
    digitalWrite(LED_CARTE,HIGH);
    return;
  }

  ArduinoOTA.handle();

  /* we do not want to process anything in OTA mode */
  if (gOtaUpdating) {
    delay(50);
    return;
  }

  if (!gMqttClient.connected() )
  {
    if (gMqttWaitingCounter < MQTT_WAIT_RECONNECT_COUNTER)
    {
      gMqttWaitingCounter++;
    } else {
      gMqttWaitingCounter=0;
      DEBUG_PRINTLN("try reconnecting MQTT")
      tryReconnectMqtt();
    }
  }
  
  if (gMqttClient.connected())
  {
    gMqttClient.loop();
  }
  
  gWebServer.handleClient();

  /* process time only if no OTA is going on */
  if ((!gOtaUpdating) && gSyncEventTriggered) 
  {
    processSyncEvent (gNtpEvent);
    gSyncEventTriggered = false;
  }


}
