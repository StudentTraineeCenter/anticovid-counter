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
        if(photoData == nullptr) {
            loge("Base64 encoding not completed!");
        }
        int numOfSplits = 4;
        int splitSize = camera->pixelsCount/numOfSplits;

        int jsonDocumentSize = 252000*sizeof(char); // 252 kB
        SpiRamJsonDocument doc(jsonDocumentSize);
        doc["deviceId"] = DEVICE_NAME;
        doc["messageId"] = messageCount++;
        doc["hasPhoto"] = "true";
        doc["photo"]["resolution"] = camera->resolution_y;
        doc["photo"]["data"] = photoData;

//        for(int n = 0; n < numOfSplits; n++) {
//            String propertyName = "data_" + String(n);
//            doc["photo"][propertyName] = String(photoData).substring(0, splitSize).c_str();
//        }
        camera->clean();
        logd(String(ESP.getFreePsram()));
        logd(String(static_cast<long>(doc.size())));
        logd(String(static_cast<long>(sizeof(doc))));
        auto buffer = (char*) ps_malloc (252500 * sizeof (char));
        char *x;
        serializeJson(doc, x);
        logd(buffer);

        EVENT_INSTANCE *message = Esp32MQTTClient_Event_Generate(x, MESSAGE);

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