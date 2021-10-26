/*
 * Custom logging utility
 */

#include <Arduino.h>
#include "Logger.h"
/*
 * Log error
 */
void loge(String message)
{
    Serial.println("ERROR: " + message);
}

/*
 * Log info
 */
void logi(String message)
{
    Serial.println("INFO: " + message);
}

/*
 * Log warning
 */
void logw(String message)
{
    Serial.println("WARNING: " + message);
}

/*
 * Log debug
 */
void logd(String message)
{
    Serial.println("DEBUG: " + message);
}

/*
 * Log 'what a terrible failure' (or 'what the f***')
 */
void logwtf(String message)
{
    Serial.println("WHAT A TERRIBLE FAUILURE: " + message);
}

/*
 * Log error with level
 */
void loge(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("ERROR: " + levelS + " " + message);
}

/*
 * Log info with level
 */
void logi(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("INFO: " + levelS + " " + message);
}

/*
 * Log warning with level
 */
void logw(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("WARNING: " + levelS + " " + message);
}

/*
 * Log debug with level
 */
void logd(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("DEBUG: " + levelS + " " + message);
}

/*
 * Log 'what a terrible failure' (or 'what the f***') with level
 */
void logwtf(String message, int level)
{
    String levelS = "";
    for (int i = 0; i < level; i++)
    {
        levelS += "--";
    }

    Serial.println("WHAT A TERRIBLE FAUILURE: " + levelS + " " + message);
}