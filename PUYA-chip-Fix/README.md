***Achter de Engelse tekst staat dezelfde tekst in het Nederlands***

**\*\*\* With the ESP8266 core 2.5.0 the PUYA patch should not be necessery anymore \*\*\***

There are a lot of ESP-01 board available now which have a Flash Chip from PUYA.
This Flash chip is not compatible with the normaly used Flash chip.

If you flash the DSMRlogger2HTTP firmware to boards with this chip everything looks alright but ...
as soon as you try to read data from the SPIFFS filesystem everything goes wrong.

There are a lot of treads about this subject. Fore instance:

```
     https://github.com/letscontrolit/ESPEasy/issues/748
```

but there are more. Luckely @igrr has fixed the SPIFFS read problem with the PUYA flash chip!

```
     See: https://github.com/esp8266/Arduino/issues/4061
```

I have provided a patched version of the file ```Esp.cpp```. This file will work with all types of Flash
chips so just do the following:

### Install patched version Esp.cpp
Find the ```esp8266/hardware/esp8266/2.4.2/cores/esp8266``` map on your computer. On my iMac it is:

```   .../Arduino15/packages/esp8266/hardware/esp8266/2.4.2/cores/esp8266 ```

Go to this directory

``` cd .../Arduino15/packages/esp8266/hardware/esp8266/2.4.2/cores/esp8266 ```

and check whether there is a file called ```Esp.cpp```

now **rename** the file ```Esp.cpp``` to something like ```Esp.cpp.org``` so you can alway's return to this version.

Than, copy the file ```Esp.cpp``` from this repository to this directory

```copy <where-ever-you-downloaded-this>/Esp.cpp  . ``` (there is a &lt;dot&gt; at the end!)

or

```cp <where-ever-you-downloaded-this>/Esp.cpp  . ``` 

And now restart the Arduino-IDE and your good to program (also) boards with a PUYA Flash chip!

Once the ```Esp.cpp``` file is replaced, restart the IDE and flash your sketch using 
the "**Tools -> Erase Flash -> All Flash Contents**" option. 
After it's done flashing, go to "**Tools -> ESP8266 Sketch Data Upload**". This will upload all the 
files in the "**data**" subfolder of your sketch folder.

## Nederlands

**\*\*\* Met de ESP8266 core 2.5.0 zou de PUYA patch niet meer nodig moeten zijn \*\*\***

Er zijn een heleboel ESP-01 bordjes in omloop die een Flash chip van PUYA hebben.
Deze Flash chip is niet compatibel met de gewoonlijk aanwezige Flash chips.

Als je de DSMRlogger2HTTP firmware naar een borje met deze PUYA chip upload, dan lijkt alles in orde maar ...
zodra je van het SPIFFS bestandssysteem probeert te lezen gaat alles fout!

Er is een hoop discussie over dit onderwerp. Bijvoorbeeld:
```
     https://github.com/letscontrolit/ESPEasy/issues/748
     https://github.com/esp8266/Arduino/issues/4061
```
maar er zijn er meer. Gelukkig heeft @igrr een oplossing voor het SPIFFS lees probleem voor de PUYA chips!
```
     Zie: https://github.com/esp8266/Arduino/issues/4061
```
Ik heb in deze repository een gepatched bestand Esp.cpp opgenomen. Deze versie van Esp.cpp werkt met 
alle bekende versies Flash chips (dus niet alleen voor de PUYA chip!).

### Installeren van het gepatchte Esp.cpp bestand
Zoek de ```esp8266/hardware/esp8266/2.4.2/cores/esp8266``` directory op jouw computer. Op mijn iMac staat die hier:
```
   .../Arduino15/packages/esp8266/hardware/esp8266/2.4.2/cores/esp8266 
```
Ga naar deze directory
```
   cd .../Arduino15/packages/esp8266/hardware/esp8266/2.4.2/cores/esp8266 
```
Controleer of in deze map een bestand ```Esp.cpp``` staat!

vervolgens **rename** je dit bestand ```Esp.cpp``` in iets als ```Esp.cpp.org``` zodat 
je altijd naar deze versie terug kunt.

Daarna kopieer je het bestand ```Esp.cpp``` uit deze repository naar deze directory:

```copy <where-ever-you-downloaded-this>/Esp.cpp . ``` (er staat een &lt;punt&gt; aan het eind van de opdracht)

of:

```cp <where-ever-you-downloaded-this>/Esp.cpp . ``` 

Nu de Arduino-IDE opnieuw opstarten en je kunt (ook) bordjes met een PUYA Flash chip flashen!

Nu ```Esp.cpp``` bestand is vervangen door het ge-patch-te ```Esp.cpp``` bestand, moet je de IDE herstarten en beginnen
met het flash van een sketch met de "**Tools -> Erase Flash -> All Flash Contents**" optie! 
Als het flashen klaar is, kun je met "**Tools -> ESP8266 Sketch Data Upload**" de bestanden in de "**data**" sub-map van je sketch map naar de ESP-01 overzetten.
