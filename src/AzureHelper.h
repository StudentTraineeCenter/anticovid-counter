#ifndef __AZUREHELPER_H__
#define __AZUREHELPER_H__

#include <Arduino.h>
#include <AzureIoTHub.h>
#include "Esp32MQTTClient.h"
#include <Logger.h>

#define MESSAGE_MAX_LEN 256
#define DEVICE_NAME "ESP32CAM"

class AzureHelper {
    private:
        unsigned long INTERVAL; 
        const char *messageData;
        unsigned long messageCount;

    public:
        static bool isBusy;
        int lastSendMicros;

        AzureHelper();
        bool init(const char *connectionString);
        void setInterval(unsigned long newInterval);

        bool isAfterInterval();
        bool sendTestMsg();
        void check();

        static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result);
        static void MessageCallback(const char *payLoad, int size);
        static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size);
        static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size);
};


#endif // __AZUREHELPER_H__