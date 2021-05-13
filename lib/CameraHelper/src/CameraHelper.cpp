#include "CameraHelper.h"

/**
 * General constructor
 * */
CameraHelper::CameraHelper() {
    s = nullptr;
    lastCapture = nullptr;
    logi("CameraHelper initialization...");
    setPins();
}

/**
 * Initialization function of the CameraHelper
 * */
bool CameraHelper::init() {
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        loge(&"Camera init failed with error 0x"[err], 1);
        return false;
    }
    logi("Camera initialized", 1);
    return true;
}

/**
 * Tries to capture the photo and save it to the lastCapture
 * */
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

/**
 * Returns whether photo is captured and in memory
 * */
bool CameraHelper::isPhotoCaptured() const {
    return lastCapture != nullptr || !lastCapture;
}

/**
 * Returns raw photo
 * */
camera_fb_t CameraHelper::getPhoto() const {
    return *lastCapture;
}

/**
 * Returns string of grayscale values converted to chars
 * Uses PSRAM instead of ordinary RAM
 * */
const char *CameraHelper::getPhotoGrayscaleString() {
    uint8_t *buf = lastCapture->buf;
    size_t len = lastCapture->len;

    auto *str = (char *) ps_malloc(sizeof(char) * (len - 1));

    for (size_t i = 0; i < len; i++) {
        int cI = static_cast<int>(*(buf + i));
        str[i] = static_cast<char>(cI == 0 ? 1 : cI);
    }
    return str;
}

/**
 * Returns frame to camera, needed after image processing completed
 * */
void CameraHelper::clean() const {
    logi("Resetting camera...");
    esp_camera_fb_return(lastCapture);
    logi("Done", 1);
}

/**
 * Returns string of grayscale values converted to chars and encodes as BASE64
 * Uses PSRAM instead of ordinary RAM
 *
 * base64 lib: https://github.com/Densaugeo/base64_arduino
 * */
const char *CameraHelper::getPhotoGrayscaleAsBase64() {
    uint8_t *buf = lastCapture->buf;
    size_t len = lastCapture->len;

    // Following code comes from base64.cpp in Arduino lib, but edited for PSRAM use...
    size_t size = base64_encode_expected_len(len) + 1;
    char *base64Str = (char *) ps_malloc(size);
    if (base64Str) {
        base64_encodestate _state;
        base64_init_encodestate(&_state);
        int base64Len = base64_encode_block((const char *) &buf[0], len, &base64Str[0], &_state);
        base64Len = base64_encode_blockend((base64Str + base64Len), &_state);
        return base64Str;
    } else return "";
}

/**
 * Initialization of pins
 * */
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

    // Only support 10 MHz current. Camera will output bad image when XCLK is 20 MHz.
    config.xclk_freq_hz = 10000000;

    config.pixel_format = PIXFORMAT_GRAYSCALE;

    config.frame_size = FRAMESIZE_QQVGA; // 320x240, QVGA
    resolution = "240x240";
    // config.jpeg_quality = 40; // used for JPEG only

    config.fb_count = 1;

    logi("Done", 2);
}
