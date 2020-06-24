#ifndef _DEBUG_LOGGING_HPP_
#define _DEBUG_LOGGING_HPP_

#include <Arduino.h>

#ifdef DEBUG_LOGGING
#define initLogging()         \
    Serial.begin(115200);     \
    while (!Serial)           \
    {                         \
        if (millis() > 10000) \
            break;            \
        delay(1);             \
    }
#define log Serial.print
#define logln Serial.println
// #define logf Serial.printf
#endif

#ifndef DEBUG_LOGGING
#define initLogging()
#define log(...)
#define logln(...)
// #define logf(...)
#endif

#endif