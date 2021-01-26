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
        auto photo = camera->lastCapture;
        uint8_t *fbBuf = photo->buf;
        for (size_t n = 0; n < photo->len; n = n + 1024) {
        if (n + 1024 < photo->len) {
            Serial.write(fbBuf, 1024);
            fbBuf += 1024;
        } else if (photo->len % 1024 > 0) {
            size_t remainder = photo->len % 1024;
            Serial.write(fbBuf, remainder);
        }
    }
        logd(String(*photo->buf));
        if (azure->sendMessagePhoto(camera)) {

        } else {
            azure->check();
        }

        camera->clean();

    }

    delay(5000);
}