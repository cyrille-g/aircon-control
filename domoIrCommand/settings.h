#ifndef _CGE_SETTINGS_EMPTY_H
#define _CGE_SETTINGS_EMPTY_H

/* DEFINES */
/* remove this line to be able to compile. Please do it once 
 *  mac address is set */

#error configure settings.h before compiling, then remove this line


/**************************** device nae, used for OTA, MQTT, mDNS,html ***********/
#define DEVICENAME "aircon" 

/**************************** OTA **************************************************/
#define OTAPASSWORD "otapwd" //the gConfPassword you will need to enter to upload remotely via the ArduinoIDE
#define OTAPORT 8800

/******************************* MQTT **********************************************/
#define MQTT_MAX_PACKET_SIZE 512

/******************************** serial ********************************************/

#define LOG_BUFFER_SIZE 200

/******************************** DEBUG ********************************************/

#define WEBLOG_QUEUE_SIZE 12


#define DEBUG // If defined debug output is activated
#define WEB_DEBUG 

#ifdef DEBUG
  #ifdef WEB_DEBUG
    #define DEBUG_PRINT(x)    {do {Serial.print(x); addLog(x);}while(0);}
    #define DEBUG_PRINTLN(x)  {do {Serial.println(x); addLog(x);}while(0);}
  #else
    #define DEBUG_PRINT(x)     {Serial.print(x);}
    #define DEBUG_PRINTLN(x)   {Serial.println(x);}
  #endif
#else
  #define DEBUG_PRINT(x)      {}
  #define DEBUG_PRINTLN(x)    {} 
#endif 


/******************************** HARDWARE *******************************************/
#define LED_CARTE BUILTIN_LED

/* PWM maximum value is defined as PWMRANGE on esp8266*/
#define MAX_PWM_COMMAND PWMRANGE
#define PWM_COMMANDED_ZERO_UNDER_COMMAND 5

/************ WIFI and MQTT Information (CHANGE THESE FOR YOUR SETUP) **************/

#define WIFISSID1 "0" 
#define WIFIPWD1 "0"
#define WIFISSID2 "0"
#define WIFIPWD2 "0"

const char* gConfMqttServerIp = "0";
const char* gConfMqttUsername = "0";
const char* gConfMqttPassword = "0";
const int   gConfMqttPort = 0;

#define MQTT_WAIT_RECONNECT_COUNTER 50000
/************* MQTT TOPICS (change these topics as you wish)  **********************/
#define MQTT_PREFIX "aircon"
#define MQTT_STATE "state"
#define MQTT_SET "command"

const char* gConfOnCommand = "ON";
const char* gConfOffCommand = "OFF";

/**************************************** NETWORK **********************************/

// the media access control (ethernet hardware) address for the shield:
const byte gConfMacAddress[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; 

/**************************************** TIME **************************************/
const uint16_t gConfNtpServerPort = 123;
const char gConftimeServer[] = "pool.ntp.org";

/****************************** AIRCON SETTINGS *********************************/
#define DEFAULT_TEMP 25
#define DEFAULT_FAN_SPEED DAIKIN_FAN_AUTO
#define DEFAULT_POWER 0
#define DEFAULT_POWERFUL 0
#define DEFAULT_QUIET 0
#define DEFAULT_SWING_H 0
#define DEFAULT_SWING_V 0
#define DEFAULT_MODE DAIKIN_COOL

/********************************** WEBSERVER ****************************************/
#define WEBSERVER_PORT 80



#endif 
