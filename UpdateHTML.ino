/*
***************************************************************************  
**  Program  : UpdateHTML, part of DSMRlogger2HTTM
**  Version  : v0.6.2
**
**  Copyright (c) 2018 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//=======================================================================
void sendDataDeviceInfo() {
//=======================================================================
  
//-Slimme Meter Info----------------------------------------------------------
  jsonString  = "{";
  jsonString += " \"Identification\":\"" + String(Identification) + "\"";
  jsonString += ",\"P1_Version\":\"" + String(P1_Version) + "\"";
  jsonString += ",\"Equipment_Id\":\"" + String(Equipment_Id) + "\"";
  jsonString += ",\"Electricity_Tariff\":\"" + String(ElectricityTariff) + "\"";
  jsonString += ",\"Gas_Device_Type\":\"" + String(GasDeviceType) + "\"";
  jsonString += ",\"Gas_Equipment_Id\":\"" + String(GasEquipment_Id) + "\"";
  
//-Device Info-----------------------------------------------------------------
  jsonString += ",\"Author\":\"Willem Aandewiel (www.aandewiel.nl)\"";
  jsonString += ",\"FwVersion\":\""         + String( _FW_VERSION ) + "\"";
  jsonString += ",\"Compiled\":\""          + String( __DATE__ ) 
                                            + String( "  " )
                                            + String( __TIME__ ) + "\"";
  jsonString += ",\"FreeHeap\":\""          + String( ESP.getFreeHeap() ) + "\"";
  jsonString += ",\"ChipID\":\""            + String( ESP.getChipId(), HEX ) + "\"";
  jsonString += ",\"CoreVersion\":\""       + String( ESP.getCoreVersion() ) + "\"";
  jsonString += ",\"SdkVersion\":\""        + String( ESP.getSdkVersion() ) + "\"";
  jsonString += ",\"CpuFreqMHz\":\""        + String( ESP.getCpuFreqMHz() ) + "\"";
  jsonString += ",\"SketchSize\":\""        + String( (ESP.getSketchSize() / 1024.0), 3) + "kB\"";
  jsonString += ",\"FreeSketchSpace\":\""   + String( (ESP.getFreeSketchSpace() / 1024.0), 3 ) + "kB\"";

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X", ESP.getFlashChipId());
  jsonString += ",\"FlashChipID\":\""       + String(cMsg) + "\"";  // flashChipId
  jsonString += ",\"FlashChipSize\":\""     + String( (float)(ESP.getFlashChipSize() / 1024.0 / 1024.0), 3 ) + "MB\"";
  jsonString += ",\"FlashChipRealSize\":\"" + String( (float)(ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3 ) + "MB\"";
  jsonString += ",\"FlashChipSpeed\":\""    + String( (float)(ESP.getFlashChipSpeed() / 1000.0 / 1000.0) ) + "MHz\"";
  jsonString += ",\"BoardType\":";
#ifdef ARDUINO_ESP8266_NODEMCU
    jsonString += String("\"ESP8266_NODEMCU\"");
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    jsonString += String("\"ESP8266_GENERIC\"");
#endif
#ifdef ESP8266_ESP01
    jsonString += String("\"ESP8266_ESP01\"");
#endif
#ifdef ESP8266_ESP12
    jsonString += String("\"ESP8266_ESP12\"");
#endif
  jsonString += ",\"SSID\":\""              + String( WiFi.SSID() ) + "\"";
//jsonString += ",\"PskKey\":\""            + String( WiFi.psk() ) + "\"";    // uncomment if you want to see this
  jsonString += ",\"IpAddress\":\""         +  WiFi.localIP().toString()  + "\"";
  jsonString += ",\"Hostname\":\""          + String( HOSTNAME ) + "\"";
  jsonString += ",\"TelegramCount\":\""     + String( telegramCount ) + "\"";
  jsonString += ",\"TelegramErrors\":\""    + String( telegramErrors ) + "\"";
  jsonString += ",\"statusLong\":\"" + lastStartup + "\"";
  for(int l = 0; l < NUMLASTLOG; l++) {
    jsonString += ",\"lastLogLine" + String((l+1)) + "\":\"[" + String(l) + "]lastLogLine: <b>" + String(lastLogLine[l]) + "</b>\"";
  }
  jsonString += "}";
  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendDataDeviceInfo(): send JSON string\r\n");

} // sendDataDeviceInfo()

//=======================================================================
void sendDataActual() {
//=======================================================================

#ifdef HAS_NO_METER
  hoursDat[0].EnergyDelivered += 0.33;
  EnergyDelivered = hoursDat[0].EnergyDelivered;
  hoursDat[0].EnergyReturned += 0.11;
  EnergyReturned  = hoursDat[0].EnergyReturned;
  hoursDat[0].GasDelivered += 0.08;
  GasDelivered    = hoursDat[0].GasDelivered;
#endif

//-Totalen----------------------------------------------------------

  jsonString  = "{";
  jsonString += " \"Timestamp\":\"" + String(pTimestamp) + "\"";
  jsonString += ",\"Energy_Delivered\":\"" + String(EnergyDelivered, 3) + "\"";
  jsonString += ",\"Energy_Returned\":\"" + String(EnergyReturned, 3) + "\"";
  jsonString += ",\"Gas_Delivered\":\"" + String(GasDelivered, 2) + "\"";
  jsonString += ",\"Energy_Delivered_Tariff1\":\"" + String(EnergyDeliveredTariff1, 3) + "\"";
  jsonString += ",\"Energy_Delivered_Tariff2\":\"" + String(EnergyDeliveredTariff2, 3) + "\"";
  jsonString += ",\"Energy_Returned_Tariff1\":\"" + String(EnergyReturnedTariff1, 3) + "\"";
  jsonString += ",\"Energy_Returned_Tariff2\":\"" + String(EnergyReturnedTariff2, 3) + "\"";
  jsonString += ",\"Power_Delivered\":\"" + String(PowerDelivered, 3) + "\"";
  jsonString += ",\"Power_Returned\":\"" + String(PowerReturned, 3) + "\"";
  jsonString += ",\"Voltage_l1\":\"" + String(Voltage_l1, 1) + "\"";
  jsonString += ",\"Current_l1\":\"" + String(Current_l1) + "\"";
  jsonString += ",\"Voltage_l2\":\"" + String(Voltage_l2, 1) + "\"";
  jsonString += ",\"Current_l2\":\"" + String(Current_l2) + "\"";
  jsonString += ",\"Voltage_l3\":\"" + String(Voltage_l3, 1) + "\"";
  jsonString += ",\"Current_l3\":\"" + String(Current_l3) + "\"";
  jsonString += ",\"Power_Delivered_l1\":\"" + String(PowerDelivered_l1) + "\"";
  jsonString += ",\"Power_Returned_l1\":\"" + String(PowerReturned_l1) + "\"";
  jsonString += ",\"Power_Delivered_l2\":\"" + String(PowerDelivered_l2) + "\"";
  jsonString += ",\"Power_Returned_l2\":\"" + String(PowerReturned_l2) + "\"";
  jsonString += ",\"Power_Delivered_l3\":\"" + String(PowerDelivered_l3) + "\"";
  jsonString += ",\"Power_Returned_l3\":\"" + String(PowerReturned_l3) + "\"";
  jsonString += "}";
  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendDataActual(): send JSON string\r\n");

} // sendDataActual()


//=======================================================================
void sendTableMonths() {
//=======================================================================
  String  sKomma = "";
  char    cYear1[10], cYear2[10];
  int8_t  iMonth, nextSlot, slot, nextSlot12, slot12;
  
  jsonString    = "[";
  for ( int16_t slot = 1; slot <= 12; slot++ ) {
    slot12      = slot + 12;
    nextSlot    = slot + 1;
    //if (nextSlot > 24) nextSlot = 24; 
    nextSlot12  = nextSlot + 12;
    if (nextSlot12 > 24) nextSlot12 = 24; // not very likely ;-)
    // if (Verbose)
    if (Verbose) TelnetStream.printf("sendTableMonths(): slotLabel[%04d], slot[%02d], nextSlot[%02d], slot12Label[%04d], slot12[%02d], nextSlot12[%02d] \r\n"
                                                       , monthsDat[slot].Label, slot, nextSlot, monthsDat[slot12].Label, slot12, nextSlot12);

    iMonth = (monthsDat[slot].Label % 100);
    sprintf(cYear1, "20%02d", (monthsDat[slot].Label / 100));
    sprintf(cYear2, "20%02d", (monthsDat[slot12].Label / 100));
    
    if (slot < 12) {
      jsonString +=  sKomma + "{\"Month\":\"" + monthName[iMonth] + "\""; 
      jsonString +=  ",\"ED1_Year\":\"" + String(cYear1) + "\""; 
      jsonString +=  ",\"Energy_Delivered1\":\"" + String(( monthsDat[slot].EnergyDelivered - monthsDat[nextSlot].EnergyDelivered), 3) + "\"";
      jsonString +=  ",\"ED2_Year\":\"" + String(cYear2) + "\""; 
      jsonString +=  ",\"Energy_Delivered2\":\"" + String(( monthsDat[slot12].EnergyDelivered - monthsDat[nextSlot12].EnergyDelivered), 3) + "\"";
      jsonString +=  ",\"ER1_Year\":\"" + String(cYear1) + "\""; 
      jsonString +=  ",\"Energy_Returned1\":\""  + String(( monthsDat[slot].EnergyReturned  - monthsDat[nextSlot].EnergyReturned), 3) + "\"";
      jsonString +=  ",\"ER2_Year\":\"" + String(cYear2) + "\""; 
      jsonString +=  ",\"Energy_Returned2\":\""  + String(( monthsDat[slot12].EnergyReturned  - monthsDat[nextSlot12].EnergyReturned), 3) + "\"";
      jsonString +=  ",\"GD1_Year\":\"" + String(cYear1) + "\""; 
      jsonString +=  ",\"Gas_Delivered1\":\""    + String(( monthsDat[slot].GasDelivered    - monthsDat[nextSlot].GasDelivered), 2) + "\"";
      jsonString +=  ",\"GD2_Year\":\"" + String(cYear2) + "\""; 
      jsonString +=  ",\"Gas_Delivered2\":\""    + String(( monthsDat[slot12].GasDelivered    - monthsDat[nextSlot12].GasDelivered), 2) + "\"";
      jsonString += "}";
    } else {
      jsonString +=  sKomma + "{\"Month\":\"Tot en met " + monthName[iMonth] + "\""; 
      jsonString +=  ",\"ED1_Year\":\"" + String(cYear1) + "\""; 
      jsonString +=  ",\"Energy_Delivered1\":\"" + String(monthsDat[slot].EnergyDelivered, 3) + "\"";
      jsonString +=  ",\"ED2_Year\":\"" + String(cYear2) + "\""; 
      jsonString +=  ",\"Energy_Delivered2\":\"" + String(monthsDat[slot12].EnergyDelivered, 3) + "\"";
      jsonString +=  ",\"ER1_Year\":\"" + String(cYear1) + "\""; 
      jsonString +=  ",\"Energy_Returned1\":\""  + String(monthsDat[slot].EnergyReturned, 3)  + "\"";
      jsonString +=  ",\"ER2_Year\":\"" + String(cYear2) + "\""; 
      jsonString +=  ",\"Energy_Returned1\":\""  + String(monthsDat[slot12].EnergyReturned, 3)  + "\"";
      jsonString +=  ",\"GD1_Year\":\"" + String(cYear1) + "\""; 
      jsonString +=  ",\"Gas_Delivered1\":\""    + String(monthsDat[slot].GasDelivered, 2)    + "\"";
      jsonString +=  ",\"GD2_Year\":\"" + String(cYear2) + "\""; 
      jsonString +=  ",\"Gas_Delivered2\":\""    + String(monthsDat[slot12].GasDelivered, 2)    + "\"";
      jsonString += "}";
    }
    sKomma      = ",";
  }
  jsonString   += "]";

  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendTableMonths(): Send data\r");

} // sendTableMonths()

//=======================================================================
void sendTableWeek() {
//=======================================================================
  int8_t  slot, prevSlot;
  String sKomma = "";
  
  jsonString    = "[";
  for ( int16_t r = 6; r >= 0; r-- ) {
    slot = r + thisWeekDay;
    if (slot > 6) slot -= 7;
    prevSlot = slot - 1;
    if (prevSlot < 0) prevSlot = 6; 
    if (Verbose) TelnetStream.printf("thisWeekDay[%d], r[%d] => prevSlot[%d], slot[%d]\r\n"
                                    , thisWeekDay,     r,       prevSlot,     slot);
    if (r != 0) {
      jsonString +=  sKomma + "{\"WeekDay\":\"" + getDayName(slot) + "\""; 
      jsonString +=  ",\"Energy_Delivered\":\"" + String((weekDat[slot].EnergyDelivered - weekDat[prevSlot].EnergyDelivered), 3) + "\"";
      jsonString +=  ",\"Energy_Returned\":\"" + String((weekDat[slot].EnergyReturned - weekDat[prevSlot].EnergyReturned), 3) + "\"";
      jsonString +=  ",\"Gas_Delivered\":\"" + String((weekDat[slot].GasDelivered - weekDat[prevSlot].GasDelivered), 2) + "\"";
    } else {
      jsonString +=  sKomma + "{\"WeekDay\":\"Tot en met " + getDayName(slot) + "\""; 
      jsonString +=  ",\"Energy_Delivered\":\"" + String(weekDat[slot].EnergyDelivered, 3) + "\"";
      jsonString +=  ",\"Energy_Returned\":\"" + String(weekDat[slot].EnergyReturned, 3) + "\"";
      jsonString +=  ",\"Gas_Delivered\":\"" + String(weekDat[slot].GasDelivered, 2) + "\"";
    }
    jsonString += "}";
    sKomma      = ",";
  }
  jsonString   += "]";
//TelnetStream.println(String(jsonString));
  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendTableWeek(): Send data\r");

} // sendTableWeek()


//=======================================================================
void sendTableHours() {
//=======================================================================
  String  sKomma = "";
  char    cHour[30];
  int8_t  thisPrev, prevSlot, thisSlot, slot, thisMinute;

  
  jsonString    = "[";
  for ( int16_t r = 8; r >= 1; r-- ) {
    int8_t thisNext;
    hourToSlot(thisHour, thisSlot, thisNext, thisPrev);
    slot = r + thisSlot;  // I want to start with this hour
    if (slot > 8) slot -= 8;
    prevSlot = r + thisPrev;
    if (prevSlot > 8)       prevSlot -= 8;

    switch(slot) {
      case 1: sprintf(cHour, "00:00 - 02:59"); break;
      case 2: sprintf(cHour, "03:00 - 05:59"); break;
      case 3: sprintf(cHour, "06:00 - 08:59"); break;
      case 4: sprintf(cHour, "09:00 - 11:59"); break;
      case 5: sprintf(cHour, "12:00 - 14:59"); break;
      case 6: sprintf(cHour, "15:00 - 17:59"); break;
      case 7: sprintf(cHour, "18:00 - 20:59"); break;
      case 8: sprintf(cHour, "21:00 - 23:59"); break;
    }

    if (Verbose) TelnetStream.printf("sendTableHours(): r[%02d], thisHour[%02d], slot[%02d], prevSlot[%02d] \r\n"
                                                      , r, thisHour, slot, prevSlot);
    
    if (r != 1) {
      jsonString +=  sKomma + "{\"Hour\":\"" + cHour + "\""; 
      jsonString +=  ",\"Energy_Delivered\":\"" + String(((hoursDat[slot].EnergyDelivered - hoursDat[prevSlot].EnergyDelivered) * 1000.0), 0) + "\"";
      jsonString +=  ",\"Energy_Returned\":\""  + String(((hoursDat[slot].EnergyReturned  - hoursDat[prevSlot].EnergyReturned) * 1000.0), 0) + "\"";
      jsonString +=  ",\"Gas_Delivered\":\""    + String(( hoursDat[slot].GasDelivered    - hoursDat[prevSlot].GasDelivered), 2) + "\"";
      jsonString += "}";
    } else {
      jsonString +=  sKomma + "{\"Hour\":\"Tot en met " + String(cHour).substring(7) + "\""; 
      jsonString +=  ",\"Energy_Delivered\":\"" + String(hoursDat[slot].EnergyDelivered, 3) + " (kWh)\"";
      jsonString +=  ",\"Energy_Returned\":\""  + String(hoursDat[slot].EnergyReturned, 3)  + " (kWh)\"";
      jsonString +=  ",\"Gas_Delivered\":\""    + String(hoursDat[slot].GasDelivered, 2)    + "\"";
      jsonString += "}";
    }
    sKomma      = ",";
  }
  jsonString   += "]";

  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendTableHours(): Send data\r");

} // sendTableHours()


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
