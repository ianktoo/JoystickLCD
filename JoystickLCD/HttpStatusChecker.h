// --- HttpStatusChecker.h ---
// Plain HTTP/1.1 GET over WiFiClient, just to read back a status code.
// No external HTTP library needed for this one-shot use case.
#pragma once

#include <WiFiS3.h>

struct HttpCheckResult {
  bool reached;    // TCP connection + a response was received
  int statusCode;  // parsed HTTP status code, or -1 if it couldn't be parsed
};

class HttpStatusChecker {
  public:
    static HttpCheckResult check(const String &host, uint16_t port = 80, const String &path = "/", unsigned long timeoutMs = 8000) {
      HttpCheckResult result = { false, -1 };
      WiFiClient client;

      Serial.print("[HttpCheck] connecting to ");
      Serial.print(host);
      Serial.print(":");
      Serial.println(port);

      if (!client.connect(host.c_str(), port)) {
        Serial.println("[HttpCheck] TCP connect failed");
        return result;
      }
      result.reached = true;

      client.print("GET " + path + " HTTP/1.1\r\n");
      client.print("Host: " + host + "\r\n");
      client.print("Connection: close\r\n\r\n");

      String statusLine = "";
      unsigned long start = millis();
      while (client.connected() || client.available()) {
        if (client.available()) {
          statusLine = client.readStringUntil('\n');
          break;
        }
        if (millis() - start > timeoutMs) {
          Serial.println("[HttpCheck] timed out waiting for response");
          client.stop();
          return result;
        }
      }
      client.stop();

      Serial.print("[HttpCheck] status line: ");
      Serial.println(statusLine);

      // Expected form: "HTTP/1.1 200 OK\r"
      int firstSpace = statusLine.indexOf(' ');
      if (firstSpace >= 0) {
        int secondSpace = statusLine.indexOf(' ', firstSpace + 1);
        String codeStr = secondSpace > firstSpace
          ? statusLine.substring(firstSpace + 1, secondSpace)
          : statusLine.substring(firstSpace + 1);
        codeStr.trim();
        result.statusCode = codeStr.toInt();
      }
      return result;
    }
};
