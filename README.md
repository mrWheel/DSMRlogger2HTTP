# DSMRlogger2HTTP
This repository contains the software for the *Draadloze Slimme Meter Reader*.

![Completed](/images/DSMRlogger_Casing.jpg)

Almost all households in the Netherlands are equipped with such a Smart Meter.

The name DSMRlogger is derived from the acronym DSMR which stands for “*Dutch Smart Meter Requirements*“.
The hardware and software should work from **version 4.0** and up of these requirements (as from that version on
the requirements state a 5v output @250mA, which is needed by the DSMRlogger to operate).

Unzip the file into your Arduino Projects directory.

A map "**DSMRlogger2HTML**" will be created with in it all the files necessary to program an ESP01
to function as a “*Slimme Meter Uitlezer*”.

If you use an **ESP01 black** (recommended) than you **must** set the board to:

<code>
Arduino-IDE settings for ESP01 (black):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DOUT"
    - Flash size: "1M (128K SPIFFS)"
    - Debug port: "Disabled"
    - Debug Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "nodemcu"   // <-- this actualy depends on the programmer you use
    - Crystal Frequency: "26 MHz"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "1"  // GPIO01 - Pin 2
    - Upload Speed: "115200"
    - Erase Flash: "Only Sketch"
    - Port: "ESP01-DSMR at <-- IP address →"
</code>

For more information see [this post](https://willem.aandewiel.nl/index.php/slimme-meter-uitlezer/).

If you are new to programming the ESP8266 (and you have mastered the Dutch language) I recoment 
[this](https://willem.aandewiel.nl/index.php/aan-de-slag-met-de-esp8266/) post!

![Actual](/images/DSMR_Actual.png)
![DeviceInfo](/images/DSMR_DeviceInfo.png)

If you upload one of the <code>PRDfiles.csv</code>, you need to reboot the ESP01-DSMR before it overwrites the files again from memory to SPIFFS (every change of the hour, day or month).

![Onderhoud](/images/DSMR_Onderhoud.png)

