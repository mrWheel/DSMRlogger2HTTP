# DSMRlogger2HTTP
This repository contains the software for the *Draadloze Slimme Meter Uitlezer*.

![Completed](/images/DSMRlogger_Casing.jpg)

Almost all households in the Netherlands are equipped with such a Smart Meter.

The name DSMRlogger is derived from the acronym DSMR which stands for “*Dutch Smart Meter Requirements*“.
The hardware and software should work from **version 4.0** and up of these requirements (as from that version on
the requirements state a 5v output @250mA, which is needed by the DSMRlogger to operate).

Unzip the file into your Arduino Projects directory.

A map "**DSMRlogger2HTML**" will be created with in it all the files necessary to program an ESP01
to function as a “*Slimme Meter Uitlezer*”.

If you use an **ESP01 black** (recommended) than you should set the board to:

<code>
Arduino-IDE settings for ESP01 (black):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DIO" / "DOUT"
    - Flash size: "1M (128K SPIFFS)"
    - Debug port: "Disabled"
    - Debug Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "nodemcu"
    - Crystal Frequency: "26 MHz" (otherwise Serial output is garbidge)
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "1"  // GPIO01 - Pin 2
    - Upload Speed: "115200"
    - Erase Flash: "Only Sketch"
    - Port: "ESP01-DSMR at <-- IP address →"
</code>

For more information see: “https://willem.aandewiel.nl/index.php/slimme-meter-uitlezer/”

![Actual](/images/DSMRactual.png)

