/*
***************************************************************************  
**  Program  : OnderhoudStuff, part of DSMRlogger2HTTP
**  Version  : v0.7.6
**
**  Mostly stolen from https://www.arduinoforum.de/User-Fips
**  See also https://www.arduinoforum.de/arduino-Thread-SPIFFS-DOWNLOAD-UPLOAD-DELETE-Esp8266-NodeMCU
**
***************************************************************************      
*/

File fsUploadFile;                      // Stores de actuele upload

void handleRoot() {                     // HTML onderhoud
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  String onderhoudHTML;
  onderhoudHTML += "<!DOCTYPE HTML><html lang='en'>";
  onderhoudHTML += "<head>";
  onderhoudHTML += "<meta charset='UTF-8'>";
  onderhoudHTML += "<meta name= viewport content='width=device-width, initial-scale=1.0,' user-scalable=yes>";
  onderhoudHTML += "<style type='text/css'>";
  onderhoudHTML += "body {background-color: lightgray;}";
  onderhoudHTML += "</style>";
  onderhoudHTML += "</head>";
  onderhoudHTML += "<body><h1>ESP01-DSMR Onderhoud</h1><h2>Upload, Download of Verwijder</h2>";

  onderhoudHTML += "<hr><h3>Selecteer bestand om te downloaden:</h3>";
  if (!SPIFFS.begin())  TelnetStream.println("SPIFFS failed to mount !\r\n");

  Dir dir = SPIFFS.openDir("/");         // List files on SPIFFS
  while (dir.next())  {
    onderhoudHTML += "<a href ='";
    onderhoudHTML += dir.fileName();
    onderhoudHTML += "?download='>";
    onderhoudHTML += "SPIFFS";
    onderhoudHTML += dir.fileName();
    onderhoudHTML += "</a> ";
    onderhoudHTML += formatBytes(dir.fileSize()).c_str();
    onderhoudHTML += "<br>";
  }

  onderhoudHTML += "<p><hr><h3>Sleep bestand om te verwijderen:</h3>";
  onderhoudHTML += "<form action='/onderhoud' method='POST'>Om te verwijderen bestand hierheen slepen ";
  onderhoudHTML += "<input type='text' style='height:45px; font-size:15px;' name='Delete' placeholder='Bestand hier in-slepen' required>";
  onderhoudHTML += "<input type='submit' class='button' name='SUBMIT' value='Verwijderen'>";
  onderhoudHTML += "</form><p><br>";
  
  onderhoudHTML += "<hr><h3>Bestand uploaden:</h3>";
  onderhoudHTML += "<form method='POST' action='/onderhoud/upload' enctype='multipart/form-data' style='height:35px;'>";
  onderhoudHTML += "<input type='file' name='upload' style='height:35px; font-size:13px;' required>";
  onderhoudHTML += "<input type='submit' value='Upload' class='button'>";
  onderhoudHTML += "</form><p><br>";
  onderhoudHTML += "<hr>Omvang SPIFFS: ";
  onderhoudHTML += formatBytes(fs_info.totalBytes).c_str();      
  onderhoudHTML += "<br>Waarvan in gebruik: ";
  onderhoudHTML += formatBytes(fs_info.usedBytes).c_str();      
  onderhoudHTML += "<p>";

  onderhoudHTML += "<hr>";
  onderhoudHTML += "<div style='width: 30%'>";
  onderhoudHTML += "  <form style='float: left;' action='/ReBoot' method='POST'>ReBoot DSMRlogger ";
  onderhoudHTML += "    <input type='submit' class='button' name='SUBMIT' value='ReBoot'>";
  onderhoudHTML += "  </form>";

  onderhoudHTML += "  <form style='float: right;' action='/' method='POST'> &nbsp; Exit Onderhoud ";
  onderhoudHTML += "   <input type='submit' class='button' name='SUBMIT' value='Exit'>";
  onderhoudHTML += "  </form>";
  onderhoudHTML += "</div>";
  onderhoudHTML += "<div style='width: 80%'>&nbsp;</div>";
  
  onderhoudHTML += "</body></html>\r\n";

  server.send(200, "text/html", onderhoudHTML);
}

