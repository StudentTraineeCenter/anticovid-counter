#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Arduino.h>

void loge(String message);
void logi(String message);
void logw(String message);
void logd(String message);
void logwtf(String message);

void loge(String message, int level);
void logi(String message, int level);
void logw(String message, int level);
void logd(String message, int level);
void logwtf(String message, int level);

#endif // __LOGGER_H__