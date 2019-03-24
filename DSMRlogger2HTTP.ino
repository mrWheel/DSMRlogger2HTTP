/*
***************************************************************************  
**  Program  : DSMRlogger2HTTP
*/
#define _FW_VERSION "v0.7.7 (" +String( __DATE__) + " " + String(__TIME__) + ")"
/*
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
  Arduino-IDE settings for ESP01 (black):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DOUT"
    - Flash size: "1M (128K SPIFFS)"
    - Debug port: "Disabled"
    - Debug Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "nodemcu"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz" 
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "1"  // GPIO01 !! for ESP-01S USE "2"! Also the "S" has no red led
    - Upload Speed: "115200"
    - Erase Flash: "Only Sketch"
    - Port: "ESP01-DSMR at <-- IP address -->"
*/

/******************** change this for testing only **********************************/
// #define HAS_NO_METER       // define if No Smart Meter is attached
/******************** don't change enything below this comment **********************/

//  part of ESP8266 Core https://github.com/esp8266/Arduino
#include <ESP8266WiFi.h>        // version 1.0.0

//  part of ESP8266 Core https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>   // Version 1.0.0

//  https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>        // version 0.14.0

//  part of ESP8266 Core https://github.com/esp8266/Arduino
#include <ArduinoOTA.h>         // Version 1.0.0

//  https://github.com/jandrassy/TelnetStream
#include <TelnetStream.h>       // Version 0.0.1

//  part of ESP8266 Core https://github.com/esp8266/Arduino
#include <FS.h>

//  https://github.com/PaulStoffregen/Time
#include <TimeLib.h>

//  https://github.com/matthijskooijman/arduino-dsmr
#include <dsmr.h>               // Version 0.1.0

#ifdef ARDUINO_ESP8266_NODEMCU
  #define VCC_ENABLE   14       // D3 = GPIO0, D5 = GPIO14, D6 = GPIO12
  #ifdef HAS_NO_METER
    #define HOSTNAME     "TEST-DSMR"
  #else
    #define HOSTNAME     "NODEMCU-DSMR"
  #endif
  //#define VCC_ENABLE    0     // <-- define valid GPIO-pin for DTR
#endif
#ifdef ARDUINO_ESP8266_GENERIC
  #ifdef HAS_NO_METER
    #define HOSTNAME     "TEST-DSMR"
  #else
    #define HOSTNAME     "ESP01-DSMR2"  // <<-- testing!!!!!
  #endif
  //#define VCC_ENABLE    0     // ESP01 does not have a free GPIO-pin
#endif
#ifdef HAS_NO_METER
    #define HOURS_FILE        "/TSThours.csv"
    #define WEEKDAY_FILE      "/TSTweek.csv"
    #define MONTHS_FILE       "/TSTmonths.csv"
#else
    #define HOURS_FILE        "/PRDhours.csv"
    #define WEEKDAY_FILE      "/PRDweek.csv"
    #define MONTHS_FILE       "/PRDmonths.csv"
#endif
#define MONTHS_CSV_HEADER "YYMM;   Energy Del;   Energy Ret;    Gas Del;\n"
#define HOURS_CSV_HEADER  "HR; Energy Del; Energy Ret;    Gas Del;\n"
#define LOG_FILE          "/logger.txt"
#define LOG_FILE_R        "/loggerR.txt"
#define NUMLASTLOG        3  
#define LED_ON            LOW
#define LED_OFF           HIGH

/**
 * Define the DSMRdata we're interested in, as well as the DSMRdatastructure to
 * hold the parsed DSMRdata. This list shows all supported fields, remove
 * any fields you are not using from the below list to make the parsing
 * and printing code smaller.
 * Each template argument below results in a field of the same name.
 */
using MyData = ParsedData<
  /* String */         identification
  /* String */        ,p1_version
  /* String */        ,timestamp
  /* String */        ,equipment_id
  /* FixedValue */    ,energy_delivered_tariff1
  /* FixedValue */    ,energy_delivered_tariff2
  /* FixedValue */    ,energy_returned_tariff1
  /* FixedValue */    ,energy_returned_tariff2
  /* String */        ,electricity_tariff
  /* FixedValue */    ,power_delivered
  /* FixedValue */    ,power_returned
