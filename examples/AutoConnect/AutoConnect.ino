/*
  --------------------------------
  ESPConnect - AutoConnect Example
  --------------------------------

  Initializes ESPConnect and attaches itself to AsyncWebServer.
  
  Github & WiKi: https://github.com/ayushsharma82/ESPConnect
  Works with both ESP8266 & ESP32
*/

#include <Arduino.h>
#if defined(ESP8266)
  /* ESP8266 Dependencies */
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>elif defined(ESP32)
  /* ESP32 Dependencies */
  /*#include <WiFi.h>*/
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif
#include <ESPNewTune.h>

// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM       true
#define FORMAT_FILESYSTEM         false

 #include "FS.h"
 #include <LittleFS.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS

FS* filesystem =      &LittleFS;
#define FileFS        LittleFS
#define FS_Name       "LittleFS"

#define LED_BUILTIN       2
#define LED_ON            HIGH
#define LED_OFF           LOW

#include <SPIFFSEditor.h>
String http_username = "admin";
String http_password = "admin";



bool startcp = false;

AsyncWebServer server(80);

//format bytes
String formatBytes(size_t bytes) 
{
  if (bytes < 1024) 
  {
    return String(bytes) + "B";
  } 
  else if (bytes < (1024 * 1024)) 
  {
    return String(bytes / 1024.0) + "KB";
  } 
  else if (bytes < (1024 * 1024 * 1024)) 
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } 
  else 
  {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

void startcpcheck(){   
   if(startcp){
    startcp = false;
    /* 
      Begin connecting to previous WiFi
      or start autoConnect AP if unable to connect
    */
    if(ESPNewTune.beginOnDemand()){
      Serial.println("Connected to WiFi");
      Serial.println("IPAddress: "+WiFi.localIP().toString());
    }else{
      Serial.println("Failed to connect to WiFi");
    }   
   } 
}


void setintistartcp(){
  startcp = true;
}


void setup(){
  Serial.begin(115200);
  Serial.println();

  /*
  * Set up file server * 
  */

if (FORMAT_FILESYSTEM) 
    FileFS.format();

  // Format FileFS if not yet
  if (!FileFS.begin(true))
  {
    Serial.println(F("SPIFFS/LittleFS failed! Already tried formatting."));
  
    if (!FileFS.begin())
    {     
      // prevents debug info from the library to hide err message.
      delay(100);      
      Serial.println(F("LittleFS failed!. Please use SPIFFS or EEPROM. Stay forever"));
      while (true)
      {
        delay(1);
      }
    }
  }
  
  File root = FileFS.open("/");
  File file = root.openNextFile();
  
  while (file) 
  {
    String fileName = file.name();
    size_t fileSize = file.size();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    file = root.openNextFile();
  }
  
  Serial.println();

  /*
  Intialise 75h595
  */
 

  /* 
    Begin connecting to previous WiFi
    or start autoConnect AP if unable to connect
  */
  if(ESPNewTune.begin(&server)){
    Serial.println("Connected to WiFi");
    Serial.println("IPAddress: "+WiFi.localIP().toString());
  }else{
    Serial.println("Failed to connect to WiFi");
  }

  
  server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello from ESP");
    //request->send(FileFS, "/index.html", String(), false);
  });

/*
  server.on("/bundle.js", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("bundle.js requested");
    request->send(FileFS, "/bundle.js", String(), false);
  });

    server.on("/bundle.css", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("bundle.css requested");
    request->send(FileFS, "/bundle.css", String(), false);
    });
*/
    server.on("/cpo", HTTP_GET, [&](AsyncWebServerRequest *request){
      setintistartcp();
      request->send(200, "text/plain", "completed cpo");
    });

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.addHandler(new SPIFFSEditor(FileFS, http_username, http_password));
  server.begin();
}


void loop(){  
  ESPNewTune.keepalive();
  startcpcheck(); 



}