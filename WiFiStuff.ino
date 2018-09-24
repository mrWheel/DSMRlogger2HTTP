/*
***************************************************************************  
**  Program  : WiFiStuff, part of DSMRlogger2HTTP
**  Version  : v0.7.0
**
**  Copyright (c) 2018 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//===========================================================================================
//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager) {
//===========================================================================================
    //blinker.attach(5.0, blink);

    if (debug) Serial.print("configModeCallback(): Entered config mode @");
    writeLogFile("configModeCallback(): Entered config mode @");
    if (debug) Serial.print(WiFi.softAPIP());
    writeLogFile(String(WiFi.softAPIP()).c_str());
    if (debug) Serial.println(" with SSID: ");
    writeLogFile(" with SSID: ");
    //if you used auto generated SSID, print it
    if (debug) Serial.println("myWiFiManager->getConfigPortalSSID()");
    writeLogFile("myWiFiManager->getConfigPortalSSID()");
    myWiFiManager->getConfigPortalSSID();
  
}   // configModeCallback()



//===========================================================================================
void setupWiFi(bool forceAP) {
//===========================================================================================
String  tmpTopic;
int     noConnectWiFi;
char    MAC5hex[5];
byte    mac[6];

    digitalWrite(BUILTIN_LED, LED_ON);  
    WiFi.macAddress(mac);
    sprintf(APname, "%s-%02x-192.168.5.1", HOSTNAME, mac[5]);
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);
    noConnectWiFi = 30;

    if (forceAP) {
        if (debug) Serial.printf("Restart DSMR to start AP [%s] to re-configure\r\n", APname);
        TelnetStream.printf("Restart DSMR to start AP [%s] to re-configure\r\n", APname);
        writeLogFile("Start AP to re-configure ..");
        wifiManager.resetSettings();
        noConnectWiFi   = 0;
    }

    WiFi.hostname(HOSTNAME);

    wifiManager.setTimeout(180);  // timeOut in 180 seconds!

#ifdef ARDUINO_ESP8266_GENERIC
      wifiManager.setAPStaticIPConfig(IPAddress(192,168,5,1), IPAddress(192,168,5,1), IPAddress(255,255,255,0));
#else
      wifiManager.setAPStaticIPConfig(IPAddress(192,168,6,1), IPAddress(192,168,6,1), IPAddress(255,255,255,0));
#endif

    WiFi.mode(WIFI_AP_STA);
    if (forceAP) {
      if (!wifiManager.startConfigPortal(APname)) {
        Serial.println("startConfigPortal(): Failed to connect and hit timeout");
        TelnetStream.println("startConfigPortal(): Failed to connect and hit timeout");
        TelnetStream.flush();
        writeLogFile("startConfigPortal(): Failed to connect and hit timeout");
        delay(1000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(1000);
      } 
    } else {
      if (!wifiManager.autoConnect(APname)) {
         writeLogFile("autoConnect(): Failed to connect and hit timeout");
         delay(1000);
         //reset and try again, or maybe put it to deep sleep
         ESP.reset();
         delay(1000);
      }
    }
    //if you get here you have connected to the WiFi AFTER reconfiguring!
    if (debug) Serial.println("connected to WiFi ... Yes! ;-)");
    Serial.flush();
    TelnetStream.println("connected to WiFi ... Yes! ;-)");
    TelnetStream.flush();
    writeLogFile("connected to WiFi!");

    // WiFi connexion is OK
    if (debug) {
      Serial.println ( "" );
      Serial.print ( "Connected to " ); Serial.println ( WiFi.SSID() );
      Serial.print ( "IP address: " );  Serial.println ( WiFi.localIP() );
    }
    TelnetStream.println ( "" );
    TelnetStream.print ( "Connected to " ); TelnetStream.println ( WiFi.SSID() );
    TelnetStream.print ( "IP address: " );  TelnetStream.println ( WiFi.localIP() );

    if (debug) {
      Serial.print(  "setupWiFi(): Free Heap[B]: ");
      Serial.println( ESP.getFreeHeap() );
      Serial.print(  "setupWiFi(): Connected to SSID: ");
      Serial.println( WiFi.SSID() );
      Serial.print(  "setupWiFi(): IP address: ");
      Serial.println( WiFi.localIP() );
      Serial.flush();
    }
    TelnetStream.print(  "setupWiFi(): Free Heap[B]: ");
    TelnetStream.println( ESP.getFreeHeap() );
    TelnetStream.print(  "setupWiFi(): Connected to SSID: ");
    TelnetStream.println( WiFi.SSID() );
    TelnetStream.print(  "setupWiFi(): IP address: ");
    TelnetStream.println( WiFi.localIP() );
    TelnetStream.flush();

    digitalWrite(BUILTIN_LED, LED_OFF);

}   // setupWiFi()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