//  /* FixedValue */    ,electricity_threshold
//  /* uint8_t */       ,electricity_switch_position
//  /* uint32_t */      ,electricity_failures
//  /* uint32_t */      ,electricity_long_failures
//  /* String */        ,electricity_failure_log
//  /* uint32_t */      ,electricity_sags_l1
//  /* uint32_t */      ,electricity_sags_l2
//  /* uint32_t */      ,electricity_sags_l3
//  /* uint32_t */      ,electricity_swells_l1
//  /* uint32_t */      ,electricity_swells_l2
//  /* uint32_t */      ,electricity_swells_l3
//  /* String */        ,message_short
//  /* String */        ,message_long
  /* FixedValue */    ,voltage_l1
  /* FixedValue */    ,voltage_l2
  /* FixedValue */    ,voltage_l3
  /* FixedValue */    ,current_l1
  /* FixedValue */    ,current_l2
  /* FixedValue */    ,current_l3
  /* FixedValue */    ,power_delivered_l1
  /* FixedValue */    ,power_delivered_l2
  /* FixedValue */    ,power_delivered_l3
  /* FixedValue */    ,power_returned_l1
  /* FixedValue */    ,power_returned_l2
  /* FixedValue */    ,power_returned_l3
  /* uint16_t */      ,gas_device_type
  /* String */        ,gas_equipment_id
//  /* uint8_t */       ,gas_valve_position
  /* TimestampedFixedValue */ ,gas_delivered
//  /* uint16_t */      ,thermal_device_type
//  /* String */        ,thermal_equipment_id
//  /* uint8_t */       ,thermal_valve_position
//  /* TimestampedFixedValue */ ,thermal_delivered
//  /* uint16_t */      ,water_device_type
//  /* String */        ,water_equipment_id
//  /* uint8_t */       ,water_valve_position
//  /* TimestampedFixedValue */ ,water_delivered
//  /* uint16_t */      ,slave_device_type
//  /* String */        ,slave_equipment_id
//  /* uint8_t */       ,slave_valve_position
//  /* TimestampedFixedValue */ ,slave_delivered
>;
 
//===========================================================================================
void displayHoursHist(bool);  // prototype (see MenuStuff tab)
void displayDaysHist(bool);   // prototype
void displayMonthsHist(bool); // prototype
void configModeCallback(WiFiManager *myWiFiManager);
void processData(MyData DSMRdata);
//===========================================================================================

typedef struct {
    uint16_t  Label;
    float     EnergyDelivered;
    float     EnergyReturned;
    float     GasDelivered;
} dataStruct;

static dataStruct hoursDat[10];    // 0 + 1-8
static dataStruct weekDat[9];      // 0 - 6 (0=sunday)
static dataStruct monthsDat[27];   // 0 + year1 1 t/m 12 + year2 1 t/m 12
static char *weekDayName[]  { "Zondag", "Maandag", "Dinsdag", "Woensdag"
                            , "Donderdag", "Vrijdag", "Zaterdag", "UnKnown" };
static char *monthName[]    { "00", "Januari", "Februari", "Maart", "April", "Mei", "Juni", "Juli"
                            , "Augustus", "September", "Oktober", "November", "December", "13" };
static char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "UnKnown" };

/**
struct FSInfo {
    size_t totalBytes;
    size_t usedBytes;
    size_t blockSize;
    size_t pageSize;
    size_t maxOpenFiles;
    size_t maxPathLength;
};
**/
static FSInfo SPIFFSinfo;

// Set up to read from the Serial port, and use VCC_ENABLE as the
// request pin. 
#ifdef VCC_ENABLE
  P1Reader reader(&Serial, VCC_ENABLE);
#else
  P1Reader reader(&Serial, 0);
#endif
;

WiFiClient        wifiClient;
ESP8266WebServer  server ( 80 );

uint32_t  waitLoop, noMeterWait, telegramCount, telegramErrors, waitForATOupdate;
char      cMsg[100], fChar[10];
char      APname[50], MAChex[13]; //n1n2n3n4n5n6\0
float     EnergyDelivered, EnergyReturned;
float     PowerDelivered, PowerReturned;
int32_t   PowerDelivered_l1, PowerDelivered_l2, PowerDelivered_l3;
int32_t   PowerReturned_l1,  PowerReturned_l2,  PowerReturned_l3;
float     GasDelivered;
String    pTimestamp;
String    P1_Version, Equipment_Id, GasEquipment_Id, ElectricityTariff;
char      Identification[150];
float     EnergyDeliveredTariff1, EnergyDeliveredTariff2, EnergyReturnedTariff1, EnergyReturnedTariff2;
float     Voltage_l1, Voltage_l2, Voltage_l3;
uint16_t  Current_l1, Current_l2, Current_l3;
uint16_t  GasDeviceType;

