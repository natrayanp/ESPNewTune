#ifndef ESPNewTune_h
#define ESPNewTune_h


#include <functional>
#include <Arduino.h>

#if defined(ESP8266)
  #include "ESP8266WiFi.h"
  #include "WiFiClient.h"
  #include "ESPAsyncTCP.h"
#elif defined(ESP32)
  #include "WiFi.h"
  #include "WiFiMulti.h"
  #include "WiFiClient.h"
  #include "AsyncTCP.h"
  #include "Preferences.h"
#endif

#include "ESPAsyncWebServer.h"
#include "DNSServer.h"
#include "espnewtune_webpage.h"
 #include "FS.h"
 #include <LittleFS.h>

/* Library Default Settings */
#define ESPNEWTUNE_DEBUG 1

#define DEFAULT_CONNECTION_TIMEOUT 30000
#define DEFAULT_PORTAL_TIMEOUT 180000

#define ALLOWED_AP_COUNT 5
#define DEFAUL_START_MODE "AUTO"

#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

#include <SPIFFSEditor.h>



#if ESPNEWTUNE_DEBUG == 1
  #define ESPNEWTUNE_SERIAL(x) Serial.print("[ESPConnect]["+String(millis())+"] "+x)
#else
  #define ESPNEWTUNE_SERIAL(x)
#endif

#define FLIP_LED_VAL(x) (_status_led_inverse ^= (x))



class ESPNewTuneClass {

  private:
    DNSServer* _dns = nullptr;
    AsyncWebServer* _server = nullptr;

    int _conn_user_count = 0;  //Connected user count to AP.

    String chipID = String(ESP_getChipId(), HEX);
    //chipID.toUpper();

    String _auto_connect_ssid = "ESP_" + chipID + "_AutoConnectAP";
    String _auto_connect_password = "MyESP_" + chipID;
    unsigned long _auto_connect_timeout = DEFAULT_PORTAL_TIMEOUT;


    WiFiMulti wifiMulti;

    struct APval
    {
        String _sta_ssid;
        String _sta_password;
    };

    struct APval APvals[ALLOWED_AP_COUNT];

    String _con_mode = DEFAUL_START_MODE;  //ONDEMAND

  private:
    void load_sta_credentials();

    // Start Captive portal
    bool start_portal();


    void WiFiStationConnected();

  public:
    // Set Custom AP
    void autoConnect(const char* ssid, const char* password, unsigned long timeout);

    // Connect to Saved WiFi Credentials
    bool begin(AsyncWebServer* server,  unsigned long timeout = DEFAULT_CONNECTION_TIMEOUT);

    // Start Captive Portal on Demand
    bool beginOnDemand(unsigned long timeout = DEFAULT_CONNECTION_TIMEOUT);

    // Erase Saved WiFi Credentials
    bool erase();

    // CODE
    //void MYEditor(const fs::FS& fs);

    /*
      Data Getters
    */

    // Return true / false depending of connection status
    bool isConnected();

    // Gets SSID of connected endpoint
    String getSSID();
    String getPassword();
};

extern ESPNewTuneClass ESPNewTune;

#endif
