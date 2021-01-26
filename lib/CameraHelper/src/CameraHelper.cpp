//
// Created by Jakub on 20-Jan-21.
//

#include "CameraHelper.h"

CameraHelper::CameraHelper() {
    s = nullptr;
    lastCapture = nullptr;
    logi("CameraHelper initialization...");
    setPins();
    if (psramFound()) {
        logd("PSRAM found", 1);
        config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
        config.jpeg_quality = 10; //10-63 lower number means higher quality
        config.fb_count = 2;
    } else {
        logd("PSRAM NOT found", 1);
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
}

bool CameraHelper::init() {
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        loge(&"Camera init failed with error 0x"[err], 1);
        return false;
    }
    logi("Camera initialized", 1);
    return true;
}

void CameraHelper::begin() {
    logi("Setting camera parameters...");
    s = esp_camera_sensor_get();
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s,
                          0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
    s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
    s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
    s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
    s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
    s->set_aec2(s, 0);           // 0 = disable , 1 = enable
    s->set_ae_level(s, 0);       // -2 to 2
    s->set_aec_value(s, 300);    // 0 to 1200
    s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t) 0);  // 0 to 6
    s->set_bpc(s, 0);            // 0 = disable , 1 = enable
    s->set_wpc(s, 1);            // 0 = disable , 1 = enable
    s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
    s->set_lenc(s, 1);           // 0 = disable , 1 = enable
    s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
    s->set_vflip(s, 0);          // 0 = disable , 1 = enable
    s->set_dcw(s, 1);            // 0 = disable , 1 = enable
    s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
    logi("Done", 1);
}

bool CameraHelper::capture() {
    logi("Capturing photo...");
    lastCapture = nullptr;
    lastCapture = esp_camera_fb_get();
    if (!lastCapture) {
        loge("Error while taking photo", 1);
        return false;
    }
    logi("Done", 1);
    return true;
}

bool CameraHelper::isPhotoCaptured() {
    return lastCapture != nullptr || !lastCapture;
}

camera_fb_t CameraHelper::getPhoto() {
    return *lastCapture;
}

uint8_t CameraHelper::getPhotoRaw() {
    return *lastCapture->buf;
}

String CameraHelper::getPhotoAsString() {
//    uint8_t *fbBuf = lastCapture->buf;
//    size_t fbLen = lastCapture->len;
//    std::stringstream ss;
//    for (size_t n = 0; n < fbLen; n = n + 1024) {
//        if (n + 1024 < fbLen) {
//            ss << fbBuf[n];
//            Serial.write(fbBuf, 1024);
//            fbBuf += 1024;
//        } else if (fbLen % 1024 > 0) {
//            size_t remainder = fbLen % 1024;
//            client.write(fbBuf, remainder);
//        }
//        Serial.write(fbBuf,fbLen);
//    }
    return String(*((char *) lastCapture->buf));
}

String CameraHelper::getPhotoFormat() {
    return String(lastCapture->format);
}

void CameraHelper::clean() {
    logi("Resetting camera...");
    esp_camera_fb_return(lastCapture);
    logi("Done", 1);
}

void CameraHelper::setPins() {
    logi("Setting PINs", 1);
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG
    logi("Done", 2);
}