String    lastReset = "";
String    lastLogLine[NUMLASTLOG + 1]; 
bool      debug = true, OTAinProgress = false, Verbose = false, showRaw = false, SPIFFSmounted = false;
String    dateTime;
int8_t    thisHour = -1, thisWeekDay = -1, thisMonth = -1, lastMonth, thisYear = 15;
int8_t    testMonth = 0;
int8_t    tries, showRawCount;
uint32_t  nextSecond, unixTimestamp;
uint64_t  upTimeSeconds;
IPAddress ipDNS, ipGateWay, ipSubnet;
uint16_t  WIFIreStartCount;
String    jsonString;

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    if (i.present()) {
        TelnetStream.print(Item::name);
        TelnetStream.print(F(": "));
        TelnetStream.print(i.val());
        TelnetStream.print(Item::unit());
        TelnetStream.println();
    }
  }
};


//===========================================================================================
String macToStr(const uint8_t* mac) {
//===========================================================================================
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
} // macToStr()


//===========================================================================================
void escapeJson(const char * Src, char * Dest) {
  int p=0;

  if (Verbose) Serial.print("In[");
  for(int c=0; Src[c] != '\0'; c++) {
    if (Verbose) Serial.print(Src[c]);
    switch((char)Src[c]) {
      case '\\':  Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='5', Dest[p++]='c';  
                  break;
      case '/':   Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='2', Dest[p++]='f';
                  break;
      case '\b':  Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='0', Dest[p++]='8';
                  break;
      case '\f':  Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='0', Dest[p++]='c';
                  break;
      case '\n':  Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='0', Dest[p++]='d';
                  break;
      case '\r':  Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='0', Dest[p++]='a';
                  break;
      case '\t':  Dest[p++]='\\'; Dest[p++]='u'; Dest[p++]='0'; Dest[p++]='0'; Dest[p++]='0', Dest[p++]='9';
                  break;
      default:    Dest[p++]=Src[c];  
    }
  }
  Dest[p] = '\0';
  if (Verbose) Serial.print("] => Out[");
  for(int c=0; c < sizeof(Dest), Dest[c] != '\0'; c++) {
    if (Verbose) Serial.print(Dest[c]);
  }
  if (Verbose) Serial.println("]");
  
} // escapeJson()

//===========================================================================================
String upTime() {
//===========================================================================================

  char    calcUptime[20];

  sprintf(calcUptime, "%d(d):%02d:%02d", int((upTimeSeconds / (60 * 60 * 24)) % 365)
                                       , int((upTimeSeconds / (60 * 60)) % 24)
                                       , int((upTimeSeconds / (60)) % 60));

  return calcUptime;

} // upTime()


//===========================================================================================
void hourToSlot(int8_t h, int8_t &slot, int8_t &nextSlot, int8_t &prevSlot) {
//===========================================================================================
  switch(h) {
    case  0:
    case  1:
    case  2:  slot = 1; nextSlot = 2; prevSlot = 8; break;
    case  3:
    case  4:
    case  5:  slot = 2; nextSlot = 3; prevSlot = 1;  break;
    case  6:
    case  7:
    case  8:  slot = 3; nextSlot = 4; prevSlot = 2;  break;
    case  9:
    case 10:
    case 11:  slot = 4; nextSlot = 5; prevSlot = 3;  break;
    case 12:
    case 13:
    case 14:  slot = 5; nextSlot = 6; prevSlot = 4;  break;
    case 15:
    case 16:
    case 17:  slot = 6; nextSlot = 7; prevSlot = 5;  break;
    case 18:
    case 19:
    case 20:  slot = 7; nextSlot = 8; prevSlot = 6;  break;
    case 21:
    case 22:
    case 23:  slot = 8; nextSlot = 1; prevSlot = 7;  break;
    default:  slot = 8; nextSlot = 1; prevSlot = 7; 
  }

} // hourToSlot()


