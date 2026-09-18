// --- Storage.h ---
// Capability contract for persistence. SdStorage implements it against an
// SD card; NullStorage is the fallback when no card is present, so the rest
// of the sketch never has to check "do we have an SD card?" itself.
#pragma once

#include <Arduino.h>

class Storage {
  public:
    virtual ~Storage() {}
    virtual bool begin() = 0;
    virtual void log(const String &line) = 0;
    virtual void saveSetting(const String &key, const String &value) = 0;
    virtual String loadSetting(const String &key, const String &fallback) = 0;
};
