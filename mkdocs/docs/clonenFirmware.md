## Clonen Firmware
De **DSMRlogger2HTTP** firmware staat 
[hier](https://github.com/mrWheel/DSMRlogger2HTTP)
(https://github.com/mrWheel/DSMRlogger2HTTP).

Er zijn twee manieren om de firmware te clonen.

1. download de repository als een `.zip` file
2. gebruik `git`

Als je niet handig bent met `git` raad ik je aan de repository als een `.zip` file te downloaden.

<hr>
### Download .zip file

![](img/GIT_Clone1.png)

Klik op de groene knop [<span style="background: green; color: white;">Clone or Download</span>]

en selecteer [<span style="background: blue; color: white;">Download ZIP]

![](img/GIT_Clone2.png)

Er volgt een scherm als dit:

![](img/GIT_SaveZIP.png)

1. Bewaar het `.zip` bestand op een plek op je computer waar je hem terug kunt vinden.
2. Unzip het `DSMRlogger2HTTP-master.zip` bestand in de ArduinoIDE `Sketchbook Location`.
3. Rename de map `DSMRlogger2HTTP-master` naar `DSMRlogger2HTTP` (dus zonder `-master`)

Ga verder naar [DSMRlogger2HTTP Sketch openen](#dsmrlogger2http-sketch-openen)

<hr>
### git clone
Om de repository met `git` te kunnen clonen moet je er éérst voor zorgen dat je `git` 
op je systeem hebt staan. Hoe je dat moet doen valt buiten de scope van deze documentatie
maar met 
[Google](https://git-scm.com/book/nl/v1/Aan-de-slag-Git-installeren)
kun je alles vinden over hoe je dit, voor jouw systeem, moet doen.

Voor nu ga ik ervan uit dat je `git` op je systeem hebt staan en dat je weet hoe je
ermee moet werken.

Ga naar de Arduino `Sketchbook location` (de map waar al je Sketches in staan, 
[kijk hier](installatieESP8266core.md))
en toets het volgende commando in:
```
git clone https://github.com/mrWheel/DSMRlogger2HTTP.git
```
That's it!

In `Sketchbook location` staat hierna een nieuwe map met de naam `DSMRlogger2HTTP`. 

<hr>
### DSMRlogger2HTTP Sketch openen
Start de ArduinoIDE ***opnieuw*** op en klik op het `open` icoon 
<center><img src="../img/IDE_Load-a.png" width="500" /></center>.

Selecteer in het `drop-down` menu ..

<center>![](img/IDE_Load-b.png)</center>

.. de sketch `DSMRlogger2HTTP`<br>
Er verschijnt een nieuw editor window met de firmware van de DSMRlogger!

<center>![](img/IDE_Load-c.png)</center>

<br>

----
<center style="font-size: 70%">[DSMR 3D]</center><br>
![](img/DSMR_3D_v3.png)