//===========================================================================================
void printData() {
//===========================================================================================

    TelnetStream.println("-Totalen----------------------------------------------------------");
    dateTime = buildDateTimeString(pTimestamp);
    sprintf(cMsg, "Datum / Tijd         :  %s", dateTime.c_str());
    TelnetStream.println(cMsg);

    dtostrf(EnergyDelivered, 9, 3, fChar);
    sprintf(cMsg, "Energy Delivered     : %skWh", fChar);
    TelnetStream.println(cMsg);

    dtostrf(EnergyReturned, 9, 3, fChar);
    sprintf(cMsg, "Energy Returned      : %skWh", fChar);
    TelnetStream.println(cMsg);

    dtostrf(PowerDelivered, 8, 0, fChar);
    sprintf(cMsg, "Power Delivered      : %skW", fChar);
    TelnetStream.println(cMsg);

    dtostrf(PowerReturned, 8, 0, fChar);
    sprintf(cMsg, "Power Returned       : %skW", fChar);
    TelnetStream.println(cMsg);
    
    dtostrf(PowerDelivered_l1, 8, 0, fChar);
    sprintf(cMsg, "Power Delivered (l1) : %sWatt", fChar);
    TelnetStream.println(cMsg);
    
    dtostrf(PowerDelivered_l2, 8, 0, fChar);
    sprintf(cMsg, "Power Delivered (l2) : %sWatt", fChar);
    TelnetStream.println(cMsg);
    
    dtostrf(PowerDelivered_l3, 8, 0, fChar);
    sprintf(cMsg, "Power Delivered (l3) : %sWatt", fChar);
    TelnetStream.println(cMsg);
    
    dtostrf(PowerReturned_l1, 8, 0, fChar);
    sprintf(cMsg, "Power Returned (l1)  : %sWatt", fChar);
    TelnetStream.println(cMsg);
    
    dtostrf(PowerReturned_l2, 8, 0, fChar);
    sprintf(cMsg, "Power Returned (l2)  : %sWatt", fChar);
    TelnetStream.println(cMsg);
    
    dtostrf(PowerReturned_l3, 8, 0, fChar);
    sprintf(cMsg, "Power Returned (l3)  : %sWatt", fChar);
    TelnetStream.println(cMsg);

    dtostrf(GasDelivered, 9, 2, fChar);
    sprintf(cMsg, "Gas Delivered        : %sm3", fChar);
    TelnetStream.println(cMsg);
    TelnetStream.println("==================================================================");
  
} // printData()


