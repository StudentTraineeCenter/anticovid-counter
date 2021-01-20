/**
 * A simple Azure IoT example for sending telemetry.
 */

#include <Arduino.h>
#include <WiFi.h>
#include "Esp32MQTTClient.h"
#include "esp_camera.h"

// #include "camera.cpp"

// Please input the SSID and password of WiFi
const char *ssid = "Don Pedros phone";
const char *password = "Spojilaci123";

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char *connectionString = "HostName=CovidCounter.azure-devices.net;DeviceId=ESP32-CAM;SharedAccessKey=QxtYp8VVHWJBPiDk1DMvu3JXDqleZhgCl/GF40NmZaQ=";

static bool hasIoTHub = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting connecting WiFi.");
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!Esp32MQTTClient_Init((const uint8_t *)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  // camera_setup();
}

void loop()
{
  Serial.println("start sending events.");

  if (hasIoTHub)
  {
    char buff[128];

    // replace the following line with your data sent to Azure IoTHub
    snprintf(buff, 128, "{\"topic\":\"iot\"}");

    if (Esp32MQTTClient_SendEvent(buff))
    {
      Serial.println("Sending data succeed");
    }
    else
    {
      Serial.println("Failure...");
    }
    // default config is 5000
    delay(500000);
  }
}
