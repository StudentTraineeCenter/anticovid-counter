#include "AzureHelperHTTP.h"

bool AzureHelperHTTP::isBusy = false;

AzureHelperHTTP::AzureHelperHTTP() {
    INTERVAL = 10e6; // 10 sec
    messageCount = 0;
    lastSendMicros = 0;
}

void
AzureHelperHTTP::init(const char *connectString, const char *sas_token, const char *root_ca, const char *deviceName,
                      const char *iothub) {
    this->connectString = connectString;
    this->sas_token = sas_token;
    this->root_ca = root_ca;
    this->deviceName = deviceName;
    this->iothub = iothub;
}

void AzureHelperHTTP::setInterval(unsigned long newInterval) {
    INTERVAL = newInterval;
}

bool AzureHelperHTTP::isAfterInterval() {
    return micros() - lastSendMicros >= INTERVAL;
}

bool AzureHelperHTTP::sendMessagePhoto(CameraHelper *camera) {
    if (!isBusy && isAfterInterval()) {
        logi("Sending message to Azure...");

        if (!camera->isPhotoCaptured()) {
            loge("Photo not captured, yet", 1);
            return false;
        }
        String datetime = String(millis());
        DynamicJsonDocument doc(1024);
        doc["blobName"] = iothub;

        String msg = "";
        serializeJson(doc, msg);

        //region HTTP Request

        String urlForFileUploadInit =
                "https://" + iothub + ".azure-devices.net/devices/" + deviceName +
                "/files?api-version=2020-03-13";

        http.begin(urlForFileUploadInit.c_str());
        http.addHeader("Authorization", sas_token);
        http.addHeader("Content-Type", "application/json");

        int httpCode = http.POST(msg);

        if (httpCode > 0) { //Check for the returning code
            String payload = http.getString();
            Serial.println(httpCode);
            Serial.println(payload);
            http.end();
            return true;
        } else {
            Serial.println("Error on HTTP request");
            http.end();
            return false;
        }
        //endregion
    } else {
        return false;
    }
}

bool AzureHelperHTTP::sendPhotoAsBase64(CameraHelper *camera) {
    if (!isBusy && isAfterInterval()) {
        logi("Sending message to Azure...");

        // Cannot send photo without actual photo
        if (!camera->isPhotoCaptured()) {
            loge("Photo not captured, yet", 1);
            return false;
        }

        // Populate JSON doc
        String datetime = String(millis());
        SpiRamJsonDocument doc(255100);
        doc["messageId"] = messageCount++;
        doc["hasPhoto"] = true;
        doc["photo"] = camera->getPhotoGrayscaleAsBase64();
        doc["res"] = camera->resolution;
        doc["debug"] = true;

        // Serialize
        String msg;
        serializeJson(doc, msg);

        // Reset camera buffer
        camera->clean();

        //region HTTP Request
        String urlForMessage = "https://" + iothub + ".azure-devices.net/devices/" + deviceName +
                               "/messages/events?api-version=2020-03-13";

        http.begin(urlForMessage.c_str());
        http.addHeader("Authorization", sas_token);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Content-Length", String(msg.length()));

        int responseCode = http.POST(msg);

        // Check response core
        if (responseCode > 0) {
            String payload = http.getString();
            Serial.println(responseCode);
            Serial.println(payload);
            http.end();
            return true;
        } else {
            Serial.println("Error on HTTP request");
            http.end();
            return false;
        }
        //endregion
    } else {
        return false;
    }
}

bool AzureHelperHTTP::sendMessage(const char *msg) {
    //region HTTP Request
    String urlForMessage = "https://" + iothub + ".azure-devices.net/devices/" + deviceName +
                           "/messages/events?api-version=2020-03-13";
    http.begin(urlForMessage.c_str());
    http.addHeader("Authorization", sas_token);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(msg);


    if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
        http.end();
        return true;
    } else {
        Serial.println("Error on HTTP request");
        http.end();
        return false;
    }
    //endregion
}