//===========================================================================================
void processData(MyData DSMRdata) {
//===========================================================================================
  int8_t slot, nextSlot, prevSlot;
  char   cID[100], cID2[100];
  
#ifndef HAS_NO_METER
  //Identification                    = DSMRdata.identification;
  //escapeJson(DSMRdata.identification.c_str(), cID2);
  //Identification                    = String(cID2);
    escapeJson(DSMRdata.identification.c_str(), Identification);
    P1_Version                        = DSMRdata.p1_version;
    pTimestamp                        = DSMRdata.timestamp;
    if (DSMRdata.equipment_id_present) {
            Equipment_Id              = DSMRdata.equipment_id;
    } else  Equipment_Id              = "UnKnown";
    if (DSMRdata.energy_delivered_tariff1_present) {
            EnergyDeliveredTariff1    = (float)DSMRdata.energy_delivered_tariff1;
    } else  EnergyDeliveredTariff1    = 0.0;
    if (DSMRdata.energy_delivered_tariff2_present) {
            EnergyDeliveredTariff2    = (float)DSMRdata.energy_delivered_tariff2;
    } else  EnergyDeliveredTariff2    = 0.0;
    if (DSMRdata.energy_returned_tariff1_present) {
            EnergyReturnedTariff1     = (float)DSMRdata.energy_returned_tariff1;
    } else  EnergyReturnedTariff1     = 0.0;
    if (DSMRdata.energy_returned_tariff2_present) {
            EnergyReturnedTariff2     = (float)DSMRdata.energy_returned_tariff2;
    } else  EnergyReturnedTariff2     = 0.0; 
    if (DSMRdata.electricity_tariff_present) {
            ElectricityTariff         = DSMRdata.electricity_tariff;
    } else  ElectricityTariff         = "-"; 
    if (DSMRdata.voltage_l1_present) {
            Voltage_l1                = (float)DSMRdata.voltage_l1;
    } else  Voltage_l1                = 0.0; 
    if (DSMRdata.voltage_l2_present) {
            Voltage_l2                = (float)DSMRdata.voltage_l2;
    } else  Voltage_l2                = 0.0; 
    if (DSMRdata.voltage_l3_present) {
            Voltage_l3                = (float)DSMRdata.voltage_l3;
    } else  Voltage_l3                = 0.0;
    if (DSMRdata.current_l1_present) {
            Current_l1                = DSMRdata.current_l1;
    } else  Current_l1                = 0;
    if (DSMRdata.current_l2_present) {
            Current_l2                = DSMRdata.current_l2;
    } else  Current_l2                = 0;
    if (DSMRdata.current_l3_present) {
            Current_l3                = DSMRdata.current_l3;
    } else  Current_l3                = 0;
    if (DSMRdata.power_delivered_present) {
            PowerDelivered            = (float)DSMRdata.power_delivered;
    } else  PowerDelivered            = 0.0;
    if (DSMRdata.power_delivered_l1_present) {
            PowerDelivered_l1         = DSMRdata.power_delivered_l1.int_val();
    } else  PowerDelivered_l1         = 0;
    if (DSMRdata.power_delivered_l2_present) {
            PowerDelivered_l2         = DSMRdata.power_delivered_l2.int_val();
    } else  PowerDelivered_l2         = 0;
    if (DSMRdata.power_delivered_l3_present) {
            PowerDelivered_l3         = DSMRdata.power_delivered_l3.int_val();
    } else  PowerDelivered_l3         = 0;
    if (DSMRdata.power_returned_present) {
            PowerReturned             = (float)DSMRdata.power_returned;
    } else  PowerReturned             = 0.0;
    if (DSMRdata.power_returned_l1_present) {
            PowerReturned_l1          = DSMRdata.power_returned_l1.int_val();
    } else  PowerReturned_l1          = 0;
    if (DSMRdata.power_returned_l2_present) {
            PowerReturned_l2          = DSMRdata.power_returned_l2.int_val();
    } else  PowerReturned_l2          = 0;
    if (DSMRdata.power_returned_l3_present) {
            PowerReturned_l3          = DSMRdata.power_returned_l3.int_val();
    } else  PowerReturned_l3          = 0;
    if (DSMRdata.gas_device_type_present) {
            GasDeviceType             = DSMRdata.gas_device_type;
    } else  GasDeviceType             = 0;
    if (DSMRdata.gas_equipment_id_present) {
            GasEquipment_Id           = DSMRdata.gas_equipment_id;
    } else  GasEquipment_Id           = "UnKnown";
    if (DSMRdata.gas_delivered_present) {
            GasDelivered              = (float)DSMRdata.gas_delivered;
    } else  GasDelivered              = 0.0;

    EnergyDelivered   = EnergyDeliveredTariff1 + EnergyDeliveredTariff2; 
    EnergyReturned    = EnergyReturnedTariff1  + EnergyReturnedTariff2;
#endif
    unixTimestamp       = epoch(pTimestamp);

//================= handle Hour change ======================================================
    if (thisHour != HourFromTimestamp(pTimestamp)) {
      thisHour = HourFromTimestamp(pTimestamp);
      hourToSlot(thisHour, slot, nextSlot, prevSlot);
      TelnetStream.printf("Saving data for thisHour[%02d] in slot[%02d] (nextSlot[%02d])\n", thisHour, slot, nextSlot);
      Serial.printf("Saving data for thisHour[%02d] in slot[%02d] (nextSlot[%02d])\n", thisHour, slot, nextSlot);
      hoursDat[slot].EnergyDelivered = EnergyDelivered;
      hoursDat[slot].EnergyReturned  = EnergyReturned;
      hoursDat[slot].GasDelivered    = GasDelivered;
      saveHourData(slot);

    } // if (thisHour != HourFromTimestamp(pTimestamp)) 

    hourToSlot(thisHour, slot, nextSlot, prevSlot);

    if (Verbose) {
      TelnetStream.printf("Put data for Hour[%02d] in nextSlot[%02d]\n", thisHour, nextSlot);
    }
    hoursDat[slot].EnergyDelivered = EnergyDelivered;
    hoursDat[slot].EnergyReturned  = EnergyReturned;
    hoursDat[slot].GasDelivered    = GasDelivered;
     
//================= handle Day change ======================================================
    if (thisWeekDay != weekday(unixTimestamp)) {
      // weekday() from unixTimestamp is from 1 (sunday) to 7 (saturday)
      thisWeekDay = weekday(unixTimestamp);
      // in our weekDat[] table we have to subtract "1" to get 0 (sunday) to 6 (saturday)
      slot = thisWeekDay - 1;
      if (slot < 0) slot = 6;
      TelnetStream.printf("Saving data for WeekDay[%02d] in slot[%02d]\n", thisWeekDay, slot);
      Serial.printf("Saving data for WeekDay[%02d] in slot[%02d]\n", thisWeekDay, slot);
      weekDat[slot].EnergyDelivered = EnergyDelivered;
      weekDat[slot].EnergyReturned  = EnergyReturned;
      weekDat[slot].GasDelivered    = GasDelivered;
      saveWeekData();
    }
    slot = weekday(unixTimestamp);
    // in our weekDat[] table we have to subtract "1" to get 0 (sunday) to 6 (saturday)
    slot -= 1;
    if (slot < 0) slot = 6;
    weekDat[slot].EnergyDelivered = EnergyDelivered;
    weekDat[slot].EnergyReturned  = EnergyReturned;
    weekDat[slot].GasDelivered    = GasDelivered;

//================= handle Month change ======================================================
    if (thisMonth != MonthFromTimestamp(pTimestamp)) {
      thisMonth = MonthFromTimestamp(pTimestamp);
      thisYear  = YearFromTimestamp(pTimestamp);
      if (Verbose) TelnetStream.printf("processData(): thisYear[20%02d] => thisMonth[%02d]\r\n", thisYear, thisMonth);
      TelnetStream.flush();
      lastMonth = getLastMonth();
      TelnetStream.printf("processData(): lastMonth[%02d] - thisYear[20%02d] => thisMonth[%02d]\r\n"
                                          , lastMonth, thisYear, thisMonth);
      if (lastMonth != thisMonth) {
          if (Verbose) TelnetStream.printf("processData(): lastMonth[%02d]; thisYear[%02d] => thisMonth[%02d]\r\n"
                                                          ,lastMonth,       thisYear,         thisMonth);
          TelnetStream.println("processData(): Move thisMonth one slot up");
          TelnetStream.flush();
          Serial.println("processData(): Move thisMonth one slot up");
          shiftDownMonthData(thisYear, thisMonth);
          if (Verbose) TelnetStream.println("processData(): months shifted down!");
          TelnetStream.flush();
      }
      TelnetStream.printf("processData(): Saving data for thisMonth[20%02d-%02d] in slot[01]\n", thisYear, thisMonth);
      TelnetStream.flush();
      Serial.printf("processData(): Saving data for thisMonth[20%02d-%02d] in slot[01]\n", thisYear, thisMonth);
      sprintf(cMsg, "%02d%02d", thisYear, thisMonth);
      monthsDat[1].Label           = String(cMsg).toInt();
      monthsDat[1].EnergyDelivered = EnergyDelivered;
      monthsDat[1].EnergyReturned  = EnergyReturned;
      monthsDat[1].GasDelivered    = GasDelivered;
      saveThisMonth(thisYear, thisMonth, false);
      if (Verbose) TelnetStream.printf("processData(): monthsDat[1] for [20%04d] saved!\r\n", String(cMsg).toInt());
      TelnetStream.flush();

    } // if (thisMonth != MonthFromTimestamp(pTimestamp)) 

    if (Verbose) {
      TelnetStream.printf("Put data for Month[20%02d-%02d] in Slot[01]\n", thisYear, thisMonth);
      TelnetStream.flush();
    }
    monthsDat[1].EnergyDelivered = EnergyDelivered;
    monthsDat[1].EnergyReturned  = EnergyReturned;
    monthsDat[1].GasDelivered    = GasDelivered;
   
} // processData()


