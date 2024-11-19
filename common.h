#pragma once

#include <Arduino.h>
#include <WString.h>

using namespace std;

#define DEBUG

#ifdef DEBUG
#define dprint(...)     Serial.print(__VA_ARGS__)
#define dprintln(...)   Serial.println(__VA_ARGS__)
#else
#define dprint(...)
#define dprintln(...)
#endif
