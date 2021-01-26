#ifndef __AZUREHELPER_H__
#define __AZUREHELPER_H__

#include <Arduino.h>
#include <AzureIoTHub.h>
#include "Esp32MQTTClient.h"
#include <Logger.h>
#include <ArduinoJson.h>
#include <CameraHelper.h>
#include "map"

#define MESSAGE_MAX_LEN 256
#define DEVICE_NAME "ESP32CAM"

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

        void uploadToBlob();

        static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result);
        static void MessageCallback(const char *payLoad, int size);
        static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size);
        static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size);
};


#endif // __AZUREHELPER_H__