//===========================================================================================
void setup() {
//===========================================================================================
  Serial.begin(115200, SERIAL_8N1);
  pinMode(BUILTIN_LED, OUTPUT);
  for(int I=0; I<5; I++) {
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
    delay(2000);
  }
  digitalWrite(BUILTIN_LED, LED_OFF);  // HIGH is OFF
  lastReset     = ESP.getResetReason();
  
  #ifdef ARDUINO_ESP8266_GENERIC
    debug = false;
  #endif
  
  if (debug) Serial.println("\nBooting....\n");

//================ SPIFFS =========================================
  if (!SPIFFS.begin()) {
    if (debug) Serial.println("SPIFFS Mount failed");   // Serious problem with SPIFFS 
    TelnetStream.println("SPIFFS Mount failed");        // Serious problem with SPIFFS 
    TelnetStream.flush();
    SPIFFSmounted = false;
    
  } else { 
    if (debug) Serial.println("SPIFFS Mount succesfull");
    TelnetStream.println("SPIFFS Mount succesfull");
    TelnetStream.flush();
    SPIFFSmounted = true;
    sprintf(cMsg, "Last reset reason: [%s]", ESP.getResetReason().c_str());
    if (debug) {
      Serial.println(cMsg);
      Serial.flush();
    }
    TelnetStream.println(cMsg);
    TelnetStream.flush();
  }
//=============end SPIFFS =========================================

#ifdef VCC_ENABLE
    // This is needed on Pinoccio Scout boards to enable the 3V3 pin.
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, LOW);
#endif


  digitalWrite(BUILTIN_LED, LED_ON);
  setupWiFi(false);
  digitalWrite(BUILTIN_LED, LED_OFF);

  if (debug) {
    Serial.println ( "" );
    Serial.print ( "Connected to " ); Serial.println (WiFi.SSID());
    Serial.print ( "IP address: " );  Serial.println (WiFi.localIP());
  }
  for (int L=0; L < 10; L++) {
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
    delay(200);
  }
  digitalWrite(BUILTIN_LED, LED_OFF);

  TelnetStream.begin();
  TelnetStream.flush();
  if (debug) Serial.println("\nTelnet server started ..");
  delay(500);

  //============= configure OTA (minimal) ====================
  ArduinoOTA.setHostname(HOSTNAME);   // defaults to esp8266-[ChipID]
  ArduinoOTA.onStart([]() {
    Serial.swap();  // stop receiving data from TxR
    OTAinProgress = true;
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    SPIFFS.end();
    SPIFFSmounted = false;
  });
  ArduinoOTA.begin();
  //==============OTA end=====================================

