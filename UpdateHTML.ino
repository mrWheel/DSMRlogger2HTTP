/*
***************************************************************************  
**  Program  : UpdateHTML, part of DSMRlogger2HTTM
**  Version  : v4.0
**
**  Copyright (c) 2018 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//=======================================================================
void sendDataMeterInfo() {

//-Meter Info----------------------------------------------------------
  jsonString  = "{";
  jsonString += " \"Identification\":\"" + String(Identification) + "\"";
  jsonString += ",\"P1_Version\":\"" + String(P1_Version) + "\"";
  jsonString += ",\"Equipment_Id\":\"" + String(Equipment_Id) + "\"";
  jsonString += ",\"Electricity_Tariff\":\"" + String(Electricity_Tariff) + "\"";
  jsonString += ",\"Gas_Device_Type\":\"" + String(Gas_Device_Type) + "\"";
  jsonString += ",\"Gas_Equipment_Id\":\"" + String(Gas_Equipment_Id) + "\"";
  jsonString += ",\"StatusLong\":\"" + lastStartup + ",  telegrams Processed: <b>" + String(telegramCount) + "</b>\"";
  for(int l = 0; l < NUMLASTLOG; l++) {
    jsonString += ",\"lastLogLine" + String((l+1)) + "\":\"[" + String(l) + "]lastLogLine: <b>" + String(lastLogLine[l]) + "</b>\"";
  }
  jsonString += "}";
  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendDataMeterInfo(): send JSON string\r\n");

} // sendDataMeterInfo()

//=======================================================================
void sendDataActual() {

#ifdef HASS_NO_METER
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
  jsonString += ",\"EnergyDelivered\":\"" + String(EnergyDelivered, 3) + "\"";
  jsonString += ",\"EnergyReturned\":\"" + String(EnergyReturned, 3) + "\"";
  jsonString += ",\"GasDelivered\":\"" + String(GasDelivered, 2) + "\"";
  jsonString += ",\"Energy_Delivered_Tariff1\":\"" + String(Energy_Delivered_Tariff1, 3) + "\"";
  jsonString += ",\"Energy_Delivered_Tariff2\":\"" + String(Energy_Delivered_Tariff2, 3) + "\"";
  jsonString += ",\"Energy_Returned_Tariff1\":\"" + String(Energy_Returned_Tariff1, 3) + "\"";
  jsonString += ",\"Energy_Returned_Tariff2\":\"" + String(Energy_Returned_Tariff2, 3) + "\"";
  jsonString += ",\"Power_Delivered\":\"" + String(PowerDelivered, 3) + "\"";
  jsonString += ",\"Power_Returned\":\"" + String(PowerReturned, 3) + "\"";
  jsonString += ",\"Voltage_l1\":\"" + String(Voltage_l1, 1) + "\"";
  jsonString += ",\"Current_l1\":\"" + String(Current_l1, 1) + "\"";
  jsonString += ",\"Voltage_l2\":\"" + String(Voltage_l2, 1) + "\"";
  jsonString += ",\"Current_l2\":\"" + String(Current_l2, 1) + "\"";
  jsonString += ",\"Voltage_l3\":\"" + String(Voltage_l3, 1) + "\"";
  jsonString += ",\"Current_l3\":\"" + String(Current_l3, 1) + "\"";
  jsonString += ",\"Power_Delivered_l1\":\"" + String(Power_Delivered_l1, 3) + "\"";
  jsonString += ",\"Power_Returned_l1\":\"" + String(Power_Returned_l1, 3) + "\"";
  jsonString += ",\"Power_Delivered_l2\":\"" + String(Power_Delivered_l2, 3) + "\"";
  jsonString += ",\"Power_Returned_l2\":\"" + String(Power_Returned_l2, 3) + "\"";
  jsonString += ",\"Power_Delivered_l3\":\"" + String(Power_Delivered_l3, 3) + "\"";
  jsonString += ",\"Power_Returned_l3\":\"" + String(Power_Returned_l3, 3) + "\"";
  jsonString += ",\"StatusLong\":\"" + lastStartup + ",  telegrams Processed: <b>" + String(telegramCount) + "</b>\"";
  for(int l = 0; l < NUMLASTLOG; l++) {
    jsonString += ",\"lastLogLine" + String((l+1)) + "\":\"[" + String(l) + "]lastLogLine: <b>" + String(lastLogLine[l]) + "</b>\"";
  }
  jsonString += "}";
  server.send(200, "application/json", jsonString);
  TelnetStream.println("sendDataActual(): send JSON string\r\n");

} // sendDataActual()

//=======================================================================
void sendTableWeek() {
  int8_t  slot, prevSlot;
  String sKomma = "";
  
  jsonString    = "[";
  for ( int16_t r = 6; r >= 0; r-- ) {
    slot = r + thisWeekDay;
    if (slot > 6) slot -= 7;
    prevSlot = slot - 1;
    if (prevSlot < 0) prevSlot = 6; 
  //TelnetStream.printf("thisWeekDay[%d], r[%d] => prevSlot[%d], slot[%d]\r\n", thisWeekDay, r, prevSlot, slot);
    if (r != 0) {
      jsonString +=  sKomma + "{\"WeekDay\":\"" + weekDayName(slot) + "\""; 
      jsonString +=  ",\"EnergyDelivered\":\"" + String((weekDayDat[slot].EnergyDelivered - weekDayDat[prevSlot].EnergyDelivered), 3) + "\"";
      jsonString +=  ",\"EnergyReturned\":\"" + String((weekDayDat[slot].EnergyReturned - weekDayDat[prevSlot].EnergyReturned), 3) + "\"";
      jsonString +=  ",\"GasDelivered\":\"" + String((weekDayDat[slot].GasDelivered - weekDayDat[prevSlot].GasDelivered), 2) + "\"";
    } else {
      jsonString +=  sKomma + "{\"WeekDay\":\"Up Till " + weekDayName(slot) + "\""; 
      jsonString +=  ",\"EnergyDelivered\":\"" + String(weekDayDat[slot].EnergyDelivered, 3) + "\"";
      jsonString +=  ",\"EnergyReturned\":\"" + String(weekDayDat[slot].EnergyReturned, 3) + "\"";
      jsonString +=  ",\"GasDelivered\":\"" + String(weekDayDat[slot].GasDelivered, 2) + "\"";
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
  String  sKomma = "";
  char    cHour[10];
  int8_t  prevSlot, slot, thisMinute;

  
  jsonString    = "[";
  for ( int16_t r = 24; r >= 1; r-- ) {
    slot = r + thisHour + 1;  // I want to start with the next hour
    if (slot > 23) slot -= 24;
    if (r == 24) {
      thisMinute = MinuteFromTimestamp(pTimestamp);
      sprintf(cHour, "%02d:%02d", thisHour, thisMinute);
    } else {
      thisMinute = 0;
      sprintf(cHour, "%02d:%02d", slot, thisMinute);
    }
    prevSlot = slot - 1;
    if (prevSlot < 0) prevSlot = 23; 
    if (Verbose) TelnetStream.printf("r[%02d], thisHour[%02d], slot[%02d], prevSlot[%02d] \r\n", r, thisHour, slot, prevSlot);
    if (r != 1) {
      jsonString +=  sKomma + "{\"Hour\":\"" + cHour + "\""; 
      jsonString +=  ",\"EnergyDelivered\":\"" + String(((hoursDat[slot].EnergyDelivered - hoursDat[prevSlot].EnergyDelivered) * 1000.0), 0) + "\"";
      jsonString +=  ",\"EnergyReturned\":\""  + String(((hoursDat[slot].EnergyReturned  - hoursDat[prevSlot].EnergyReturned) * 1000.0), 0) + "\"";
      jsonString +=  ",\"GasDelivered\":\""    + String(( hoursDat[slot].GasDelivered    - hoursDat[prevSlot].GasDelivered), 2) + "\"";
      jsonString += "}";
    } else {
      jsonString +=  sKomma + "{\"Hour\":\"Up Till " + cHour + "\""; 
      jsonString +=  ",\"EnergyDelivered\":\"" + String(hoursDat[slot].EnergyDelivered, 3) + " (kWh)\"";
      jsonString +=  ",\"EnergyReturned\":\""  + String(hoursDat[slot].EnergyReturned, 3)  + " (kWh)\"";
      jsonString +=  ",\"GasDelivered\":\""    + String(hoursDat[slot].GasDelivered, 2)    + "\"";
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
