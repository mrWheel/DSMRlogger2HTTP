## Introductie
Dit is de documentatie voor de DSMR-logger v2 en v3 en de DSMRlogger2HTTP firmware.   

Ga voor de volledige project beschrijving naar 
<a href="https://willem.aandewiel.nl/index.php/2018/08/28/slimme-meter-uitlezer/" target="_blank">
www.aandewiel.nl</a>

#### Het idee
Zo langzamerhand zijn alle huizen in Nederland uitgerust met een “Slimme Meter”. 
Deze meters beschikken over een "P1" poort, deze poort kan worden uitgelezen. 
Dit project stelt je in staat dit zelf thuis uit te lezen en de gegevens op een 
door jouw gewenst formaat te presenteren dmv een "DSMR logger". Er zijn 
commerciele P1 oplossingen beschikbaar, dit project geeft je aanzienlijk meer 
controle over de kosten en de gegevens. 

<a href="https://nl.wikipedia.org/wiki/Slimme_meter" target="_blank">DSMR</a> 
is een acroniem voor *Dutch Smart Meter Requirements*

De doelstellingen van de DSMR logger project zijn:

- Moet eenvoudig en goedkoop gemaakt kunnen worden;
- Moet “Over The Air” ge-update kunnen worden;
- Moet een beperkte historie hebben die intern opgeslagen wordt;
- Moet via een externe terminal benaderd kunnen worden (telnet);
- Moet via een browser de gegevens kunnen presenteren;
- Moet via een REST API de gegevens kunnen leveren;
- Moet gegevens eenvoudig verder laten verwerken.


De DSMR-logger is een hardware en software systeem waarmee de Slimme Meter 
(vanaf *DSMR* versie 4.0) kan worden uitgelezen. De uitgelezen data (telegrammen) 
worden in de DSMR-logger opgeslagen en kunnen in de vorm van tabellen en 
charts, via WiFi, op een computer of tablet worden weergegeven.

<div class="admonition note">
<p class="admonition-title">Begrippen</p>
In dit document worden de volgende begrippen gebruikt:
<table>
<tr><th align="left">Begrip</th><th align="left">Omschrijving</th></tr>
<tr>
	<td style="vertical-align:top">DSMR-logger</td>
	<td>de Hardware</td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger v2 (of Versie 2)</td>
	<td>Versie 2 van de Hardware
            <br>Dit is de hardware van de DSMR-logger die is uitgerust 
            <br>met een ESP-01 (Black/S) bordje.
	    <br>Voor deze versie is de firmware DSMRlogger2HTTP bedoeld.
        </td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger v3 (of Versie 3)</td>
	<td>Versie 3 van de Hardware
	    <br>Verder gelijk aan Versie 2.
        </td>
</tr>
<tr>
	<td style="vertical-align:top">DSMRlogger2HTTP</td>
	<td>De firmware voor de DSMR-logger v2 en v3</td></tr>
<tr>
	<td style="vertical-align:top">ESP-01</td>
	<td>Een bordje met een ESP8266 processor
	    <br>en 512KB flash geheugen
	    <br>Dit bordje is <b>niet</b> geschikt voor dit project
	</td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01 (Black)</td>
	<td>Een ESP-01 met 1MB flash geheugen
            <br>Dit bordje heeft een rode Power Led en een blauwe Led op GPIO-01
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01S</td>
	<td>Een ESP-01 met 1MB flash geheugen
            <br>Dit bordje heeft alleen een blauwe Led op GPIO-02
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01(Black/S)</td>
	<td>Een ESP-01 Black of ESP-01S
            <br>DSMR-logger v2 en v3 maken gebruik van dit bordje
        </td>
</tr>
</table>
</div>
Een volledige beschrijving van dit project kun je hier:
<a href="https://willem.aandewiel.nl/index.php/2018/08/28/slimme-meter-uitlezer/" target="_blank">
www.aandewiel.nl</a>
vinden.



<br>

---
<center  style="font-size: 70%">[DSMRlogger v2]</center><br>
<center>![](img/DSMRlogger_Build2.jpg)</center>

