#include <Arduino.h>
void loge(String message)
{
    Serial.println("ERROR: " + message);
}

void logi(String message)
{
    Serial.println("INFO: " + message);
}

void logw(String message)
{
    Serial.println("WARNING: " + message);
}

void logd(String message)
{
    Serial.println("DEBUG: " + message);
}

void logwtf(String message)
{
    Serial.println("WHAT A TERRIBLE FAUILURE: " + message);
}

void loge(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("ERROR: " + levelS + " " + message);
}

void logi(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("INFO: " + levelS + " " + message);
}

void logw(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("WARNING: " + levelS + " " + message);
}

void logd(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("DEBUG: " + levelS + " " + message);
}

void logwtf(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("WHAT A TERRIBLE FAUILURE: " + levelS + " " + message);
}