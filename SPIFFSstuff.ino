/*
***************************************************************************  
**  Program  : SPIFFSstuff, part of DSMRlogger2HTTP
**  Version  : v0.7.7
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//===========================================================================================
int32_t freeSpace() {
//===========================================================================================
  int32_t space;
  
  SPIFFS.info(SPIFFSinfo);

  space = (int32_t)(SPIFFSinfo.totalBytes - SPIFFSinfo.usedBytes);

  return space;
  
} // freeSpace()

//===========================================================================================
void listSPIFFS() {
//===========================================================================================
  Dir dir = SPIFFS.openDir("/");

  TelnetStream.println();
  while (dir.next()) {
    File f = dir.openFile("r");
    TelnetStream.printf("%-15s %ld \r\n", dir.fileName().c_str(), f.size());
    yield();
  }
  TelnetStream.flush();

  SPIFFS.info(SPIFFSinfo);

  int32_t space = (int32_t)(SPIFFSinfo.totalBytes - SPIFFSinfo.usedBytes);
  TelnetStream.println("\r");
  TelnetStream.printf("Available SPIFFS space [%ld]bytes\r\n", freeSpace());
  TelnetStream.printf("           SPIFFS Size [%ld]kB\r\n", (SPIFFSinfo.totalBytes / 1024));
  TelnetStream.printf("     SPIFFS block Size [%ld]bytes\r\n", SPIFFSinfo.blockSize);
  TelnetStream.printf("      SPIFFS page Size [%ld]bytes\r\n", SPIFFSinfo.pageSize);
  TelnetStream.printf(" SPIFFS max.Open Files [%ld]\r\n\n", SPIFFSinfo.maxOpenFiles);


} // listSPIFFS()


//===========================================================================================
void saveHourData(int8_t slot) {
//===========================================================================================

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
    
  sprintf(cMsg, "saveHourData(%02d) ...", slot);
  TelnetStream.print(cMsg);
  TelnetStream.flush();
  Serial.print(cMsg);
  displayHoursHist(false);    // log to file

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return;
  }

  if (slot >= 1 && slot <= 8) {
    if (    (hoursDat[1].EnergyDelivered == 0.0) 
         && (hoursDat[1].EnergyReturned  == 0.0)
         && (hoursDat[1].GasDelivered    == 0.0)) {
      TelnetStream.println("saveHourData(): Error? (Zero values)!!\r");
      TelnetStream.flush();
      return;
    }
}

  File hoursFile = SPIFFS.open(HOURS_FILE, "w");

  sprintf(cMsg, HOURS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();
  TelnetStream.printf("Record Length is [%d]\n", recLength);

  hoursFile.print(cMsg);  // write CSV Header
    
  yield();
  for (int8_t r = 1; r <= 8; r++) {
    sprintf(cMsg, "%02d; %10s; %10s; %10s;\n", r, String(hoursDat[r].EnergyDelivered, 3).c_str()
                                                , String(hoursDat[r].EnergyReturned, 3).c_str()
                                                , String(hoursDat[r].GasDelivered, 2).c_str());
    if (r == slot) {
      TelnetStream.printf("saveHourData(%02d): => %s\r\n", r, cMsg);
      Serial.printf("saveHourData(%02d): => %s\r\n", r, cMsg);
    }
    hoursFile.print(cMsg);
  }

  hoursFile.close();  

  TelnetStream.println("saveHourData(): Done\r");
  TelnetStream.flush();

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

} // saveHourData()


//===========================================================================================
bool readHourData() {
//===========================================================================================
  int8_t  r=0;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.print("readHourData() ... ");
  TelnetStream.flush();
  Serial.print("readHourData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return true;
  }

  sprintf(cMsg, HOURS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();

  File hoursFile = SPIFFS.open(HOURS_FILE, "r");
  TelnetStream.printf("%s: size(%d) ", HOURS_FILE, hoursFile.size());
  TelnetStream.flush();
  Serial.printf("%s: size(%d) \r\n", HOURS_FILE, hoursFile.size());

  if (hoursFile.size() == 0) return false;

  String header = hoursFile.readStringUntil('\n');  // read CSV Header (rec 0)

  r = 1;
//while(hoursFile.available() && (r <= 8)) {        // read data (records 1 till 8)
  for(r=1; r <= 8; r++) {
      yield();
      if (Verbose)  TelnetStream.printf(" %d", r);
      else          TelnetStream.print("h");
      TelnetStream.flush();
      if (hoursFile.available() > 0) {
        hoursDat[r].Label            = (int)hoursFile.readStringUntil(';').toInt();
        hoursDat[r].EnergyDelivered  = (float)hoursFile.readStringUntil(';').toFloat();
        hoursDat[r].EnergyReturned   = (float)hoursFile.readStringUntil(';').toFloat();
        hoursDat[r].GasDelivered     = (float)hoursFile.readStringUntil(';').toFloat();
        String n = hoursFile.readStringUntil('\n');
      }
      delay(10);
  }
  TelnetStream.print("  ");

  hoursFile.close();  
  
  if (hoursDat[8].Label != 8) {
    TelnetStream.printf("readHourData(): ERROR!! ==> read [%d] records\r\n", --r);
    for (r=1; r<=8; r++) {
      TelnetStream.printf("Label[%d] has value [%d]\r\n", r, hoursDat[r].Label);
      TelnetStream.flush();
      Serial.printf("Label[%d] has value [%d]\r\n", r, hoursDat[r].Label);
    }
    return false;
  }

  TelnetStream.println("readHourData(): Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // readHourData()


//===========================================================================================
void saveWeekData() {
//===========================================================================================

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.print("saveWeekData() ... ");
  TelnetStream.flush();
  Serial.print("saveWeekData(s) ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return;
  }

  displayDaysHist(false);   // write to log-file

  File weekFile = SPIFFS.open(WEEKDAY_FILE, "w");
    
  for (int16_t r=0; r < 7; r++) {
    yield();
    if (Verbose) TelnetStream.printf(" %d", r);
    TelnetStream.flush();
    weekFile.print(r);  // Label
    weekFile.print(";");
    weekFile.print((float)weekDat[r].EnergyDelivered);
    weekFile.print(";");
    weekFile.print((float)weekDat[r].EnergyReturned);
    weekFile.print(";");
    weekFile.print((float)weekDat[r].GasDelivered);
    weekFile.println(";");
  //delay(10);
  }

  weekFile.close();  

  TelnetStream.println("saveWeekData(): Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  
} // saveWeekData()

//===========================================================================================
bool readWeekData() {
//===========================================================================================
  int8_t  r=0;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.print("readWeekData() ... ");
  TelnetStream.flush();
  Serial.print("readWeekData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return true;
  }

  File weekFile = SPIFFS.open(WEEKDAY_FILE, "r");
  TelnetStream.printf("%s: size(%d) ", WEEKDAY_FILE, weekFile.size());
  TelnetStream.flush();
  Serial.printf("%s: size(%d) \r\n", WEEKDAY_FILE, weekFile.size());

  if (weekFile.size() == 0) return false;

  for(r = 0; r<=6; r++) {
      yield();
      if (Verbose)  TelnetStream.printf(" %d", r);
      else          TelnetStream.print("w");
      TelnetStream.flush();
      if (weekFile.available() > 0) {
        weekDat[r].Label            = (int)weekFile.readStringUntil(';').toInt();
        weekDat[r].EnergyDelivered  = (float)weekFile.readStringUntil(';').toFloat();
        weekDat[r].EnergyReturned   = (float)weekFile.readStringUntil(';').toFloat();
        weekDat[r].GasDelivered     = (float)weekFile.readStringUntil(';').toFloat();
        String n = weekFile.readStringUntil('\n');
      }
      delay(10);
      //r++;
  }
  TelnetStream.print("  ");
  
  weekFile.close();  

  if (weekDat[6].Label != 6) {
    TelnetStream.printf("readWeekData(): ERROR!! ==> read [%d] records\r\n", r);
    TelnetStream.flush();
    return false;
  }

  TelnetStream.println("readWeekDatData(): Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // readWeekData()


//===========================================================================================
void saveThisMonth(int8_t thisYear, int8_t thisMonth, bool force) {
//===========================================================================================
  bool   errorVal = false;
  int8_t sltYear  = thisYear;
  int8_t sltMonth = thisMonth;
  File   monthsFile;
  
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  sprintf(cMsg, "saveThisMonth(20%02d-%02d) acceptZeroVal[%d]... \r\n", thisYear, thisMonth, force);
  TelnetStream.print(cMsg);
  TelnetStream.flush();
  Serial.print(cMsg);
  displayMonthsHist(false);    // log to file

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return;
  }
  
  if (!force) {  
    if (    (monthsDat[1].EnergyDelivered == 0.0) 
         && (monthsDat[1].EnergyReturned  == 0.0)
         && (monthsDat[1].GasDelivered    == 0.0)) {
      TelnetStream.println("saveThisMonth(): Error? (Zero value)!!\r");
      TelnetStream.flush();
      return;
    }
  }

  sprintf(cMsg, MONTHS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();
  TelnetStream.printf("Record Length is [%d]\n", recLength);
  TelnetStream.flush();
  Serial.printf("Record Length is [%d]\n", recLength);

  if (!SPIFFS.exists(MONTHS_FILE)) {
    TelnetStream.println("saveThisMonth(): create empty file");
    TelnetStream.flush();
    
    monthsFile = SPIFFS.open(MONTHS_FILE, "w");
    monthsFile.print(cMsg);
    monthsFile.close();  
  }

  monthsFile = SPIFFS.open(MONTHS_FILE, "r+");
    
  yield();
  sprintf(cMsg, "%02d%02d; %12s; %12s; %10s;\n", thisYear, thisMonth
                                           , String(monthsDat[1].EnergyDelivered, 3).c_str()
                                           , String(monthsDat[1].EnergyReturned, 3).c_str()
                                           , String(monthsDat[1].GasDelivered, 2).c_str());
                                           
  if (Verbose) TelnetStream.printf("saveThisMonth(): slot[01] => %s\r", cMsg);
  TelnetStream.flush();
  Serial.printf("saveThisMonth(): slot[01] => %s\r", cMsg);
  
  monthsFile.seek(recLength, SeekSet);
  monthsFile.print(cMsg);

  monthsFile.close();  

  TelnetStream.println("saveThisMonth(): Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

} // saveThisMonth()


//===========================================================================================
bool readMonthData() {
//===========================================================================================
  int8_t  r=1;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  
  TelnetStream.println("readMonthData() ... \r");
  TelnetStream.flush();
  Serial.println("readMonthData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return false;
  }

  sprintf(cMsg, MONTHS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();

  File monthsFile = SPIFFS.open(MONTHS_FILE, "r");
  TelnetStream.printf("%s: size(%d) ", MONTHS_FILE, monthsFile.size());
  TelnetStream.flush();
  Serial.printf("%s: size(%d) \r\n", MONTHS_FILE, monthsFile.size());

  if (monthsFile.size() == 0) return false;

//-- seek() gives strange results ..  
//monthsFile.seek(recLength, SeekSet); // skip header
  String mHeader = monthsFile.readStringUntil('\n');  // skip header
  
  for(r = 1; r <= 24; r++) {
      yield();
      if (monthsFile.available() > 0) {
        monthsDat[r].Label            = (int)monthsFile.readStringUntil(';').toInt();
        monthsDat[r].EnergyDelivered  = (float)monthsFile.readStringUntil(';').toFloat();
        monthsDat[r].EnergyReturned   = (float)monthsFile.readStringUntil(';').toFloat();
        monthsDat[r].GasDelivered     = (float)monthsFile.readStringUntil(';').toFloat();
        String n = monthsFile.readStringUntil('\n');
      }
      if (Verbose) TelnetStream.printf("recNo[%02d] Label[%04d], ED[%s], ER[%s], GD[%s]\r\n"
                                        , r, monthsDat[r].Label
                                        ,    String(monthsDat[r].EnergyDelivered, 3).c_str()
                                        ,    String(monthsDat[r].EnergyReturned, 3).c_str()
                                        ,    String(monthsDat[r].GasDelivered, 2).c_str() );
      else TelnetStream.print("m");
      TelnetStream.flush();
      delay(10);
  }
  
  monthsFile.close();  

  TelnetStream.println("readMonthData(): Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // readMonthData()


//===========================================================================================
int8_t getLastMonth() {
//===========================================================================================
  int16_t yearMonth, lastMonth;
  
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  TelnetStream.print("getLastMonth() ... ");
  TelnetStream.flush();
  Serial.print("getLastMonth() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return -1;
  }

  sprintf(cMsg, MONTHS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();

  File monthsFile = SPIFFS.open(MONTHS_FILE, "r");
//-- seek() gives strange results ..  
//monthsFile.seek(recLength, SeekSet); // skip header
  String mHeader = monthsFile.readStringUntil('\n');  // skip header record

  TelnetStream.printf("getLastMonth(): Header[%s]\r\n", mHeader.c_str());
  TelnetStream.flush();

  yearMonth = (int)monthsFile.readStringUntil(';').toInt();
  monthsFile.close();
  
  TelnetStream.printf("getLastMonth(): yearMonth[20%04d]\r\n", yearMonth);
  TelnetStream.flush();
  lastMonth = (yearMonth % 100);
  
  sprintf(cMsg, "getLastMonth() ... ==> [%02d]", lastMonth);
  TelnetStream.println(cMsg);
  TelnetStream.flush();
  Serial.println(cMsg);
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return lastMonth;

} // getLastMonth()



//===========================================================================================
int8_t getLastYear() {
//===========================================================================================
  int16_t yearMonth, lastYear;
  
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  TelnetStream.print("getLastYear() ... ");
  TelnetStream.flush();
  Serial.print("getLastYear() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return -1;
  }

  sprintf(cMsg, MONTHS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();

  File monthsFile = SPIFFS.open(MONTHS_FILE, "r");
//-- seek() gives strange results ..  
//monthsFile.seek(recLength, SeekSet); // skip header
  String mHeader = monthsFile.readStringUntil('\n');  // skip header record

  yearMonth = (int)monthsFile.readStringUntil(';').toInt();
  monthsFile.close();
  
  lastYear = (yearMonth / 100);
  
  sprintf(cMsg, "getLastYear() ... ==> [20%02d]", lastYear);
  TelnetStream.println(cMsg);
  TelnetStream.flush();
  Serial.println(cMsg);
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return lastYear;

} // getLastYear()


//===========================================================================================
void shiftDownMonthData(int8_t thisYear, int8_t thisMonth) {
//===========================================================================================
  uint8_t recNo = 0;
  File   monthsFile;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.printf("shiftDownMonthData(20%02d-%02d) ... \n", thisYear, thisMonth);
  TelnetStream.flush();
  Serial.printf("shiftDownMonthData(20%02d-%02d) ... \n", thisYear, thisMonth);
  displayMonthsHist(false);   // log to file
  displayMonthsHist(true);    // log to TelnetStream

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    TelnetStream.flush();
    return;
  }

  monthsFile = SPIFFS.open(MONTHS_FILE, "w");

  //--- write header -----
  sprintf(cMsg, MONTHS_CSV_HEADER);
  monthsFile.print(cMsg);
  if (Verbose) TelnetStream.printf("recNo[%02d] => tab[00] %s", recNo, cMsg);
  TelnetStream.flush();
  Serial.printf("recNo[%02d] => tab[00] %s", recNo, cMsg);
  recNo++;
  
  int8_t recLength = String(cMsg).length();
  if (Verbose) TelnetStream.printf("Record Length is [%d]\n", recLength);
  TelnetStream.flush();

  //--- write first record ---------
  sprintf(cMsg, "%02d%02d; %12s; %12s; %10s;\n", thisYear, thisMonth
                                               , String(0.0, 3).c_str()
                                               , String(0.0, 3).c_str()
                                               , String(0.0, 2).c_str());
  monthsFile.print(cMsg);
  if (Verbose) TelnetStream.printf("recNo[%02d] => tab[01] %s\r", recNo, cMsg);
  TelnetStream.flush();
  Serial.printf("recNo[%02d] => tab[01] %s", recNo, cMsg);
  recNo++;
  //--- write second, third etc. record ---------
  for (int8_t ym=1; ym<=23; ym++) {
      sprintf(cMsg, "%04d; %12s; %12s; %10s;\n", monthsDat[ym].Label
                                               , String(monthsDat[ym].EnergyDelivered, 3).c_str()
                                               , String(monthsDat[ym].EnergyReturned, 3).c_str()
                                               , String(monthsDat[ym].GasDelivered, 2).c_str());
      monthsFile.print(cMsg);
      if (Verbose) TelnetStream.printf("recNo[%02d] => tab[%02d] %s\r", recNo, ym, cMsg);
      TelnetStream.flush();
      Serial.printf("recNo[%02d] => tab[%02d] %s\n", recNo, ym, cMsg);
      recNo++;
  }
  
  monthsFile.close();  

  if (!readMonthData()) TelnetStream.println("shiftDownMonthData(): error readMonthData()!\r");
  TelnetStream.flush();

  TelnetStream.println("shiftDownMonthData(): Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

} // shiftDownMonthData()


#ifdef HAS_NO_METER
//===========================================================================================
void createDummyData() {
//===========================================================================================

  TelnetStream.println("createDummyData() ==> hoursDat[].. \r");
  TelnetStream.flush();
  for (int s=1; s<=8; s++) {
    hoursDat[s].Label             = s;
    hoursDat[s].EnergyDelivered   = 0.0;
    hoursDat[s].EnergyReturned    = 0.0;
    hoursDat[s].GasDelivered      = 0.0;
  }
  saveHourData(9);
  
  TelnetStream.println("createDummyData() ==> weekDat[].. \r");
  TelnetStream.flush();
  for (int s=0; s<=6; s++) {
    weekDat[s].Label           = s;
    weekDat[s].EnergyDelivered = 0.0;
    weekDat[s].EnergyReturned  = 0.0;
    weekDat[s].GasDelivered    = 0.0;
  }
  saveWeekData();
  
  TelnetStream.println("createDummyData() ==> monthsDat[].. \r");
  TelnetStream.flush();
  
  File monthsFile = SPIFFS.open(MONTHS_FILE, "w");
  //--- write header -----
  sprintf(cMsg, MONTHS_CSV_HEADER);
  monthsFile.print(cMsg);

  //--- write zeroes to file ---------
  char cLabel[10];
  for (int s=1; s<=24; s++) {
    if (s>12) {   
      sprintf(cLabel, "15%02d", (25 - s));
    } else {
      sprintf(cLabel, "16%02d", (13 - s));
    }
    sprintf(cMsg, "%04d; %12s; %12s; %10s;\n", String(cLabel).toInt()
                                             , String(0.0, 3).c_str()
                                             , String(0.0, 3).c_str()
                                             , String(0.0, 2).c_str());
      monthsFile.print(cMsg);
  }

  monthsFile.close();  

  if (!readMonthData()) TelnetStream.println("createDummyData():(): error readMonthData()!\r");
  TelnetStream.flush();
  displayMonthsHist(true);  // to TelnetStream

  thisYear  = getLastYear();
  thisMonth = getLastMonth();
  
  TelnetStream.println("createDummyData(): Done\r");
  TelnetStream.flush();

} // createDummyData()
#endif

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
