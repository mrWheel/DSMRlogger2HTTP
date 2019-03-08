# Binary voor ESP-01 

## 1)
Download, als zip file, de laatste versie van DSMRlogger2HTTP van github

### 1.1)
Als je het zipfile download, pak deze dan uit (unzip) en kopieer de map 
```DSMRlogger2HTTP-master``` naar je Arduino projecten directory met de naam ```DSMRlogger2HTTP``` (let
op, het deel '**-master**' moet je dus weghalen!).

In de map staan nu twee andere mappen die van belang zijn. '**/bin**' want hier staat het binary file in dat we naar de ESP-01 gaan uploaden en '**/data**' waar de bestanden voor het file-systeem in staan.

### 1.2)
Je kunt de repository ook clonen. Ga hiervoor in de Arduino projecten directory staan en toets het volgende commando in:

```
    git clone https://github.com/mrWheel/DSMRlogger2HTTP.git
```

Zoek in de (nieuwe) map ```DSMRlogger2HTTP/bin/``` het binary file met de nieuwe firmware. Onthou het pad hier naartoe.

## 2) Voor update "Over The Air":

### 2.1) 
Zoek uit waar jouw versie van '**espota.py**' staat
### 2.2) 
Zoek uit wat het IPaddress van je ESP-01 is (kijk in de IDE bij [port]/[network ports])
### 2.3) 
Onthou waar je de binary van **DSMRlogger2HTTP_v6.ino.bin** heb opgeslagen.
### 2.4) 
Toets het commando in zoals hier onder weergegeven, maar vervang alle variabelen met wat je hiervoor hebt uitgezocht!
```
    python <path naar: 'espota.py'> -i <IPaddress ESP-01> -p 8266 --auth= -f <path naar 'DSMRlogger2HTTP_v6.ino.bin'>
```

*LET OP: de quotjes ('), kleiner-dan (<) en groter-dan (>) tekens moet je niet intoetsen!!*

## 3) Voor update naar een, via een kabel verbonden, programmer voor de ESP-01:

### 3.1)
Zoek uit waar jouw versie van '**esptool**' staat
### 3.2)
Zoek uit op welke (USB)poort de programmer voor de ESP-01 is aangesloten
### 3.3)
Onthou waar je de binary van ```DSMRlogger2HTTP_v6.ino.bin``` heb opgeslagen.
### 3.4)
Toets het commando in zoals hier onder weergegeven, maar vervang alle variabelen met wat je hiervoor hebt uitgezocht!

```
  <path naar: 'esptool'> -vv -cd nodemcu -cb 115200 -cp <USBpoort> -ca 0x00000 -cf <path naar 'DSMRlogger2HTTP_v6.ino.bin'>
```

*LET OP: de quotjes ('), kleiner-dan (<) en groter-dan (>) tekens moet je niet intoetsen!!*

## 4)
Start nu de ArduinoIDE op, selecteer het programma ```DSMRlogger2HTTP```.

## 5)
Selecteer in [**Tools**] de correcte settings voor het ESP-01 board.

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DOUT"
    - Flash size: "1M (128K SPIFFS)"
    - Debug port: "Disabled"
    - Debug Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "nodemcu"   // <-- dit is afhankelijk van de programmer die je gebruikt
    - Crystal Frequency: "26 MHz" (otherwise Serial output is garbidge)
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "1"  // GPIO01 - Pin 2
    - Upload Speed: "115200"
    - Erase Flash: "Only Sketch"
    - Port: "ESP01-DSMR at <-- IP address -->" of USB poort

Klik nu [**Tools**] -> [**ESP8266 Sketch Data Upload**] aan. Nu wordt het filesysteem, inclusief de benodigde bestanden (met name **index.html** en **index.js**) in het SPIFFS bestands-systeem gezet!

## 6)
Klaar!
