// This file is based on Microsoft IoT Hub Demo and [https://github.com/petrkucerak/AzureCommunityDay][this repository].

#include <time.h>
#include "AzureHelper.h"

bool AzureHelper::isBusy = false;

/*
 * Default constructor
 */
AzureHelper::AzureHelper() {
    INTERVAL = 10e6; // 10 sec
    messageCount = 0;
    lastSendMicros = 0;
}

/*
 * Initialization function - creates connection to Azure IoT Hub using Connection String
 */
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

/*
 * Sets interval of sending photos
 */
void AzureHelper::setInterval(unsigned long newInterval) {
    INTERVAL = newInterval;
}

/*
 * Checks if the set interval has elapsed
 */
bool AzureHelper::isAfterInterval() {
    return micros() - lastSendMicros >= INTERVAL;
}

/*
 * Sends photo to Azure IoT Hub using MQTT protocol without optional Key-Value parameters
 * @param camera: instance of CameraHelper using which the photo will be taken
 */
bool AzureHelper::sendMessagePhoto(CameraHelper *camera) {
    return AzureHelper::sendMessagePhoto(camera, std::map<String, String>());
}

/*
 * Sends photo to Azure IoT Hub using MQTT protocol with optional Key-Value parameters
 * @param camera: instance of CameraHelper using which the photo will be taken
 * @param jsonData: map of Key-Value parameters which will be added to MQTT message
 * @returns 'true' if the photo was successfully sent or 'false' if it wasn't
 */
bool AzureHelper::sendMessagePhoto(CameraHelper *camera, const std::map<String, String> &jsonData) {
    if (!isBusy && isAfterInterval()) {
        logi("Sending message to Azure...");

        if (!camera->isPhotoCaptured()) {
            loge("Photo not captured, yet", 1);
            return false;
        }

        auto photoData = camera->getPhotoGrayscaleAsBase64();
        if (photoData == nullptr) {
            loge("Base64 encoding not completed!");
            return false;
        }
//        int numOfSplits = 4;
//        int splitSize = photo.length() / numOfSplits;
//        int jsonDocumentSize = 252000 * sizeof(char); // 252 kB
//        SpiRamJsonDocument doc(jsonDocumentSize);

//        for (int n = 0; n < numOfSplits; n++) {
//            String propertyName = String("photo") + String(n);
//            if (n + 1 < numOfSplits) {
//                doc[propertyName] = photo.substring(0 + (n * splitSize),
//                                                                splitSize + (n * splitSize)).c_str();
//            } else if (n + 1 == numOfSplits) {
//                doc[propertyName] = photo.substring(0 + (n * splitSize),
//                                                                splitSize + (n * splitSize) + 1).c_str();
//            }
//        }

        int msgId = messageCount++;
        DynamicJsonDocument doc1(1024);
        doc1["deviceId"] = DEVICE_NAME;
        doc1["messageId"] = msgId;
        doc1["hasPhoto"] = "true";
        doc1["photoRes"] = "96x96";
        doc1["photo0"] = String(photoData).substring(0, 3100).c_str();

        DynamicJsonDocument doc2(1024);
        doc2["deviceId"] = DEVICE_NAME;
        doc2["messageId"] = msgId;
        doc2["hasPhoto"] = "true";
        doc2["photoRes"] = "96x96";
        doc2["photo1"] = String(photoData).substring(3100, 6200).c_str();

        DynamicJsonDocument doc3(1024);
        doc3["deviceId"] = DEVICE_NAME;
        doc3["messageId"] = msgId;
        doc3["hasPhoto"] = "true";
        doc3["photoRes"] = "96x96";
        doc3["photo2"] = String(photoData).substring(6200, 9300).c_str();

        DynamicJsonDocument doc4(1024);
        doc4["deviceId"] = DEVICE_NAME;
        doc4["messageId"] = msgId;
        doc4["hasPhoto"] = "true";
        doc4["photoRes"] = "96x96";
        doc4["photo3"] = String(photoData).substring(9300).c_str();

        camera->clean();
        logd("Free RAM: " + String(ESP.getFreeHeap()));

        String msg1, msg2, msg3, msg4;
        serializeJson(doc1, msg1);
        serializeJson(doc2, msg2);
        serializeJson(doc3, msg3);
        serializeJson(doc4, msg4);
        Serial.println(msg1.c_str());
        Serial.println(msg2.c_str());
        Serial.println(msg3.c_str());
        Serial.println(msg4.c_str());
        EVENT_INSTANCE *message1 = Esp32MQTTClient_Event_Generate(msg1.c_str(), MESSAGE);
        EVENT_INSTANCE *message2 = Esp32MQTTClient_Event_Generate(msg2.c_str(), MESSAGE);
        EVENT_INSTANCE *message3 = Esp32MQTTClient_Event_Generate(msg3.c_str(), MESSAGE);
        EVENT_INSTANCE *message4 = Esp32MQTTClient_Event_Generate(msg4.c_str(), MESSAGE);

        bool result = false;
        if (Esp32MQTTClient_SendEventInstance(message1) && Esp32MQTTClient_SendEventInstance(message2) &&
            Esp32MQTTClient_SendEventInstance(message3) && Esp32MQTTClient_SendEventInstance(message4)) {
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

/*
 * Checks for any callbacks (confirmation, cloud-to-device, message, device twin or device method)
 */
void AzureHelper::check() {
    Esp32MQTTClient_Check();
}

/*
 * Callback method for confirmation of send
 */
void AzureHelper::SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result) {
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
        logi("Send Confirmation Callback finished.");
    }
}

/*
 * Callback method for messages
 */
void AzureHelper::MessageCallback(const char *payLoad, int size) {
    logi("Message callback:");
    logi(payLoad, 1);
}

/*
 * Callback method for device twin messages
 */
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

/*
 * Callback method for device method
 */
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