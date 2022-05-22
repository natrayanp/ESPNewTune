<p align="center">
  <b>ESPNewTune</b> is a simplistic and easy approch to having a WiFi manager on your ESP MCUs. It is a lightweight library which let's you configure WiFi networks without any fuss. ESPNewTune works with <b>ESP32</b> for now.
</p>
 
<br>

<h2>How to Install</h2>

###### Manual Install

For Windows: Download the [Repository](https://github.com/natrayanp/ESPNewTune/archive/main.zip) and extract the .zip in Documents>Arduino>Libraries>{Place "ESPNewTune" folder Here}

For Linux: Download the [Repository](https://github.com/natrayanp/ESPNewTune/archive/main.zip) and extract the .zip in Sketchbook>Libraries>{Place "ESPNewTune" folder Here}

###### Manually through IDE

Download the [Repository](https://github.com/natrayanp/ESPNewTune/archive/main.zip), Go to Sketch>Include Library>Add .zip Library> Select the Downloaded .zip File.

<br>

<h2>Dependencies</h2>
<p>
  
  <h5>For ESP32:</h5>
  <ul>
      <li>ESP32 Arduino Core - <b>latest</b></li>
      <li>AsyncTCP - <b>v1.1.1</b></li>
  	  <li>ESPAsyncWebServer - <b>v1.2.3</b></li>
  </ul>
</p>

<br>

<h2>Documentation</h2>
<p>ESPNewTune is a dead simple library which does your work in just 2 lines:</p>

<br>

#### `ESPNewTune.autoConnect(const char* ssid, const char* password, unsigned long timeout)`
This function sets the SSID and password for your captive portal. The third parameter - `timeout` is optional which tells the library to keep the captive portal running for `x` - milliseconds. (Default - `3 minutes`)

<br>

#### `ESPNewTune.begin(AsyncWebServer *server)`
Call this function after `autoConnect` to start the WiFi manager. This function will first try to connect to the previous configured WiFi credentials and if those are not found, it will spin up a captive portal. If the user doesn't sets the wifi credentials, the library will return to execute your rest of the code in the setup block.

**Please note:** WiFi inactivity is managed by your code. If you think you need to reboot the MCU if there is no WiFi then please call the `ESP.restart()` function after checking `ESPNewTune.begin()`'s response.

<br>
<br>

<h2>Examples</h2>
 
 Checkout example for ESP32 in `examples` directory. [Click Here](https://github.com/natrayanp/ESPNewTune/tree/master/examples)
 
<br>

<h2>Contributions</h2>
<p>Every contribution to this repository is highly appreciated!</p>

ESPNewTune consists of 2 parts:
- C++ library ( in `src` folder )
- Svelte webpage ( in `portal` folder ) ( provided source code for transparency )</li>

You can contribute to the part you have most skill in.


<br>


<h2>License</h2>
ESPConnect is licensed under MIT.
<br>
<br>

# ESPNewTune
