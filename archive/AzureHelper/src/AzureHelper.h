#ifndef __AZUREHELPER_H__
#define __AZUREHELPER_H__

#include <Arduino.h>
#include <AzureIoTHub.h>
#include "Esp32MQTTClient.h"
#include <Logger.h>
#include <ArduinoJson.h>
#include <CameraHelper.h>
#include "map"
#include <cstdio>

#define MESSAGE_MAX_LEN 256
#define DEVICE_NAME "ESP32CAM"

struct SpiRamAllocator {
    static void* allocate(size_t size) {
        return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    }

    static void deallocate(void* pointer) {
        heap_caps_free(pointer);
    }

    static void* reallocate(void* ptr, size_t new_size) {
        return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
    }
};
using SpiRamJsonDocument = ArduinoJson6172_91::BasicJsonDocument<SpiRamAllocator>;

class AzureHelper {
    private:
        unsigned long INTERVAL; 
        const char *messageData;
        unsigned long messageCount;
        const char *connectString;

    public:
        static bool isBusy;
        unsigned long lastSendMicros;

        AzureHelper();
        bool init(const char *connectionString);
        void setInterval(unsigned long newInterval);

        bool isAfterInterval();
        bool sendMessagePhoto(CameraHelper *camera);
        bool sendMessagePhoto(CameraHelper *camera, const std::map<String, String>& jsonData);
        void check();

        static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result);
        static void MessageCallback(const char *payLoad, int size);
        static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size);
        static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size);

    void sendTest();
};


#endif // __AZUREHELPER_H__