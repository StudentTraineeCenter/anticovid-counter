# Anticovid Counter
Petrova sekce
## obsah
<ul>
    <li>assets   
        <ul>
            <li>schema_cloud_connection.pdf - schema navrhu zapojeni v cloudu
            <li>seminarka.pdf
        </ul>
    <li>src - hlavni kod
        <ul>
            <li>iot_hub.cpp - template for connect
            <li>main.cpp - ...
        </ul>
</ul>

## platformio.ini info
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
lib_deps = 
	ottowinter/ESPAsyncWebServer-esphome@^1.2.7
	ottowinter/AsyncTCP-esphome@^1.1.1
	ottowinter/ESPAsyncTCP-esphome@^1.2.3
	vschina/ESP32 Azure IoT Arduino@^0.1.0
