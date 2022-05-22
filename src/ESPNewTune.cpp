#include "ESPNewTune.h"
//#include <Neer.h>

/*
  Loads STA Credentials into memory
*/

void ESPNewTuneClass::load_sta_credentials(){
  // Get saved WiFi Credentials
  #if defined(ESP8266)
    station_config config = {};
    wifi_station_get_config(&config);
    for(int i=0; i < strlen((char*)config.ssid); i++){
      _sta_ssid += (char)config.ssid[i];
    }
    for(int i=0; i < strlen((char*)config.password); i++){
      _sta_password += (char)config.password[i];
    }
  #elif defined(ESP32)
    Preferences preferences;
    //_timezoneName = preferences.getString("timezone", "");
    for(int i=0;i<ALLOWED_AP_COUNT;i++){
      preferences.begin("espnewtune", false);
      APvals[i]._sta_ssid = preferences.getString("ssid_"+i, "");
      APvals[i]._sta_password = preferences.getString("password_"+i, "");
      preferences.end();
      ESPNEWTUNE_SERIAL(APvals[i]._sta_ssid);
    }

  #endif
}


/*
  Start Captive Portal and Attach DNS & Webserver
*/
bool ESPNewTuneClass::start_portal(){
  bool confi_saved = false;
  
  _server->reset();

  ESPNEWTUNE_SERIAL("Starting Captive Portal\n");
  // Try Connecting Station
  WiFi.mode(WIFI_AP_STA);


  // Add handlers for AP CONNECT AND DISCONNECT
  //int *cuc = &_conn_user_count;
  WiFiEventId_t eventID = WiFi.onEvent([&](WiFiEvent_t event, WiFiEventInfo_t info){
    _conn_user_count = _conn_user_count + 1;
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);

  
  WiFiEventId_t eventID1 = WiFi.onEvent([&](WiFiEvent_t event, WiFiEventInfo_t info){
    _conn_user_count = _conn_user_count - 1;
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);


  WiFiEventId_t eventID2 = WiFi.onEvent([&](WiFiEvent_t event, WiFiEventInfo_t info){
    ESPNEWTUNE_SERIAL("Started Captive Portal at : "+WiFi.softAPIP().toString()+"\n");
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_START);


  // Start Access Point
  WiFi.softAP(_auto_connect_ssid.c_str(), _auto_connect_password.c_str());

  /*
  if(_con_mode == "AUTO"){
    if(APvals[0]._sta_ssid != ""){
      //WiFi.begin(_sta_ssid.c_str(), _sta_password.c_str());
       for(int i=0;i<ALLOWED_AP_COUNT;i++){
         if(APvals[i]._sta_ssid != ""){
           wifiMulti.addAP(APvals[i]._sta_ssid.c_str(),APvals[i]._sta_password.c_str())
         }
       }
       wifiMulti.run();
  }  
 */

  // Start our DNS Server
  _dns = new DNSServer();
  _dns->setErrorReplyCode(DNSReplyCode::NoError);
  _dns->start(53, "*", WiFi.softAPIP());

  auto scanGET = _server->on("/espnewtune/scan", HTTP_GET, [&](AsyncWebServerRequest *request){
     ESPNEWTUNE_SERIAL("Starting Scan\n");
    String json = "[";
    int n = WiFi.scanComplete();
    if(n == WIFI_SCAN_FAILED){
      WiFi.scanNetworks(true);
      return request->send(202);
    }else if(n == WIFI_SCAN_RUNNING){
      return request->send(202);
    }else{
      for (int i = 0; i < n; ++i){
        String ssid = WiFi.SSID(i);
        // Escape invalid characters
        ssid.replace("\\","\\\\");
        ssid.replace("\"","\\\"");
        json+="{";
        json+="\"name\":\""+ssid+"\",";
        #if defined(ESP8266)
          json+="\"open\":"+String(WiFi.encryptionType(i) == ENC_TYPE_NONE ? "true": "false");
        #elif defined(ESP32)
          json+="\"open\":"+String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "true": "false");
        #endif
        

        bool hasv = false;
        for(int i=0; i < ALLOWED_AP_COUNT;i++){
            if(APvals[i]._sta_ssid == ssid){
                json+=",\"inlist\":true";
                json+=",\"password\":\""+APvals[i]._sta_password+"\"";
                hasv = true;
                break;
            }
        }
        if(!hasv){
            json+=",\"inlist\":false";
            json+=",\"password\":\"\"";
        } 
        hasv = false;
                
        json+="}";
        if(i != n-1) json+=",";
      }
      WiFi.scanDelete();
      /*
      if(WiFi.scanComplete() == -2){
        WiFi.scanNetworks(true);
      }
      */
    }
    json += "]";
    request->send(200, "application/json", json);
    json = String();
    ESPNEWTUNE_SERIAL("End Scan\n");
  });

  
  auto erasePOST = _server->on("/espnewtune/erase", HTTP_POST, [&](AsyncWebServerRequest *request){
    // Get FormData
    String ssid = request->hasParam("ssid", true) ? request->getParam("ssid", true)->value().c_str() : "";
    String optyp = request->hasParam("optyp", true) ? request->getParam("optyp", true)->value().c_str() : "";

    int s_offset = 0;   

    if (optyp == "FEW") { 
      for(int i=0;i<ALLOWED_AP_COUNT;i++){
        if(APvals[i]._sta_ssid == ssid){ 
          s_offset = s_offset + 1;
        } else {
            if(s_offset >0) {
              APvals[i-s_offset]._sta_ssid = APvals[i]._sta_ssid;
              APvals[i-s_offset]._sta_password = APvals[i]._sta_password;
            }
        }
      }
    }

    int counter_start = 0;
    if(s_offset >0) {
      counter_start = ALLOWED_AP_COUNT-s_offset;
    }

    for(int i = counter_start; i < ALLOWED_AP_COUNT;i++ ){
      APvals[i]._sta_ssid = "";
      APvals[i]._sta_password = "";
    }
    
    if(erase()){
      ESPNEWTUNE_SERIAL("Erasing of "+ optyp + "Completed");
      request->send(200, "application/json", "{\"message\":\"Erase Completed. Rebooting...\"}");
    }else{
        ESPNEWTUNE_SERIAL("Erasing of "+ optyp + "Failed");
        return request->send(500, "application/json", "{\"message\":\"Error while Erasing WiFi Credentials...\"}");
    }

  });


  // Accept incomming WiFi Credentials
  auto connectPOST = _server->on("/espnewtune/connect", HTTP_POST, [&](AsyncWebServerRequest *request){
    ESPNEWTUNE_SERIAL("Inside Connect\n");

    if(!_conn_resp_sent){              
      if(WiFi.status() == WL_CONNECTED){ 
         ESPNEWTUNE_SERIAL("SSID is valid so can proceed with reboot\n");  
        _conn_resp_sent = true;      
        return request->send(202, "application/json", "{\"message\":\"Access Point Credentials Checked. Proceeding to Save...\",\"code\":\"SUC\"}");        
      }else if(WiFi.status() == WL_CONNECT_FAILED){
        ESPNEWTUNE_SERIAL("Connecting to wifi is failed.  Ask user to check\n");     
        _conn_resp_sent = true;      
        return request->send(200, "application/json", "{\"message\":\"Credentials Saved. Rebooting...\",\"code\":\"FAI\"}");             
      } else {
        ESPNEWTUNE_SERIAL("Inside faile of connect\n");              
        return request->send(202, "application/json", "{\"message\":\"Unable to connect to SSID... Unknown issue\"}");
      }   

    } 
    
    if (!_configured){
        // Get FormData
        String ssid = request->hasParam("ssid", true) ? request->getParam("ssid", true)->value().c_str() : "";
        String password = request->hasParam("password", true) ? request->getParam("password", true)->value().c_str() : "";
        String optyp = request->hasParam("optyp", true) ? request->getParam("optyp", true)->value().c_str() : "";



        ESPNEWTUNE_SERIAL("ssid: "+ssid+"\n");
        ESPNEWTUNE_SERIAL("password"+password+"\n");
        ESPNEWTUNE_SERIAL("optyp"+optyp+"\n");

        int ok = 0;       
        
        if(optyp != "CON"){

          if(ssid.length() <= 0){
            return request->send(403, "application/json", "{\"message\":\"Invalid SSID\"}");
          }

          if(ssid.length() > 32 || password.length() > 64){
            return request->send(403, "application/json", "{\"message\":\"Credentials exceed character limit of 32 & 64 respectively.\"}");
          }    
        }

        #if defined(ESP8266)
          if(ok == 1){
        #elif defined(ESP32)
          if(ok == ESP_OK){
        #endif        
            ESPNEWTUNE_SERIAL("Starting WIFI\n");
            _configured = true;
            _conn_resp_sent = false; 
            WiFi.begin(ssid.c_str(), password.c_str());
            ESPNEWTUNE_SERIAL("Starting WIFI completed\n");         
            if(optyp != "CON"){
              return request->send(202, "application/json", "{\"message\":\"Starting to connect to WIFI... Comback for status check...\"}");
            } else {
              return request->send(200, "application/json", "{\"message\":\"Starting to connect to WIFI [con only]...\",\"code\":\"SUC\"}");        
            }            
          }else{
            //ESPNEWTUNE_SERIAL("WiFi config failed with: %d\n", ok);
            ESPNEWTUNE_SERIAL("Starting WIFI Failed\n");
            return request->send(500, "application/json", "{\"message\":\"Error while saving WiFi Credentials: "+String(ok)+"\"}");
          }      
      
        } else {       
        
            // Get FormData
            String ssid = request->hasParam("ssid", true) ? request->getParam("ssid", true)->value().c_str() : "";
            String password = request->hasParam("password", true) ? request->getParam("password", true)->value().c_str() : "";
            String optyp = request->hasParam("optyp", true) ? request->getParam("optyp", true)->value().c_str() : "";
            String timezone = request->hasParam("timezone", true) ? request->getParam("timezone", true)->value().c_str() : "";

            ESPNEWTUNE_SERIAL("Starting WIFI save\n");

              /*
              if(_timezoneName =="" || strcmp(timezone,_timezoneName)){
                _timezoneName = timezone;
                preferences.begin("espnewtune", false);
                preferences.putString("timezone", timezone.c_str());                
                preferences.end();   
                configTzTime(getTZ(timezone), "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");                        
              }
              */

            // Save WiFi Credentials in Flash      
            #if defined(ESP8266)
              struct station_config	config = {};
              ssid.toCharArray((char*)config.ssid, sizeof(config.ssid));
              password.toCharArray((char*)config.password, sizeof(config.password));
              config.bssid_set = false;
              ok = wifi_station_set_config(&config);
            #elif defined(ESP32)
              Preferences preferences;
              String searchstr = "";
              if (optyp == "UPD") {
                searchstr = ssid;
              }

              for(int i=0;i<ALLOWED_AP_COUNT;i++){
                if(APvals[i]._sta_ssid == searchstr){
                  preferences.begin("espnewtune", false);
                  preferences.putString("ssid_"+i, ssid.c_str());
                  preferences.putString("password_"+i, password.c_str());
                  preferences.end();
                  APvals[i]._sta_ssid = ssid;
                  APvals[i]._sta_password = password;                   
                  break;
                }
              }
            #endif    
            _configured = false;
            confi_saved = true;
          return request->send(200, "application/json", "{\"message\":\"Access Point Configured\",\"code\":\"SUC\"}");        
        }
    
  });

  //_server->addHandler(new SPIFFSEditor(LittleFS, "admin", "admin"));

  auto wifimodeGET = _server->on("/espnewtune/wifimode", HTTP_GET, [&](AsyncWebServerRequest *request){
    return request->send(200, "application/json", "{\"message\":\""+ getCurrentMode() +"\",\"code\":\"SUC\"}");    
  });

  auto indexGET = _server->on("/espnewtune", HTTP_GET, [&](AsyncWebServerRequest *request){
    /*
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESPNEWTUNE_HTML, ESPNEWTUNE_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    */

    ESPNEWTUNE_SERIAL("Inside /espnewtune");
    request->send(LittleFS, "/index.html", "text/html", false);
  });

  _server->on("/bundle.js", HTTP_GET, [&](AsyncWebServerRequest *request){
    ESPNEWTUNE_SERIAL("bundle.js inside espnew requested");
    //request->send(LittleFS, "/bundle.js", "text/js", false);
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/bundle.js", "text/js", false);
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    /*
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/js", ESPNEWTUNE_JS, ESPNEWTUNE_JS_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    */
  });

  _server->on("/bundle.css", HTTP_GET, [&](AsyncWebServerRequest *request){
    ESPNEWTUNE_SERIAL("bundle.css inside espnew requested");
    //request->send(LittleFS, "/bundle.css", "text/css", false);
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/bundle.css", "text/css", false);
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    /*
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", ESPNEWTUNE_CSS, ESPNEWTUNE_CSS_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    */
  });


  //_server->onNotFound([&](AsyncWebServerRequest *request){
    /*
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESPCONNECT_HTML, ESPCONNECT_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    */
  /*  request->send(LittleFS, "/index.html", "text/html", false);
  });
  */

  _server->onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });

  auto portalRewrite = _server->rewrite("/", "/espnewtune?id=kum").setFilter(ON_AP_FILTER);
  ESPNEWTUNE_SERIAL("Portal rewrite done\n");
  // Begin Webserver
  _server->begin();

  unsigned long lastMillis = millis();

  while((WiFi.status() != WL_CONNECTED && (unsigned long)(millis() - lastMillis) < _auto_connect_timeout) || _conn_user_count > 0){
    _dns->processNextRequest();
    yield();
    
    /*
    if(confi_saved) {   
      if(WiFi.status() == WL_CONNECTED && _conn_resp_sent){
        _conn_user_count = 0;
        _conn_resp_sent = true;  // Make forceful disconnect as configuration is done
      } else if((WiFi.status() != WL_CONNECTED || (unsigned long)(millis() - lastMillis) > DEFAULT_CONNECTION_TIMEOUT)){
        confi_saved = false;
        lastMillis = millis();        
      }
    }
    */
   if(confi_saved) { 
     _conn_user_count = 0;  // Make forceful disconnect as configuration is done
   }

  }

  if(WiFi.status() != WL_CONNECTED){
    ESPNEWTUNE_SERIAL("Portal timed out\n");
  }else{
    ESPNEWTUNE_SERIAL("Connected to STA\n");
  }


  _server->removeHandler(&indexGET);
  _server->removeHandler(&scanGET);
  _server->removeHandler(&connectPOST);
  _server->removeHandler(&erasePOST);  
  _server->removeHandler(&wifimodeGET);
  _server->removeRewrite(&portalRewrite);
  
  
  
  /*
  _server->onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
  */

  _dns->stop();
  delete _dns;
  _dns = nullptr;
  
  ESPNEWTUNE_SERIAL("Closed Portal\n");
  WiFi.softAPdisconnect(true);
  if(confi_saved){

    WiFi.mode(WIFI_STA);   

    if(APvals[0]._sta_ssid != ""){      
      for(int i=0;i<ALLOWED_AP_COUNT;i++){
        if(APvals[i]._sta_ssid != ""){
          wifiMulti.addAP(APvals[i]._sta_ssid.c_str(),APvals[i]._sta_password.c_str());
        } else {
          break; //After "" there exists no saved Acess Points.  So cut short the loop.
        }
      }
    }
    wifiMulti.run();
    //WiFi.begin(_sta_ssid.c_str(), _sta_password.c_str());
    return true;
  }else{
    return false;
  }
}


/*
  Set Custom AP Credentials
*/
void ESPNewTuneClass::autoConnect(const char* ssid, const char* password, unsigned long timeout){
  _auto_connect_ssid = ssid;
  _auto_connect_password = password;
  _auto_connect_timeout = timeout;
}


/*
  Connect to saved WiFi Credentials
*/
bool ESPNewTuneClass::begin(AsyncWebServer* server, unsigned long timeout){
  _server = server;
  _con_mode = "AUTO";
  load_sta_credentials();

/* this is for neer*/
  //Neer.begin(server);  
/**/

/**/
//First time setup
 // APvals[0]._sta_ssid = "Niruhome";
  //APvals[0]._sta_password = "Nidha123";
/**/

  if(APvals[0]._sta_ssid != ""){
    ESPNEWTUNE_SERIAL("STA Pre-configured:\n");
    ESPNEWTUNE_SERIAL("Connecting to STA [");

    // Try connecting to STA
    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.mode(WIFI_STA);

    if(APvals[0]._sta_ssid != ""){      
      for(int i=0;i<ALLOWED_AP_COUNT;i++){
        if(APvals[i]._sta_ssid != ""){
          wifiMulti.addAP(APvals[i]._sta_ssid.c_str(),APvals[i]._sta_password.c_str());
        }
      }
    }
    wifiMulti.run();
    //WiFi.begin(_sta_ssid.c_str(), _sta_password.c_str());

    // Check WiFi connection status till timeout
    unsigned long lastMillis = millis();
    while(WiFi.status() != WL_CONNECTED && (unsigned long)(millis() - lastMillis) < timeout){
      Serial.print("#");
      delay(500);
      yield();
    }
    Serial.print("]\n");

    if(WiFi.status() != WL_CONNECTED){
      ESPNEWTUNE_SERIAL("Connection to STA Falied [!]\n");
    } 
  }

  // Start Captive Portal if not connected to STA
  if(WiFi.status() != WL_CONNECTED){
    // Start captive portal
    return start_portal();
  }else{
    ESPNEWTUNE_SERIAL("Connected to STA\n");
    ESPNEWTUNE_SERIAL("SSID: "+WiFi.SSID()+"\n");
    return true;
  }
}

    

/*
  Start AP on Demend
*/
bool ESPNewTuneClass::beginOnDemand(unsigned long timeout){
  
  #if defined(ESP32)
    WiFi.disconnect(true, true);
  #endif
  // Start Captive Portal if not connected to STA
  if(WiFi.status() != WL_CONNECTED){
    // Start captive portal
    return start_portal();
  }else{
    ESPNEWTUNE_SERIAL("Unable to disconnect wifi to being on Demand\n");
    return false;
  }
}


/*
  Erase Stored WiFi Credentials
*/
bool ESPNewTuneClass::erase(){
  #if defined(ESP8266)
// -----------  Added this section to erase stored wifi credentials - RLB -20220318  --------
    int ok = 0;
    struct station_config	config = {};
    memset(&config.ssid, 0, sizeof(config.ssid));
    memset(&config.password, 0, sizeof(config.password));
    config.bssid_set = false;
    ok = wifi_station_set_config(&config);
// -----------------------------------------------------------------------------------------
    return WiFi.disconnect(true);
  #elif defined(ESP32)
    Preferences preferences;
    for(int i=0;i<ALLOWED_AP_COUNT;i++){
      preferences.begin("espnewtune", false);
      preferences.putString("ssid_"+i, APvals[i]._sta_ssid.c_str());
      preferences.putString("password_"+i, APvals[i]._sta_password.c_str());
      preferences.end();
    }
    WiFi.disconnect(true, true);
    return true;
  #endif
}

/*
  Hearbeat function that keeps connected to available network
*/
void ESPNewTuneClass::keepalive()
{  
  //Keepalive parameters
  static ulong _checkwifi_timeout    = 0;
  static ulong _checkstatus_timeout  = 0;
  static ulong current_millis;

  #define WIFICHECK_INTERVAL    60000L

  #define HEARTBEAT_INTERVAL    120000L


  current_millis = millis();
  
  // Check WiFi every WIFICHECK_INTERVAL (1) minutes.
  if ((current_millis > _checkwifi_timeout) || (_checkwifi_timeout == 0))
  {
    //check_WiFi();
    if ( (WiFi.status() != WL_CONNECTED) )
    {
      ESPNEWTUNE_SERIAL("\nWiFi lost. Trying to connect to one of the configured point\n");
      connectMultiWiFi();
    }
    _checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((current_millis > _checkstatus_timeout) || (_checkstatus_timeout == 0))
  {
    heartBeatPrint();
    _checkstatus_timeout = current_millis + HEARTBEAT_INTERVAL;
  }
}



uint8_t ESPNewTuneClass::connectMultiWiFi()
{
  uint8_t status;
  ESPNEWTUNE_SERIAL("[");
  if(APvals[0]._sta_ssid != ""){  

    WiFi.mode(WIFI_STA); 

    for(int i=0;i<ALLOWED_AP_COUNT;i++){
      if(APvals[i]._sta_ssid != ""){
        wifiMulti.addAP(APvals[i]._sta_ssid.c_str(),APvals[i]._sta_password.c_str());
      }
    }

    
    status = wifiMulti.run();
    delay(2200);

    int i = 0;
    status = wifiMulti.run();

    while ( ( i++ < 20 ) && ( status != WL_CONNECTED ) )
    {
      status = wifiMulti.run();
      if ( WiFi.status() == WL_CONNECTED )
        break;
      else
        ESPNEWTUNE_SERIAL("*");
        delay(500);
    }
    ESPNEWTUNE_SERIAL("]\n");

    if ( status == WL_CONNECTED )
    {
      ESPNEWTUNE_SERIAL("WiFi connected after time: " + i);
      ESPNEWTUNE_SERIAL("SSID:" + WiFi.SSID() + ",RSSI= " + WiFi.RSSI());
      ESPNEWTUNE_SERIAL("Channel:"+ WiFi.channel() + ",IP address:" + WiFi.localIP() );
    } else{    
      ESPNEWTUNE_SERIAL("WiFi not connected");  
      #if ESP8266      
          ESP.reset();
      #else
          ESP.restart();
      #endif    
    }
    
  } else {
     ESPNEWTUNE_SERIAL("No WiFi configured yet... Starting Captive Portal");
     ESPNEWTUNE_SERIAL("]\n");
     beginOnDemand();
  }  
  return status;
}



void ESPNewTuneClass::heartBeatPrint()
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print(F("H"));        // H means connected to WiFi
  else
    Serial.print(F("F"));        // F means not connected to WiFi

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }

}


