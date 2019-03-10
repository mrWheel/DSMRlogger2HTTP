/*
***************************************************************************  
**  Program  : MenuStuff, part of DSMRlogger2HTTP
**  Version  : v0.7.8
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//===========================================================================================
void displayDaysHist(bool Telnet=true) {
//===========================================================================================
  char ED[20], ER[20], GD[20], today;
  int thisDay = weekday(unixTimestamp) - 1;
  int Label;

  if (Telnet) TelnetStream.println("\n======== WeekDay History ==========\r\n");

  for (int i=0; i<7; i++) {
    if (i == thisDay) today = '*';
    else              today = ' ';
    Label = weekDat[i].Label;
    dtostrf(weekDat[i].EnergyDelivered, 9, 3, ED);
    dtostrf(weekDat[i].EnergyReturned, 9, 3, ER);
    dtostrf(weekDat[i].GasDelivered, 8, 2, GD);
    sprintf(cMsg, "[%02d][%02d]%c Energy Del.[%s], Ret.[%s], Gas Del.[%s]\r", i, Label, today
                                                                            , ED, ER, GD);
    if (Telnet) TelnetStream.println(cMsg);
  }
  TelnetStream.flush();
  
} // displayDaysHist()


//===========================================================================================
void displayHoursHist(bool Telnet=true) {
//===========================================================================================
  char ED[20], ER[20], GD[20], hour;
  int thisHour = HourFromTimestamp(pTimestamp);
  int Label;
  
  if (Telnet) TelnetStream.println("\n======== Hours History ==========\r\n");
  for (int i=1; i<=8; i++) {
    if (i == thisHour)  hour = '*';
    else                hour = ' '; 
    Label = hoursDat[i].Label;
    dtostrf(hoursDat[i].EnergyDelivered, 9, 3, ED);
    dtostrf(hoursDat[i].EnergyReturned, 9, 3, ER);
    dtostrf(hoursDat[i].GasDelivered, 8, 2, GD);
    sprintf(cMsg, "[%02d][%02d]%c Energy Del.[%s], Ret.[%s], Gas Del.[%s]\r", i, Label, hour
                                                                          , ED, ER, GD);
    if (Telnet) TelnetStream.println(cMsg);

  }
  TelnetStream.flush();

} // displayHoursHist()


//===========================================================================================
void displayMonthsHist(bool Telnet=true) {
//===========================================================================================
  char ED[20], ER[20], GD[20], signal;
  int thisMonth = MonthFromTimestamp(pTimestamp);
  int thisYear  = YearFromTimestamp(pTimestamp);
  int Label;
  
  if (Telnet) TelnetStream.println("\n======== Months History ==========\r\n");
  for (int i=1; i<=24; i++) {
    Label = monthsDat[i].Label;
    dtostrf(monthsDat[i].EnergyDelivered, 9, 3, ED);
    dtostrf(monthsDat[i].EnergyReturned, 9, 3, ER);
    dtostrf(monthsDat[i].GasDelivered, 8, 2, GD);
    sprintf(cMsg, "[%02d][%04d] Energy Del.[%s], Ret.[%s], Gas Del.[%s]\r", i, Label
                                                                          , ED, ER, GD);
    if (Telnet) TelnetStream.println(cMsg);

  }
  TelnetStream.flush();

} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo() {
//===========================================================================================
  TelnetStream.println("\r\n==================================================================\r");
  TelnetStream.print(" \r\n               (c)2018 by [Willem Aandewiel (www.aandewiel.nl)");
  TelnetStream.print("]\r\n         Firmware Version [");  TelnetStream.print( _FW_VERSION );
  TelnetStream.print("]\r\n                 Compiled [");  TelnetStream.print( __DATE__ ); 
                                                           TelnetStream.print( "  " );
                                                           TelnetStream.print( __TIME__ );
  TelnetStream.print("]\r\n      Telegrams Processed [");  TelnetStream.print( telegramCount );
  TelnetStream.print("]\r\n              With Errors [");  TelnetStream.print( telegramErrors );
  TelnetStream.print("]\r\n                 FreeHeap [");  TelnetStream.print( ESP.getFreeHeap() );
  TelnetStream.print("]\r\n                  Chip ID [");  TelnetStream.print( ESP.getChipId(), HEX );
  TelnetStream.print("]\r\n             Core Version [");  TelnetStream.print( ESP.getCoreVersion() );
  TelnetStream.print("]\r\n              SDK Version [");  TelnetStream.print( ESP.getSdkVersion() );
  TelnetStream.print("]\r\n           CPU Freq (MHz) [");  TelnetStream.print( ESP.getCpuFreqMHz() );
  TelnetStream.print("]\r\n         Sketch Size (kB) [");  TelnetStream.print( ESP.getSketchSize() / 1024.0 );
  TelnetStream.print("]\r\n   Free Sketch Space (kB) [");  TelnetStream.print( ESP.getFreeSketchSpace() / 1024.0 );

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X", ESP.getFlashChipId());
  TelnetStream.print("]\r\n            Flash Chip ID [");  TelnetStream.print( cMsg );
  TelnetStream.print("]\r\n     Flash Chip Size (kB) [");  TelnetStream.print( ESP.getFlashChipSize() / 1024 );
  TelnetStream.print("]\r\nFlash Chip Real Size (kB) [");  TelnetStream.print( ESP.getFlashChipRealSize() / 1024 );
  TelnetStream.print("]\r\n         Flash Chip Speed [");  TelnetStream.print( ESP.getFlashChipSpeed() / 1000 / 1000 );
  FlashMode_t ideMode = ESP.getFlashChipMode();
  TelnetStream.print("]\r\n          Flash Chip Mode [");  TelnetStream.print( flashMode[ideMode] );

  TelnetStream.println("]\r");

  TelnetStream.println("==================================================================");
  TelnetStream.print(" \r\n               Board type [");
#ifdef ARDUINO_ESP8266_NODEMCU
    TelnetStream.print("ESP8266_NODEMCU");
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    TelnetStream.print("ESP8266_GENERIC");
#endif
#ifdef ESP8266_ESP01
    TelnetStream.print("ESP8266_ESP01");
#endif
#ifdef ESP8266_ESP12
    TelnetStream.print("ESP8266_ESP12");
#endif
  TelnetStream.print("]\r\n                     SSID [");  TelnetStream.print( WiFi.SSID() );
  TelnetStream.print("]\r\n                  PSK key [");  TelnetStream.print( WiFi.psk() );
  TelnetStream.print("]\r\n               IP Address [");  TelnetStream.print( WiFi.localIP() );
  TelnetStream.print("]\r\n                 Hostname [");  TelnetStream.print( HOSTNAME );
  TelnetStream.print("]\r\n                   upTime [");  TelnetStream.print( upTime() );
  TelnetStream.println("]\r");
  TelnetStream.println("==================================================================\r\n");
  TelnetStream.flush();

} // displayBoardInfo()


//===========================================================================================
void waitForOTAupload() {
//===========================================================================================
  uint32_t maxWait = millis() + 33000;

  TelnetStream.println("Wait 30 seconds for OTAupload to start....\r");
  TelnetStream.flush();

  OTAinProgress = true;
  while(maxWait > millis()) { // hm.. does not seem to work ..
    ArduinoOTA.handle();
    delay(10);
  }
  TelnetStream.println("now Rebooting.\r");
  TelnetStream.flush();
  ESP.reset();
    
} // waitForOTAupload()


//===========================================================================================
void handleKeyInput() {
//===========================================================================================
  char    inChar;

  while (TelnetStream.available() > 0) {
    yield();
    inChar = (char)TelnetStream.read();
    
    switch(inChar) {
      case 'b':
      case 'B':     displayBoardInfo();
                    break;
      case 'd':
      case 'D':     displayDaysHist(true);
                    displayHoursHist(true);
                    displayMonthsHist(true);
                    break;
      case 'F':     TelnetStream.printf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", APname);
                    TelnetStream.flush();
                    delay(1000);
                    WiFi.disconnect();  // deletes credentials !
                    //setupWiFi(true);
                    ESP.reset();
                    break;
      case 'G':     if (!readWeekData())  TelnetStream.println("handleKeyInput(): error readWeekData()!");
                    if (!readHourData())  TelnetStream.println("handleKeyInput(): error readHourData()!");
                    if (!readMonthData()) TelnetStream.println("handleKeyInput(): error readMonthData()!");
                    break;
      case 'i':
      case 'I':     for(int I=0; I<10; I++) {
                      digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
                      delay(1000);
                    }
                    break;
#ifdef HAS_NO_METER
      case 'Z':     createDummyData();
                    break;
      case 'm':
      case 'M':     testMonth++;
                    break;
#else
      case 'n':
      case 'M':     showRaw = !showRaw;
                    break;
#endif
      case 'R':     TelnetStream.print("Reboot in 3 seconds ... \r");
                    TelnetStream.flush();
                    delay(3000);
                    TelnetStream.println("now Rebooting.                      \r");
                    TelnetStream.flush();
                    ESP.reset();
                    break;
      case 's':
      case 'S':     listSPIFFS();
                    break;
      case 'U':     saveWeekData();
                    saveHourData(9);
                    saveThisMonth(thisYear, thisMonth, false);
                    break;
      case 'v':
      case 'V':     if (Verbose) {
                      TelnetStream.println("Verbose is OFF\r");
                      Verbose = false;
                    } else {
                      TelnetStream.println("Verbose is ON\r");
                      Verbose = true;
                    }
                    TelnetStream.flush();
                    break;
      case 'w':
      case 'W':     if (waitForATOupdate > millis()) {
                        OTAinProgress = false;
                        waitForATOupdate = millis();
                    } else {
                        OTAinProgress = true;
                        waitForATOupdate = millis() + 30000;  // wait for 30 seconds
                    }
                    break;
      default:      TelnetStream.println("\nCommands are:\r\n");
                    TelnetStream.println("   B - Board Type\r");
                    TelnetStream.println("   D - Display tables from memory\r");
                    TelnetStream.println("  *F - Force Re-Config WiFi\r");
                    TelnetStream.println("  *G - Get from SPIFFS (read Data-files)\r");
                    TelnetStream.printf ("   I - Identify by blinking LED on GPIO[%02d]\r\n", BUILTIN_LED);
#ifdef HAS_NO_METER
                    TelnetStream.println("  *Z - create Dummy Data\r");
                    TelnetStream.println("   m - force next Month\r");
#endif
                    if (showRaw) {
                      TelnetStream.println("   N - Start Parsing again\r");
                    } else {
                      TelnetStream.println("   N - No Parsing (show RAW data from Smart Meter)\r");
                      showRawCount = 0;
                    }
                    TelnetStream.println("  *R - Reboot\r");
                    TelnetStream.println("   S - SPIFFS space available\r");
                    TelnetStream.println("  *U - Update SPIFFS (save Data-files)\r");
                    TelnetStream.println("   V - Toggle Verbose\r");
                    if (waitForATOupdate > millis()) {
                      TelnetStream.println("   W - Cancel Wait for ATO update\r");
                    } else {
                      TelnetStream.println("   W - Wait for OTA upload \r");
                    }
                    TelnetStream.println("\r");

    } // switch()
    while (TelnetStream.available() > 0) {
       yield();
       (char)TelnetStream.read();
    }
  }
  
} // handleKeyInput()


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
