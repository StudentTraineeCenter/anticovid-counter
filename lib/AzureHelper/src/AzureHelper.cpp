#include <time.h>
#include "AzureHelper.h"

bool AzureHelper::isBusy = false;

AzureHelper::AzureHelper() {
    INTERVAL = 10e6; // 10 sec
    messageData = R"({"deviceId":"%s", "messageId":%d, "Temperature":%f, "Humidity":%f})";
    messageCount = 0;
    lastSendMicros = 0;
}

bool AzureHelper::init(const char *connectionString) {
    logi("Connecting to Azure...");
    AzureHelper::connectString = connectionString;
    if (!Esp32MQTTClient_Init((const uint8_t *) connectionString, true)) {
        loge("Failed init!", 1);
        return false;
    }
    logi("Connected successfully!", 1);

    Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
    Esp32MQTTClient_SetMessageCallback(MessageCallback);
    Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
    Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
    return true;
}

void AzureHelper::setInterval(unsigned long newInterval) {
    INTERVAL = newInterval;
}

bool AzureHelper::isAfterInterval() {
    return micros() - lastSendMicros >= INTERVAL;
}

bool AzureHelper::sendMessagePhoto(CameraHelper *camera) {
    return AzureHelper::sendMessagePhoto(camera, std::map<String, String>());
}

bool AzureHelper::sendMessagePhoto(CameraHelper *camera, const std::map<String, String> &jsonData) {
    if (!isBusy && isAfterInterval()) {
        logi("Sending message to Azure...");

        if (!camera->isPhotoCaptured()) {
            loge("Photo not captured, yet", 1);
            return false;
        }

        int jsonDocumentSize = 252000*sizeof(char); // 252 kB
        SpiRamJsonDocument doc(jsonDocumentSize);
        doc["deviceId"] = DEVICE_NAME;
        doc["messageId"] = messageCount++;
        doc["hasPhoto"] = "true";
        doc["photo"]["data"] = camera->getPhotoGrayscaleString();
        doc["photo"]["format"] = String(camera->getPhoto().format).c_str();

        for (const auto &it : jsonData)
            doc[it.first.c_str()] = it.second.c_str();

        logd(String(ESP.getFreePsram()));
        logd(String(static_cast<long>(doc.size())));
        logd(String(static_cast<long>(sizeof(doc))));
        char *buffer = (char *) ps_malloc (254000 * sizeof (char));

        serializeJson(doc, buffer, jsonDocumentSize);

        logd(buffer);

        EVENT_INSTANCE *message = Esp32MQTTClient_Event_Generate(buffer, MESSAGE);

        if (Esp32MQTTClient_SendEventInstance(message)) {
            lastSendMicros = micros();
            logi("Message sent!", 1);
            return true;
        } else {
            loge("Error while sending message!", 1);
            return false;
        }
    } else {
        return false;
    }
}

void AzureHelper::check() {
    Esp32MQTTClient_Check();
}

void AzureHelper::SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result) {
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
        logi("Send Confirmation Callback finished.");
    }
}

void AzureHelper::MessageCallback(const char *payLoad, int size) {
    logi("Message callback:");
    logi(payLoad, 1);
}

void AzureHelper::DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size) {
    char *temp = (char *) malloc(size + 1);
    if (temp == nullptr) {
        return;
    }
    memcpy(temp, payLoad, size);
    temp[size] = '\0';
    // Display Twin message.Serial.println(temp);
    free(temp);
}

int AzureHelper::DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size,
                                      unsigned char **response, int *response_size) {

    logi("Try to invoke method " + String(methodName));
    const char *responseMessage = "\"Successfully invoke device method\"";
    int result = 200;

    if (strcmp(methodName, "start") == 0) {
        logi("Start sending temperature and humidity data", 1);
        isBusy = true;
    } else if (strcmp(methodName, "stop") == 0) {
        logi("Stop sending temperature and humidity data", 1);
        isBusy = false;
    } else {
        logi("No method " + String(methodName) + " found", 1);
        responseMessage = "\"No method found\"";
        result = 404;
    }

    *response_size = strlen(responseMessage) + 1;
    *response = (unsigned char *) strdup(responseMessage);

    return result;
}