/* get tz
#define TZ_Asia_Kolkata	("IST-5:30")
#define TZ_Asia_Singapore	("<+08>-8")
https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/41f6479e85a9954b22afe9f4b816f96780e3d916/src/utils/TZ.h
*/

/*
const String ESPNewTuneClass::getTZ(String timezoneName){
  switch (timezoneName) {
      case "Asia/Kolkata": 
        return TZ_Asia_Kolkata;
        break;
      case "Asia/Singapore": 
        return TZ_Asia_Singapore;
        break;
      default: 
        return TZ_Asia_Kolkata;
        break;
  }      
  return "Asia/Kolkata";
}
*/

/*
  Return Connection Status
*/
bool ESPNewTuneClass::isConnected(){
  return (WiFi.status() == WL_CONNECTED);
}

String ESPNewTuneClass::getSSID(){
  return  WiFi.SSID();
}

String ESPNewTuneClass::getPassword(){
  return "";
}


/*
  Return Current mode we are in
*/

String ESPNewTuneClass::getCurrentMode(){
  
  wifi_mode_t curmode = WiFi.getMode();

  switch (curmode) {
    case 1: 
      return "WIFI_STA";
      break;
    case 2: 
      return "WIFI_AP";
      break;
    case 3: 
      return "WIFI_STA_AP";
      break;
    default: 
      return "";
      break;
  }
}

ESPNewTuneClass ESPNewTune;