//------ tabellen inlezen ----------------------------
  if (!readHourData())    TelnetStream.println("setup(): error readHourData()!");
  if (!readWeekData()) TelnetStream.println("setup(): error readWeekData()!");
  if (!readMonthData())   TelnetStream.println("setup(): error readMonthData()!");
  TelnetStream.flush();
  thisYear        = getLastYear();
  thisMonth       = getLastMonth();

  telegramCount   = 0;
  telegramErrors  = 0;
  
  server.on("/getDeviceInfo.json", sendDataDeviceInfo);
  server.on("/getActual.json", sendDataActual);
  server.on("/getTableWeek.json", sendTableWeek);
  server.on("/getTableHours.json", sendTableHours);
  server.on("/getTableMonths.json", sendTableMonths);
  server.on("/ReBoot", HTTP_POST, handleReBoot);
  
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/index.js", SPIFFS, "/index.js");

  server.on("/onderhoud", HTTP_POST, handleFileDelete);
  server.on("/onderhoud", handleRoot);
  server.on("/onderhoud/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  server.begin();
  if (debug) Serial.println( "HTTP server started" );
  TelnetStream.println( "HTTP server started" );
  Serial.flush();
  TelnetStream.flush();

#ifdef VCC_ENABLE
  digitalWrite(VCC_ENABLE, LOW);
  delay(200);
#endif
  for(int l=0; l < NUMLASTLOG; l++) {
    lastLogLine[l] = String(l);
  }
  
  if (debug) Serial.println("\nEnable reader..");
  Serial.flush();
  TelnetStream.println("\nEnable reader..");
  TelnetStream.flush();
  delay(100);
  reader.enable(true);

  waitLoop      = millis() + 5000;
  noMeterWait   = millis() + 5000;
  upTimeSeconds = (millis() / 1000) + 50;
  nextSecond    = millis() + 1000;
  
} // setup()


