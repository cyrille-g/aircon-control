# aircon-control
esp8266 based bridge between aircon units and domotics systems / web servers. 

Are you tired of coming back from work to find a very hot or very cold home ? Have an old air conditionning unit that cannot be easily integrated in a domotic system ?

This program is for you. It uses an inexpensive wemos d1 mini, an IR led and a temperature sensor to bridge the IR command of a daikin air conditionning unit to a domotic system using MQTT. It also offers a web server interface.

Finally, it is very easy to convert to any other aircon system.

Libs needed :

- PubSubClient
- ArduinoJson
- ArduinoOTA
- NtpClientLib

NtpClientLib also uses the Time lib. 

I use these dependencies in nearly all my projects. Currently the Ntp client is not used, so feel free to remove it.
