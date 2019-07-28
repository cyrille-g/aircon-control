#ifndef _CGE_GLOBAL_VARS_H
#define _CGE_GLOBAL_VARS_H

#include <ESP8266WiFiMulti.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <PubSubClient.h>
//#include <WiFiUdp.h>
#include <ir_Daikin.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/******************************** structures**************************/

typedef struct weblogQueueElem_s {
  char *pLogLine;
  STAILQ_ENTRY(weblogQueueElem_s) logEntry;
} weblogQueueElem_t;

/******************************** queue system ***********************/

typedef STAILQ_HEAD(, weblogQueueElem_s) weblogQueueHead_t;

/******************************** hardware ***************************/
int gOnboardLedState = HIGH;
bool gOtaUpdating = false;
int gPinD1 = LOW;
int gPinD2 = LOW; 
int gPinD3 = LOW; 
int gPinD4 = LOW;
int gPinD5 = LOW; 
int gPinD6 = LOW; 
int gPinD7 = LOW;
int gPinD8 = LOW;

/******************************** network ****************************/
ESP8266WiFiMulti wifiMulti; 
WiFiClient   gEspClient;
PubSubClient gMqttClient(gEspClient);
ESP8266WebServer gWebServer(WEBSERVER_PORT);
ESP8266HTTPUpdateServer gHttpUpdater; // web interface to remotely update the firmware

/******************************* web log  ******************************/
weblogQueueHead_t gLogQueue;
uint8_t gWeblogCount;
char gLogBuffer[LOG_BUFFER_SIZE] = {0};

/**************************************** JSON ************************/
const int JSON_BUFFER_SIZE = JSON_OBJECT_SIZE(10);

/******************************IR    management ***********************/
typedef struct {
  char *setTopic = NULL;
  char *stateTopic = NULL;
  uint8_t temperature = DEFAULT_TEMP;
  uint8_t fan = DEFAULT_FAN_SPEED;
  uint8_t power = DEFAULT_POWER;
  uint8_t powerful = DEFAULT_POWERFUL;
  uint8_t quiet = DEFAULT_QUIET;
  uint8_t swingVertical = DEFAULT_SWING_V ;
  uint8_t programMode = DEFAULT_MODE;
} aircon_t;

aircon_t gAircon;

OneWire gOneWire(D6);
DallasTemperature gSensor(&gOneWire);

typedef struct {
  OneWire *pOneWire = &gOneWire;
  DallasTemperature *pSensor = &gSensor;
  DeviceAddress ds18DeviceAddress;
  float lastTemp = -1.0;
  char *setTopic = NULL;
  char *stateTopic = NULL;
} temperatureSensor_t;


temperatureSensor_t gTemperatureSensor;


typedef struct setting_t {
  String name;
  uint8_t value;
} setting_t;

#define FAN_STATE_COUNT 6

setting_t gFanSpeedStates[FAN_STATE_COUNT] = { { "Auto", DAIKIN_FAN_AUTO }, { "Slowest", 1 }, { "Slow", 2 }, { "Medium", 3 }, { "Fast", 4 }, { "Fastest", 5 } };

uint8_t getFanStateFromString(String str)
{
  int i;
  for (i = 0; i < FAN_STATE_COUNT; i++)
  {
    if ( gFanSpeedStates[i].name == str)
      return gFanSpeedStates[i].value;
  }
}

#define PROGRAM_STATE_COUNT 5

setting_t gProgramStates[PROGRAM_STATE_COUNT] = { { "Cool", DAIKIN_COOL }, { "Heat", DAIKIN_HEAT }, { "Fan", DAIKIN_FAN }, { "Auto", DAIKIN_AUTO }, { "Dry", DAIKIN_DRY } };

uint8_t getProgramStateFromString(String str)
{
  int i;
  for (i = 0; i < PROGRAM_STATE_COUNT; i++)
  {
    if ( gProgramStates[i].name == str)
      return gProgramStates[i].value;
  }
}

#define ON_OFF_COUNT 2

setting_t gOnOffStates[ON_OFF_COUNT] = { { "On", 1 }, { "Off", 0 } };


uint8_t getOnOffStateFromString(String str)
{
  int i;
  for (i = 0; i < ON_OFF_COUNT; i++)
  {
    if ( gOnOffStates[i].name == str)
      return gOnOffStates[i].value;
  }
}

IRDaikinESP daikinAircon(D2);  // An IR LED is controlled by D2

uint16_t gMqttWaitingCounter;

/**************************************** NTP ************************/
boolean gSyncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t gNtpEvent; // Last triggered event
#endif
