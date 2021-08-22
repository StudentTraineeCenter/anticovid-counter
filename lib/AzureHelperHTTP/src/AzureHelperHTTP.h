#ifndef __AZUREHELPERHTTP_H__
#define __AZUREHELPERHTTP_H__

#define HELLO_WORLD "Hello World from IoTHubDeviceClient_LL_UploadToBlob"

#include <Arduino.h>
#include <Logger.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <CameraHelper.h>
#include <cstdio>

struct SpiRamAllocator {
    void* allocate(size_t size) {
        return ps_malloc(size);

    }
    void deallocate(void* pointer) {
        free(pointer);
    }
};

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;

class AzureHelperHTTP {
private:
    unsigned long INTERVAL;
    unsigned long messageCount;
    const char *connectString;
    String sas_token;
    const char *root_ca;
    String deviceName;
    String iothub;
    WiFiClient wifi;
    HTTPClient http;


public:
    static bool isBusy;
    unsigned long lastSendMicros;

    AzureHelperHTTP();
    void init(const char *connectString, const char *sas_token, const char *root_ca, const char *deviceName, const char *iothub);
    void setInterval(unsigned long newInterval);

    bool isAfterInterval();
    bool sendMessagePhoto(CameraHelper *camera);
    bool sendPhotoAsBase64(CameraHelper *camera);
    bool sendMessage(const char *msg);
    void sendFile();
};

#endif //ANTICOVIDPEOPLECOUNTER_AZUREHELPERHTTP_H
