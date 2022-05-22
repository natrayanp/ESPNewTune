#include "ESPNewTune.h"


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
  bool configured = false;


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
  if(_dns == nullptr){
    ESPNEWTUNE_SERIAL("dns is null\n");
    } else {
    ESPNEWTUNE_SERIAL("dns is NOT NULL\n");
    }
  _dns = new DNSServer();
  _dns->setErrorReplyCode(DNSReplyCode::NoError);
  _dns->start(53, "*", WiFi.softAPIP());

  auto scanGET = _server->on("/espnewtune/scan", HTTP_GET, [&](AsyncWebServerRequest *request){
     ESPNEWTUNE_SERIAL("Starting Scan\n");

    String json = "[";
    int n = WiFi.scanComplete();
    ESPNEWTUNE_SERIAL("scan completed and its result: "+n+"\n");


    if(n == WIFI_SCAN_FAILED){
      WiFi.scanNetworks(true);
      return request->send(202);
    }else if(n == WIFI_SCAN_RUNNING){
      return request->send(202);
    }else{
      ESPNEWTUNE_SERIAL(n+"\n");
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
            if(APvals[i]._sta_ssid == ""){
                    break;
            }
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
      ESPNEWTUNE_SERIAL("After scan delete: ");
      ESPNEWTUNE_SERIAL(n+"\n");

      /*
      if(WiFi.scanComplete() == -2){
        WiFi.scanNetworks(true);
      }
      */

    }
    json += "]";
    ESPNEWTUNE_SERIAL("End Scan\n");
    return request->send(200, "application/json", json);
    //json = String();
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

    }

    #if defined(ESP8266)
      if(ok == 1){
    #elif defined(ESP32)
      if(ok == ESP_OK){
    #endif
        ESPNEWTUNE_SERIAL("Starting WIFI\n");
        WiFi.begin(ssid.c_str(), password.c_str());
        ESPNEWTUNE_SERIAL("Starting WIFI completed\n");
        configured = true;

        if(WiFi.status() != WL_CONNECTED){
          return request->send(500, "application/json", "{\"message\":\"Unable to connect to SSID: " +ssid +".  Recheck Password\"}");
        }else{
          ESPNEWTUNE_SERIAL("Starting WIFI completed\n");
          request->send(200, "application/json", "{\"message\":\"Credentials Saved. Rebooting...\"}");
        }
      }else{
        //ESPNEWTUNE_SERIAL("WiFi config failed with: %d\n", ok);
        ESPNEWTUNE_SERIAL("Starting WIFI Failed\n");
        return request->send(500, "application/json", "{\"message\":\"Error while saving WiFi Credentials: "+String(ok)+"\"}");
      }
  });

  auto indexGET = _server->on("/espnewtune", HTTP_GET, [&](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESPNEWTUNE_HTML, ESPNEWTUNE_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);

    ESPNEWTUNE_SERIAL("Inside /espnewtune");
    //request->send(LittleFS, "/index.html", "text/html", false);
  });

  _server->on("/bundle.js", HTTP_GET, [&](AsyncWebServerRequest *request){
    ESPNEWTUNE_SERIAL("bundle.js inside espnew requested");
    //request->send(LittleFS, "/bundle.js", "text/js", false);
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/js", ESPNEWTUNE_JS, ESPNEWTUNE_JS_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  _server->on("/bundle.css", HTTP_GET, [&](AsyncWebServerRequest *request){
    ESPNEWTUNE_SERIAL("bundle.css inside espnew requested");
    //request->send(LittleFS, "/bundle.css", "text/css", false);
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", ESPNEWTUNE_CSS, ESPNEWTUNE_CSS_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
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

  auto portalRewrite = _server->rewrite("/", "/espnewtune").setFilter(ON_AP_FILTER);
  ESPNEWTUNE_SERIAL("Portal rewrite done\n");
  // Begin Webserver
  _server->begin();

  bool tryconfcon = true;
  unsigned long lastMillis = millis();
  unsigned long lastMillis1 = millis();

  while((WiFi.status() != WL_CONNECTED && (unsigned long)(millis() - lastMillis) < _auto_connect_timeout) || _conn_user_count > 0){
    _dns->processNextRequest();
    yield();
    if(configured) {
        ESPNEWTUNE_SERIAL("INSIDE while conf\n");
      if(WiFi.status() == WL_CONNECTED){
          ESPNEWTUNE_SERIAL("INSIDE while conf if\n");
        _conn_user_count = 0;  // Make forceful disconnect as configuration is done
      } else if((WiFi.status() != WL_CONNECTED && (unsigned long)(millis() - lastMillis1) > DEFAULT_CONNECTION_TIMEOUT)){
          ESPNEWTUNE_SERIAL("INSIDE while conf else\n");
        tryconfcon = false;
        lastMillis = millis();
      }
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
  if(configured){

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

ESPNewTuneClass ESPNewTune;
