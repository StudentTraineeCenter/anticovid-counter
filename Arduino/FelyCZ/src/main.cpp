#include <Arduino.h>
#include <Secrets.h>
#include <WiFiClientSecure.h>
#include <Logger.h>
#include "AzureHelper.h"
#include <AzureIoTSocket_WiFi.h>
#include <AzureIotHub.h>
#include "CameraHelper.h"
#include <cstdio>

#define BAUD_RATE 115200

AzureHelper *azure;
CameraHelper *camera;

/*
 * Check for Wi-Fi network and join it.
 * Blocks the main thread until successfully connected.
 */
bool initWifi() {
    logi("Wifi", 1);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    logi("Connecting...", 2);
    while (WiFiClass::status() != WL_CONNECTED) {} // Wait until connection successful
    logi("Connected successfully (IP: " + WiFi.localIP().toString() + ", MAC: " + WiFi.macAddress() + ")", 2);
    return true;
}

/*
 * Arduino method
 */
void setup() {
    logi("Setting baud rate to " + String(BAUD_RATE));
    Serial.begin(BAUD_RATE);

    logi("Initializing device...");
    if (!initWifi())
        return;
    azure = new AzureHelper();
    azure->init(CONNECTION_STRING);
    camera = new CameraHelper();
    if (!camera->init())
        return;
}

/*
 * Arduino method
 */
void loop() {
    if (WiFiClass::status() != WL_CONNECTED) // If Wi-Fi connection lost, re-init
        initWifi();

    if (camera->capture() && WiFi.isConnected()) {
        if (azure->sendMessagePhoto(camera)) {
            logd("Photo sent!");
        }
        logd("Captured");
    }
    delay(5000);
}