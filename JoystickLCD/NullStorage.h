// --- NullStorage.h ---
// No-op Storage used when no SD card is present. Serial logging still
// happens independently, so nothing is silently lost - it's just not
// persisted across power cycles.
#pragma once

#include "Storage.h"

class NullStorage : public Storage {
  public:
    bool begin() override {
      return false;
    }
    void log(const String &line) override {}
    void saveSetting(const String &key, const String &value) override {}
    String loadSetting(const String &key, const String &fallback) override {
      return fallback;
    }
};
