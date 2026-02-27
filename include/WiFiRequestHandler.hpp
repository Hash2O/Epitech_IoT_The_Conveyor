#ifndef WIFI_REQUEST_HANDLER_HPP
#define WIFI_REQUEST_HANDLER_HPP

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include "Product.hpp"
#include "config.h" // Configuration WiFi et API (à modifier selon vos besoins)

class WiFiRequestHandler {
public:
  WiFiRequestHandler();
  ~WiFiRequestHandler();

  bool connectWiFi();
  String getLocalIP();

  Product getProductByUid(String uid);
  void postStockMovement(Product product);

private:
  WiFiMulti wiFiMulti;
};

#endif
