/*
***************************************************************************  
**  Program  : SPIFFSstuff, part of DSMRlogger2HTTP
**  Version  : v5.2
**
**  Copyright (c) 2018 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//===========================================================================================
uint16_t freeSpace() {
//===========================================================================================
  int32_t space;
  
  SPIFFS.info(SPIFFSinfo);

  space = (uint16_t)(SPIFFSinfo.totalBytes - SPIFFSinfo.usedBytes);

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
} // listSPIFFS()

//===========================================================================================
void readLogFile() {
//===========================================================================================
    String  logFileName;
    char    charF = '\0';
    char    SDbuffer[100];
    uint8_t bSize;

    if (!SPIFFSmounted) {
      TelnetStream.println("No SPIFFS filesystem..");
      return;
    }
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    for(int i=0; i<=1; i++) {
      if (i==0) logFileName = LOG_FILE_R;
      else      logFileName = LOG_FILE;

      if (!SPIFFS.exists(logFileName)) {
        continue;
      }
      File logFile = SPIFFS.open(logFileName, "r");

      bSize = 0;
      memset(SDbuffer,0,sizeof(SDbuffer));
      while (logFile.available()) {
        yield();
        charF = (char)logFile.read();
        if (charF == '\n' || charF == '\r' || charF == '\0' || bSize > 98) {
            if (bSize > 0) {
                TelnetStream.println(SDbuffer);
            }
            bSize = 0;
            memset(SDbuffer,0,sizeof(SDbuffer));
        }
        else {
            if (charF >= 32 && charF <= 126) {
                SDbuffer[bSize++] = (char)charF;
            }
        }
        yield();
      }
      
      logFile.close();
    
    } // for i ..
    
    TelnetStream.println("\n===END===\r");

}   // readLogFile()

//===========================================================================================
bool writeLogFile(String logLine) {
//===========================================================================================

    bool newLogFile;

    if (!SPIFFSmounted) {
      TelnetStream.println("No SPIFFS filesystem..");
      return false;
    }

    if (freeSpace() < 1000) {
      rotateLogFile("rotateLogFile due to space");
    }
    
    if (SPIFFS.exists(LOG_FILE)) {
        newLogFile = false;
    } else {
        TelnetStream.println("writeLogFile(): new logfile\r");
        newLogFile = true;
    }

    if (logLine.length() > 4) {
      for(int l = (NUMLASTLOG - 1); l > 0; l--) {
        lastLogLine[l] = lastLogLine[l - 1];
      }
      lastLogLine[0] = "-";
      if (pTimestamp.length() >= 10) {
        lastLogLine[0] = "[" + buildDateTimeString(pTimestamp) + "] " + logLine;
      } else {
        lastLogLine[0] = "[Reboot] " + logLine;
      }
    }
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File logFile = SPIFFS.open(LOG_FILE, "a");

    // if the file is available, write to it:
    if (logFile) {
      logFile.print(lastLogLine[0]);
      logFile.println("\r");
      logFile.close();
      lastLogLine[0].replace('\r', ' ');
      lastLogLine[0].replace('\n', ' ');
      return true;
    
    } else {
      lastLogLine[0].replace('\r', ' ');
      lastLogLine[0].replace('\n', ' ');
      return false;
    }

}   // writeLogFile()

//===========================================================================================
void rotateLogFile(String reason) {
//===========================================================================================

  if (SPIFFS.exists(LOG_FILE_R)) {
    SPIFFS.remove(LOG_FILE_R);
  }
  SPIFFS.rename(LOG_FILE, LOG_FILE_R);
  writeLogFile(reason);
  
} // rotateLogFile()


//===========================================================================================
bool saveHourData(int8_t slot) {
//===========================================================================================
  bool errorVal = false;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
    
  sprintf(cMsg, "saveHourData(%d) ...", slot);
  TelnetStream.print(cMsg);
  Serial.print(cMsg);
  writeLogFile(cMsg);
  displayHoursHist(false);    // log to file

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }
  
  if (slot >= 1 && slot <= 8) {
    if (    (hoursDat[slot].EnergyDelivered == 0.0) 
         || (hoursDat[slot].EnergyReturned == 0.0)
         || (hoursDat[slot].GasDelivered == 0.0)) {
      TelnetStream.println("saveHourData(): Error (Zero value)!!\r");
      TelnetStream.flush();
      return false;
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
      TelnetStream.print(cMsg);
      Serial.print(cMsg);
    }
    hoursFile.print(cMsg);
  }

  hoursFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // saveHourData()


//===========================================================================================
bool readHourData() {
//===========================================================================================
  int8_t  r=0;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.print("readHourData() ... ");
  Serial.print("readHourData() ... ");
  writeLogFile("readHourData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }

  sprintf(cMsg, HOURS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();

  File hoursFile = SPIFFS.open(HOURS_FILE, "r");
  TelnetStream.printf("%s: size(%d) ", HOURS_FILE, hoursFile.size());
  Serial.printf("%s: size(%d) \r\n", HOURS_FILE, hoursFile.size());

  String header = hoursFile.readStringUntil('\n');  // read CSV Header (rec 0)

  r = 1;
//while(hoursFile.available() && (r <= 8)) {        // read data (records 1 till 8)
  for(r=1; r <= 8; r++) {
      yield();
      if (Verbose) TelnetStream.printf(" %d", r);
      hoursDat[r].Label            = (int)hoursFile.readStringUntil(';').toInt();
      hoursDat[r].EnergyDelivered  = (float)hoursFile.readStringUntil(';').toFloat();
      hoursDat[r].EnergyReturned   = (float)hoursFile.readStringUntil(';').toFloat();
      hoursDat[r].GasDelivered     = (float)hoursFile.readStringUntil(';').toFloat();
      String n = hoursFile.readStringUntil('\n');
      delay(10);
      //r++;
  }
  
  hoursFile.close();  
  
  if (hoursDat[8].Label != 8) {
    TelnetStream.printf("readHourData(): ERROR!! ==> read [%d] records\r\n", --r);
    for (r=1; r<=8; r++) {
      TelnetStream.printf("Label[%d] has value [%d]\r\n", r, hoursDat[r].Label);
      Serial.printf("Label[%d] has value [%d]\r\n", r, hoursDat[r].Label);
    }
    TelnetStream.flush();
    writeLogFile("Error reading [hours.csv] ...");
    return false;
  }

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // readHourData()


//===========================================================================================
void saveWeekDayData() {
//===========================================================================================

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.print("saveWeekDayData(s) ... ");
  Serial.print("saveWeekDayData(s) ... ");
  writeLogFile("saveWeekDayData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return;
  }

  displayDaysHist(false);   // write to log-file

  File weekFile = SPIFFS.open(WEEKDAY_FILE, "w");
    
  for (int16_t r=0; r < 7; r++) {
    yield();
    if (Verbose) TelnetStream.printf(" %d", r);
    weekFile.print(r);  // Label
    weekFile.print(";");
    weekFile.print((float)weekDayDat[r].EnergyDelivered);
    weekFile.print(";");
    weekFile.print((float)weekDayDat[r].EnergyReturned);
    weekFile.print(";");
    weekFile.print((float)weekDayDat[r].GasDelivered);
    weekFile.println(";");
  //delay(10);
  }

  weekFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  
} // saveWeekDayData()

//===========================================================================================
bool readWeekDayData() {
//===========================================================================================
  int8_t  r=0;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.print("readWeekDayData() ... ");
  Serial.print("readWeekDayData() ... ");
  writeLogFile("readWeekDayData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }
  
  File weekFile = SPIFFS.open(WEEKDAY_FILE, "r");

  r=0;
//while(weekFile.available() && (r <= 6)) {
  for(r = 0; r<=6; r++) {
      yield();
      if (Verbose) TelnetStream.printf(" %d", r);
      weekDayDat[r].Label            = (int)weekFile.readStringUntil(';').toInt();
      weekDayDat[r].EnergyDelivered  = (float)weekFile.readStringUntil(';').toFloat();
      weekDayDat[r].EnergyReturned   = (float)weekFile.readStringUntil(';').toFloat();
      weekDayDat[r].GasDelivered     = (float)weekFile.readStringUntil(';').toFloat();
      String n = weekFile.readStringUntil('\n');
      delay(10);
      //r++;
  }
  
  weekFile.close();  

  if (weekDayDat[6].Label != 6) {
    TelnetStream.printf("readWeekDayData(): ERROR!! ==> read [%d] records\r\n", r);
    TelnetStream.flush();
    writeLogFile("Error reading [weekDay.csv] ...");
    return false;
  }

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // readWeekDayData()


//===========================================================================================
bool saveMonthData(int8_t thisYear, int8_t thisMonth) {
//===========================================================================================
  bool   errorVal = false;
  int8_t sltYear  = thisYear;
  int8_t sltMonth = thisMonth;
  File   monthsFile;
  
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  TelnetStream.printf("saveMonthData(%02d-%02d) ... ", thisYear, thisMonth);
  Serial.printf("saveMonthData(%02d-%02d) ... ", thisYear, thisMonth);
  writeLogFile("saveMonthData() ... ");
  displayMonthsHist(false);    // log to file

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }
  
  if (    (monthsDat[1].EnergyDelivered == 0.0) 
       || (monthsDat[1].EnergyReturned == 0.0)
       || (monthsDat[1].GasDelivered == 0.0)) {
    TelnetStream.println("saveMonthData(): Error (Zero value)!!\r");
    TelnetStream.flush();
    return false;
  }

  sprintf(cMsg, MONTHS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();
  TelnetStream.printf("Record Length is [%d]\n", recLength);
  Serial.printf("Record Length is [%d]\n", recLength);

  if (!SPIFFS.exists(MONTHS_FILE)) {
    TelnetStream.println("saveMonthData(): create empty file");
    writeLogFile("saveMonthData(): create empty file");
    
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
  TelnetStream.print(cMsg);
  Serial.print(cMsg);
  monthsFile.seek(recLength, SeekSet);
  monthsFile.print(cMsg);

  monthsFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return true;

} // saveMonthData()


//===========================================================================================
void readMonthData() {
//===========================================================================================
  int8_t  r=1;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  
  TelnetStream.print("readMonthData() ... ");
  Serial.print("readMonthData() ... ");
  writeLogFile("readMonthData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return;
  }

  sprintf(cMsg, MONTHS_CSV_HEADER);
  int8_t recLength = String(cMsg).length();

  File monthsFile = SPIFFS.open(MONTHS_FILE, "r");
//-- seek() gives strange results ..  
//monthsFile.seek(recLength, SeekSet); // skip header
  String mHeader = monthsFile.readStringUntil('\n');  // skip header
  
//while(monthsFile.available() && (r < 25)) {
  for(r = 1; r <= 24; r++) {
      yield();
      if (Verbose) TelnetStream.printf(" %d", r);
      monthsDat[r].Label            = (int)monthsFile.readStringUntil(';').toInt();
      if (Verbose) TelnetStream.printf("[%02d] YYMM[%04d]\r\n", r, monthsDat[r].Label);
      monthsDat[r].EnergyDelivered  = (float)monthsFile.readStringUntil(';').toFloat();
      monthsDat[r].EnergyReturned   = (float)monthsFile.readStringUntil(';').toFloat();
      monthsDat[r].GasDelivered     = (float)monthsFile.readStringUntil(';').toFloat();
      String n = monthsFile.readStringUntil('\n');
      delay(5);
      //r++;
  }
  
  monthsFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

} // readMonthData()



//===========================================================================================
int8_t getLastMonth() {
//===========================================================================================
  int16_t yearMonth, lastMonth;
  
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  TelnetStream.print("getLastMonth() ... ");
  Serial.print("getLastMonth() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
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
  
  lastMonth = (yearMonth % 100);
  
  sprintf(cMsg, "getLastMonth() ... ==> [%02d]", lastMonth);
  Serial.println(cMsg);
  writeLogFile(cMsg);
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  return lastMonth;

} // getLastMonth()


//===========================================================================================
void shiftDownMonthData(int8_t thisYear, int8_t thisMonth) {
//===========================================================================================
  uint8_t recNo = 0;
  File   monthsFile;

  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  TelnetStream.printf("shiftDownMonthData(%02d-%02d) ... \n", thisYear, thisMonth);
  Serial.printf("shiftDownMonthData(%02d-%02d) ... \n", thisYear, thisMonth);
  writeLogFile("shiftDownMonthData() ... ");
  displayMonthsHist(false);    // log to file

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return;
  }

  monthsFile = SPIFFS.open(MONTHS_FILE, "w");

  //--- write header -----
  sprintf(cMsg, MONTHS_CSV_HEADER);
  monthsFile.print(cMsg);
  TelnetStream.printf("[%02d] => tab[00] %s", recNo, cMsg);
  Serial.printf("[%02d] => tab[00] %s", recNo, cMsg);
  recNo++;
  
  int8_t recLength = String(cMsg).length();
  TelnetStream.printf("Record Length is [%d]\n", recLength);

  //--- write first record ---------
  sprintf(cMsg, "%02d%02d; %12s; %12s; %10s;\n", thisYear, thisMonth
                                               , String(monthsDat[1].EnergyDelivered, 3).c_str()
                                               , String(monthsDat[1].EnergyReturned, 3).c_str()
                                               , String(monthsDat[1].GasDelivered, 2).c_str());
  monthsFile.print(cMsg);
  TelnetStream.printf("recNo[%02d] => tab[01] %s", recNo, cMsg);
  Serial.printf("recNo[%02d] => tab[01] %s", recNo, cMsg);
  recNo++;
  //--- write second, third etc. record ---------
  for (int8_t ym=2; ym<=24; ym++) {
      sprintf(cMsg, "%04d; %12s; %12s; %10s;\n", monthsDat[ym].Label
                                               , String(monthsDat[ym].EnergyDelivered, 3).c_str()
                                               , String(monthsDat[ym].EnergyReturned, 3).c_str()
                                               , String(monthsDat[ym].GasDelivered, 2).c_str());
      TelnetStream.printf("recNo[%02d] => tab[%02d] %s", recNo, ym, cMsg);
      Serial.printf("recNo[%02d] => tab[%02d] %s\n", recNo, ym, cMsg);
      monthsFile.print(cMsg);
      recNo++;
  }
  
  monthsFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

  readMonthData();

} // shiftDownMonthData()


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
