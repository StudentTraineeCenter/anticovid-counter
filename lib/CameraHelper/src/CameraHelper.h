//
// Created by Jakub on 20-Jan-21.
//

#ifndef ANTICOVIDPEOPLECOUNTER_CAMERAHELPER_H
#define ANTICOVIDPEOPLECOUNTER_CAMERAHELPER_H

#include <Arduino.h>
#include <Logger.h>
#include "CameraHelper.h"
#include "esp_camera.h"
#include <sstream>
//#include "FS.h"                // SD Card ESP32
//#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

class CameraHelper {
private:
    void setPins();

    sensor_t *s;
    camera_config_t config{};

public:
    camera_fb_t *lastCapture;
    CameraHelper();

    bool init();

    void begin();

    bool capture();

    bool isPhotoCaptured();

    camera_fb_t getPhoto();

    uint8_t getPhotoRaw();

    String getPhotoAsString();

    String getPhotoFormat();

    void clean();
};

#endif //ANTICOVIDPEOPLECOUNTER_CAMERAHELPER_H
