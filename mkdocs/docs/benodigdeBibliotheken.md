## Benodigde Bibliotheken

Voor de **DSMRlogger2HTTP** firmware zijn de volgende bibliotheken nodig:

### dsmr
Deze library is ontwikkeld door *Matthijs Kooijman* en vormt het hart van de
DSMR-logger. Je kunt de bibliotheek [hier](https://github.com/matthijskooijman/arduino-dsmr)
(https://github.com/matthijskooijman/arduino-dsmr) downloaden.
De firmware is getest met `Version 0.1 - Commit f79c906 on 18 Sep 2018`.

### TimeLib
Deze is door *Paul Stoffregen* ontwikkeld. Je kunt hem [hier](https://github.com/PaulStoffregen/Time)
(https://github.com/PaulStoffregen/Time) downloaden.

### WiFiManager
Je kunt de, door *Tzapu* ontwikkelde, bibliotheek [hier](https://github.com/tzapu/WiFiManager)
(https://github.com/tzapu/WiFiManager) downloaden.
De DSMR-logger firmware is getest met `version 0.14.0` van deze bibliotheek.

### TelnetStream
Deze bibliotheek is door *Jan Drassy* ontwikkeld.
Je kunt deze bibliotheek [hier](https://github.com/jandrassy/TelnetStream) 
(https://github.com/jandrassy/TelnetStream) downloaden.
De firmware is getest met `version 0.0.1`.

**Let op:** De installatie van deze bibliotheek gaat net als de andere bibliotheken. Een update 
kan echter pas geïnstalleerd worden als éérst de map `TelnetStream-master` uit de
map `Libraries` wordt verwijderd! 

### Overige libraries
Onderstaande libraries zijn onderdeel van de `ESP8266 Core` **en moeten dus niet handmatig
geïnstalleerd worden**!

	* ESP8266WiFi    
	* ESP8266WebServer
	* FS           
	* ArduinoOTA  

<br>

---
<center style="font-size: 70%">[Slimme Meter]</center><br>
![](img/ISKRA.jpg)

