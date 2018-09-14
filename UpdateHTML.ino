/*
***************************************************************************  
**  Program  : UpdateHTML, part of DSMRlogger2HTTM
**  Version  : v4.1
**
**  Copyright (c) 2018 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//=======================================================================
void sendDataMeterInfo() {
//=======================================================================

//-Meter Info----------------------------------------------------------
  jsonString  = "{";
  jsonString += " \"Identification\":\"" + String(Identification) + "\"";
  jsonString += ",\"P1_Version\":\"" + String(P1_Version) + "\"";
  jsonString += ",\"Equipment_Id\":\"" + String(Equipment_Id) + "\"";
  jsonString += ",\"Electricity_Tariff\":\"" + String(Electricity_Tariff) + "\"";
  jsonString += ",\"Gas_Device_Type\":\"" + String(Gas_Device_Type) + "\"";
  jsonString += ",\"Gas_Equipment_Id\":\"" + String(Gas_Equipment_Id) + "\"";
  jsonString += ",\"SWversion\":\"" + String(_SW_VERSION) +"\"" ;
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
//=======================================================================

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
void sendTableMonths() {
//=======================================================================
  String  sKomma = "";
  char    cYearMonth[30];
  int8_t  nextSlot, slot;
  
  jsonString    = "[";
  for ( int16_t slot = 1; slot <= 24; slot++ ) {
    nextSlot = slot + 1;
    if (nextSlot > 23) nextSlot = 23; 
    if (Verbose) TelnetStream.printf("sendTableMonths(): slotMonth[%02d], slot[%02d], nextSlot[%02d] \r\n"
                                                       , (monthsDat[slot].Label % 100), slot, nextSlot);

    sprintf(cYearMonth, "20%02d-%02d", (monthsDat[slot].Label / 100), (monthsDat[slot].Label % 100));
    
    if (slot < 24) {
      jsonString +=  sKomma + "{\"YearMonth\":\"" + cYearMonth + "\""; 
      jsonString +=  ",\"EnergyDelivered\":\"" + String(( monthsDat[slot].EnergyDelivered - monthsDat[nextSlot].EnergyDelivered), 3) + "\"";
      jsonString +=  ",\"EnergyReturned\":\""  + String(( monthsDat[slot].EnergyReturned  - monthsDat[nextSlot].EnergyReturned), 3) + "\"";
      jsonString +=  ",\"GasDelivered\":\""    + String(( monthsDat[slot].GasDelivered    - monthsDat[nextSlot].GasDelivered), 2) + "\"";
      jsonString += "}";
    } else {
      jsonString +=  sKomma + "{\"YearMonth\":\"Up Till " + cYearMonth + "\""; 
      jsonString +=  ",\"EnergyDelivered\":\"" + String(monthsDat[slot].EnergyDelivered, 3) + "\"";
      jsonString +=  ",\"EnergyReturned\":\""  + String(monthsDat[slot].EnergyReturned, 3)  + "\"";
      jsonString +=  ",\"GasDelivered\":\""    + String(monthsDat[slot].GasDelivered, 2)    + "\"";
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
      jsonString +=  ",\"EnergyDelivered\":\"" + String(((hoursDat[slot].EnergyDelivered - hoursDat[prevSlot].EnergyDelivered) * 1000.0), 0) + "\"";
      jsonString +=  ",\"EnergyReturned\":\""  + String(((hoursDat[slot].EnergyReturned  - hoursDat[prevSlot].EnergyReturned) * 1000.0), 0) + "\"";
      jsonString +=  ",\"GasDelivered\":\""    + String(( hoursDat[slot].GasDelivered    - hoursDat[prevSlot].GasDelivered), 2) + "\"";
      jsonString += "}";
    } else {
      jsonString +=  sKomma + "{\"Hour\":\"Up Till " + String(cHour).substring(7) + "\""; 
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
