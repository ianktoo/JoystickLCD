// --- SdStorage.h ---
// Plain-text SD card storage: settings.txt holds "key=value" lines, log.txt
// is an append-only "millis,message" log. Requires an SD card module wired
// over SPI with its chip-select pin passed to the constructor.
#pragma once

#include <SD.h>
#include "Storage.h"

class SdStorage : public Storage {
  private:
    static const char* LOG_FILE;
    static const char* SETTINGS_FILE;
    static const int MAX_SETTINGS = 16;

    int csPin;
    bool ready = false;

  public:
    explicit SdStorage(int chipSelectPin) : csPin(chipSelectPin) {}

    bool begin() override {
      ready = SD.begin(csPin);
      if (ready) {
        Serial.println("[Storage] SD card ready");
      } else {
        Serial.println("[Storage] SD card not found, falling back to Serial-only logging");
      }
      return ready;
    }

    void log(const String &line) override {
      if (!ready) {
        return;
      }
      File f = SD.open(LOG_FILE, FILE_WRITE);
      if (f) {
        f.print(millis());
        f.print(",");
        f.println(line);
        f.close();
      }
    }

    void saveSetting(const String &key, const String &value) override {
      if (!ready) {
        return;
      }

      String otherLines[MAX_SETTINGS];
      int count = 0;

      if (SD.exists(SETTINGS_FILE)) {
        File f = SD.open(SETTINGS_FILE, FILE_READ);
        while (f && f.available() && count < MAX_SETTINGS) {
          String line = f.readStringUntil('\n');
          line.trim();
          if (line.length() == 0) {
            continue;
          }
          int eq = line.indexOf('=');
          if (eq < 0 || line.substring(0, eq) == key) {
            continue; // dropped: malformed, or the key we're about to rewrite
          }
          otherLines[count++] = line;
        }
        if (f) {
          f.close();
        }
      }

      SD.remove(SETTINGS_FILE);
      File out = SD.open(SETTINGS_FILE, FILE_WRITE);
      if (out) {
        for (int i = 0; i < count; i++) {
          out.println(otherLines[i]);
        }
        out.print(key);
        out.print("=");
        out.println(value);
        out.close();
      }
    }

    String loadSetting(const String &key, const String &fallback) override {
      if (!ready || !SD.exists(SETTINGS_FILE)) {
        return fallback;
      }
      File f = SD.open(SETTINGS_FILE, FILE_READ);
      String result = fallback;
      while (f && f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        int eq = line.indexOf('=');
        if (eq < 0) {
          continue;
        }
        if (line.substring(0, eq) == key) {
          result = line.substring(eq + 1);
          break;
        }
      }
      if (f) {
        f.close();
      }
      return result;
    }
};

const char* SdStorage::LOG_FILE = "log.txt";
const char* SdStorage::SETTINGS_FILE = "settings.txt";
