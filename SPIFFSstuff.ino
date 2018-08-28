/*
***************************************************************************  
**  Program  : SPIFFSstuff, part of DSMRlogger2HTTP
**  Version  : v4.0
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
bool saveHourData() {
//===========================================================================================
  bool errorVal = false;
  
  TelnetStream.print("saveHourData() ... ");
  writeLogFile("saveHourData() ... ");
  displayHoursHist(false);    // log to file

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }
  
  if ((EnergyDelivered == 0.0) || (EnergyReturned == 0.0) || (GasDelivered == 0.0)) {
    TelnetStream.println("saveHourData(): Error (Zero value)!!\r");
    TelnetStream.flush();
    return false;;
  }

  File hoursFile = SPIFFS.open(HOURS_FILE, "w");
    
  for (int16_t r=0; r < 24; r++) {
    yield();
    TelnetStream.printf(" %d", r);
    hoursFile.print(r);  // Label
    hoursFile.print(";");
    hoursFile.print((float)hoursDat[r].EnergyDelivered);
    hoursFile.print(";");
    hoursFile.print((float)hoursDat[r].EnergyReturned);
    hoursFile.print(";");
    hoursFile.print((float)hoursDat[r].GasDelivered);
    hoursFile.println(";");
  }

  hoursFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();

  return true;

} // saveHourData()

//===========================================================================================
bool readHourData() {
//===========================================================================================
  int8_t  r=0;
  
  TelnetStream.print("readHourData() ... ");
  writeLogFile("readHourData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }
  
  File hoursFile = SPIFFS.open(HOURS_FILE, "r");
  TelnetStream.printf("%s: size(%d) ", HOURS_FILE, hoursFile.size());

  while(hoursFile.available() && (r < 24)) {
      yield();
      TelnetStream.printf(" %d", r);
      hoursDat[r].Label            = (int)hoursFile.readStringUntil(';').toInt();
      hoursDat[r].EnergyDelivered  = (float)hoursFile.readStringUntil(';').toFloat();
      hoursDat[r].EnergyReturned   = (float)hoursFile.readStringUntil(';').toFloat();
      hoursDat[r].GasDelivered     = (float)hoursFile.readStringUntil(';').toFloat();
      String n = hoursFile.readStringUntil('\n');
      delay(10);
      r++;
  }
  
  hoursFile.close();  
  
  if (hoursDat[23].Label != 23) {
    TelnetStream.printf(" ERROR!! ==> read [%d] records\r\n", r);
    TelnetStream.flush();
    writeLogFile("Error reading [hours.csv] ...");
    return false;
  }

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  return true;

} // readHourData()


//===========================================================================================
void saveWeekDayData() {
//===========================================================================================

  TelnetStream.print("saveWeekDayData(s) ... ");
  writeLogFile("saveWeekDayData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return;
  }

  displayDaysHist(false);   // write to log-file

  File WeekFile = SPIFFS.open(WEEKDAY_FILE, "w");
    
  for (int16_t r=0; r < 7; r++) {
    yield();
    TelnetStream.printf(" %d", r);
    WeekFile.print(r);  // Label
    WeekFile.print(";");
    WeekFile.print((float)weekDayDat[r].EnergyDelivered);
    WeekFile.print(";");
    WeekFile.print((float)weekDayDat[r].EnergyReturned);
    WeekFile.print(";");
    WeekFile.print((float)weekDayDat[r].GasDelivered);
    WeekFile.println(";");
  //delay(10);
  }

  WeekFile.close();  

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  
} // saveWeekDayData()

//===========================================================================================
bool readWeekDayData() {
//===========================================================================================
  int8_t  r=0;
  
  TelnetStream.print("readWeekDayData() ... ");
  writeLogFile("readWeekDayData() ... ");

  if (!SPIFFSmounted) {
    TelnetStream.println("No SPIFFS filesystem..\r");
    return true;
  }
  
  File WeekFile = SPIFFS.open(WEEKDAY_FILE, "r");

  while(WeekFile.available() && (r < 7)) {
      yield();
      TelnetStream.printf(" %d", r);
      weekDayDat[r].Label            = (int)WeekFile.readStringUntil(';').toInt();
      weekDayDat[r].EnergyDelivered  = (float)WeekFile.readStringUntil(';').toFloat();
      weekDayDat[r].EnergyReturned   = (float)WeekFile.readStringUntil(';').toFloat();
      weekDayDat[r].GasDelivered     = (float)WeekFile.readStringUntil(';').toFloat();
      String n = WeekFile.readStringUntil('\n');
      delay(10);
      r++;
  }
  
  WeekFile.close();  

  if (weekDayDat[6].Label != 6) {
    TelnetStream.printf(" ERROR!! ==> read [%d] records\r\n", r);
    TelnetStream.flush();
    writeLogFile("Error reading [weekDay.csv] ...");
    return false;
  }

  TelnetStream.println(" Done\r");
  TelnetStream.flush();
  return true;

} // readWeekDayData()


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