String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " Byte";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + " KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + " MB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void handleReBoot() {
  String redirectHTML = "";

  redirectHTML += "<!DOCTYPE HTML><html lang='en-US'>";
  redirectHTML += "<head>";
  redirectHTML += "<meta charset='UTF-8'>";
  redirectHTML += "<style type='text/css'>";
  redirectHTML += "body {background-color: lightgray;}";
  redirectHTML += "</style>";
  redirectHTML += "<title>Redirect to DSMRlogger</title>";
  redirectHTML += "</head>";
  redirectHTML += "<body><h1>ESP01-DSMR Onderhoud</h1>";
  redirectHTML += "<h3>Rebooting ESP01-DSMR</h3>";
  redirectHTML += "<br><div style='width: 500px; position: relative; font-size: 25px;'>";
  redirectHTML += "  <div style='float: left;'>Redirect over &nbsp;</div>";
  redirectHTML += "  <div style='float: left;' id='counter'>20</div>";
  redirectHTML += "  <div style='float: left;'>&nbsp; seconden ...</div>";
  redirectHTML += "  <div style='float: right;'>&nbsp;</div>";
  redirectHTML += "</div>";
  redirectHTML += "<!-- Note: don't tell people to `click` the link, just tell them that it is a link. -->";
  redirectHTML += "<br><br><hr>If you are not redirected automatically, click this <a href='/'>ESP01-DSMR</a>.";
  redirectHTML += "  <script>";
  redirectHTML += "      setInterval(function() {";
  redirectHTML += "          var div = document.querySelector('#counter');";
  redirectHTML += "          var count = div.textContent * 1 - 1;";
  redirectHTML += "          div.textContent = count;";
  redirectHTML += "          if (count <= 0) {";
  redirectHTML += "              window.location.replace('/'); ";
  redirectHTML += "          } ";
  redirectHTML += "      }, 1000); ";
  redirectHTML += "  </script> ";
  redirectHTML += "</body></html>\r\n";
  
  server.send(200, "text/html", redirectHTML);
  
  TelnetStream.println("ReBoot DSMRlogger ..");
  TelnetStream.flush();
  if (debug) {
    Serial.println("ReBoot DSMRlogger ..");
    Serial.flush();
  }
  delay(1000);
  ESP.reset();
  
} // handleReBoot()


bool handleFileRead(String path) {
  TelnetStream.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  TelnetStream.println(contentType);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
  
} // handleFileRead()


void handleFileDelete() {                               
  String file2Delete, hostNameURL, IPaddressURL;                     
  if (server.args() == 0) return handleRoot();
  if (server.hasArg("Delete")) {
    file2Delete = server.arg("Delete");
    file2Delete.toLowerCase();
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())    {
      String path = dir.fileName();
      path.replace(" ", "%20"); path.replace("ä", "%C3%A4"); path.replace("Ä", "%C3%84"); path.replace("ö", "%C3%B6"); path.replace("Ö", "%C3%96");
      path.replace("ü", "%C3%BC"); path.replace("Ü", "%C3%9C"); path.replace("ß", "%C3%9F"); path.replace("€", "%E2%82%AC");
      hostNameURL   = "http://" + String(HOSTNAME) + ".local" + path + "?download=";
      hostNameURL.toLowerCase();
      IPaddressURL  = "http://" + WiFi.localIP().toString() + path + "?download=";
      IPaddressURL.toLowerCase();
    //if (server.arg("Delete") != "http://" + WiFi.localIP().toString() + path + "?download=" )
      if ( (file2Delete != hostNameURL ) && (file2Delete != IPaddressURL ) ) {
        continue;
      }
      SPIFFS.remove(dir.fileName());
      String header = "HTTP/1.1 303 OK\r\nLocation:";
      header += server.uri();
      header += "\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      return;
    }
    String onderhoudHTML;                                    
    onderhoudHTML += "<!DOCTYPE HTML><html lang='de'><head><meta charset='UTF-8'><meta name= viewport content=width=device-width, initial-scale=1.0, user-scalable=yes>";
    onderhoudHTML += "<meta http-equiv='refresh' content='3; URL=";
    onderhoudHTML += server.uri();
    onderhoudHTML += "'><style>body {background-color: powderblue;}</style></head>\r\n<body><center><h2>Bestand niet gevonden</h2>wacht 3 seconden...</center>";
    server.send(200, "text/html", onderhoudHTML );
  }
  
} // handleFileDelete()


void handleFileUpload() {                                 
  if (server.uri() != "/onderhoud/upload") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    TelnetStream.print("handleFileUpload Name: "); TelnetStream.println(filename);
    if (filename.length() > 30) {
      int x = filename.length() - 30;
      filename = filename.substring(x, 30 + x);
    }
    if (!filename.startsWith("/")) filename = "/" + filename;
    TelnetStream.print("handleFileUpload Name: "); TelnetStream.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    TelnetStream.print("handleFileUpload Data: "); TelnetStream.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    yield();
    TelnetStream.print("handleFileUpload Size: "); TelnetStream.println(upload.totalSize);
    handleRoot();
  }
  
} // handleFileUpload()


//void formatSpiffs() {       // Format SPIFFS
//  SPIFFS.format();
//  handleRoot();
//}
