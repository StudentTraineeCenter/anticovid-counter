// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. 

#include <Arduino.h>
#include <Secrets.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <AzureIoTSocket_WiFi.h>
#include <AzureIotHub.h>
#include "Esp32MQTTClient.h"
#include <Logger.h>
#include "AzureHelper.h"

#define BAUD_RATE 115200

AzureHelper azure;

bool initWifi() {
  logi("Wifi", 1);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  logi("Connecting...", 2);
  while (WiFi.status() != WL_CONNECTED) {} // Wait until connection successful
  logi("Connected successfully (IP: "+ WiFi.localIP().toString() + ", MAC: " + WiFi.macAddress() + ")", 2);
  return true;  
}

void setup()
{
  logi("Setting baud rate to " + String(BAUD_RATE));
  Serial.begin(BAUD_RATE);

  logi("Initializing device...");
  if(!initWifi())
    return;

  azure = AzureHelper();
  if(!azure.init(CONNECTION_STRING)) {
    return;
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    initWifi();
  }

  if (azure.sendTestMsg()){}
  else
  {
    azure.check();
  }
  delay(60);
}