//===========================================================================================
void loop () {
//===========================================================================================
  ArduinoOTA.handle();
  server.handleClient();
  handleKeyInput();

  if (!showRaw) {
    reader.loop();
  }
  
  if (!OTAinProgress) {
    if (!showRaw) {
      if (millis() > waitLoop) {
        //waitLoop = millis() + 10000;  // tien seconden?
        waitLoop = millis() + 2000;  // test 2 seconden

        reader.enable(true);
#ifdef ARDUINO_ESP8266_GENERIC
        digitalWrite(BUILTIN_LED, LED_ON);
#else
        digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
#endif
      }
    } 
  } else {  // waiting for ATO-update (or in progress) ...
      if (millis() > waitForATOupdate) {
        OTAinProgress     = false;
        waitForATOupdate  = millis();
      }
  }
  
#ifdef HAS_NO_METER
  // ---- create some dummy data for testing without a Slimme Meter connected (HAS_NO_METER is defined)
  static  MyData    DSMRdata;
  static  uint8_t   sMinute = 1, sHour = 20, sDay = 27, sMonth = 12, sYear = 16;
          char      testID[100];
          
  if (millis() > noMeterWait) {
    noMeterWait += 2000;

    strcpy(testID, "/ABCD(*)EFGHIJ(*)KLMNOPQRSTUVWXYZ");
    testID[6]  = '\\';
    testID[15] = '\t';
    escapeJson(testID, Identification);
    P1_Version                        = "TST";

    EnergyDelivered  += (float)(random(1, 50) / 15.0);
    EnergyReturned   += (float)(random(1, 40) / 55.0);
    GasDelivered     += (float)(random(1, 30) / 100.0);
    sMinute += 37;
    if (testMonth > 0) {
      sMonth   += testMonth;    
      sDay      = 10;
      testMonth = 0;
    }
    if (sMinute >= 60) {
      sMinute -= 59;
      sHour++;
    }
    if (sHour >= 24) {  // 0 .. 23
      sHour = 0;
      sDay += 3;
    }
    if (sDay >= 30) {
      sDay = (sDay % 30 ) + 1;
      sMonth++;
    }
    if (sMonth <  1) sMonth = 1;
    if (sMonth > 12) {
      sMonth = 1;
      sYear++;
    }

    telegramCount++;
    sprintf(cMsg, "%02d%02d%02d%02d%02d15S", sYear, sMonth, sDay, sHour, sMinute);
    pTimestamp = String(cMsg);
    TelnetStream.printf("pTimestamp [%s] sYear[%02d] sMonth[%02d] sDay[%02d] sHour[%02d] sMinute[%02d]\r\n"
                       , pTimestamp.c_str(), sYear,  sMonth,      sDay,      sHour,      sMinute);
    TelnetStream.flush();
    if (!OTAinProgress && !showRaw) {
      digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
      processData(DSMRdata);
    }

  } // noMeterWait > millis()

#else
  //---- this part is only processed when HAS_NO_METER is not defined!
  if (!OTAinProgress) {
    if (showRaw) {
      while(Serial.available() > 0) {
        char rIn = Serial.read();
        if (rIn == '!') showRawCount++;
        TelnetStream.write((char)rIn);
      }
      if (showRawCount > 20) {
        showRaw = false;
      }
    } else {
      if (reader.available()) {
        ArduinoOTA.handle();

        //-- declaration of DSMRdata must be in 
        //-- if-statement so it will be initialized
        //-- in every itteration (don't know how else)
        MyData    DSMRdata;
        String    DSMRerror;

        TelnetStream.println("\n==================================================================\r");
        TelnetStream.printf("read telegram [%d]\r\n", ++telegramCount);
        Serial.printf("read telegram [%d]\n", ++telegramCount);

        if (reader.parse(&DSMRdata, &DSMRerror)) {  // Parse succesful, print result
          digitalWrite(BUILTIN_LED, LED_OFF);
          processData(DSMRdata);
          if (Verbose) {
            DSMRdata.applyEach(showValues());
            printData();
          }
        } else {                                    // Parser error, print error
          TelnetStream.printf("Parse error %s\r\n", DSMRerror.c_str());
          telegramErrors++;
        }
        
      } // if (reader.available()) 
    }
  } // if (!OTAinProgress) 
#endif

  if (millis() > nextSecond) {
    nextSecond += 1000; // nextSecond is ahead of millis() so it will "rollover" 
    upTimeSeconds++;    // before millis() and this will probably work just fine
  }

} // loop()



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
