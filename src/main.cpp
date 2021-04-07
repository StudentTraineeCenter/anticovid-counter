// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. 

#include <Arduino.h>
#include <Secrets.h>
#include <WiFiClientSecure.h>
#include <AzureIoTSocket_WiFi.h>
#include <AzureIotHub.h>
#include <Logger.h>
#include "AzureHelper.h"
#include "CameraHelper.h"

#define BAUD_RATE 115200

AzureHelper *azure;
CameraHelper *camera;

bool initWifi() {
    logi("Wifi", 1);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    logi("Connecting...", 2);
    while (WiFiClass::status() != WL_CONNECTED) {} // Wait until connection successful
    logi("Connected successfully (IP: " + WiFi.localIP().toString() + ", MAC: " + WiFi.macAddress() + ")", 2);
    return true;
}

void setup() {
    logi("Setting baud rate to " + String(BAUD_RATE));
    Serial.begin(BAUD_RATE);

    logi("Initializing device...");
    if (!initWifi())
        return;
    azure = new AzureHelper();
    if (!azure->init(CONNECTION_STRING))
        return;
    camera = new CameraHelper();
    if (!camera->init())
        return;

}

void loop() {
    if (WiFiClass::status() != WL_CONNECTED)
        initWifi();

    if(camera->capture()){
        if (azure->sendMessagePhoto(camera)) {
            // Yay!
        } else {
            azure->check();
        }
        logd("Captured");
    }
    camera->clean();

    delay(5000);
}