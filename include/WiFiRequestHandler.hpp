#ifndef WIFI_REQUEST_HANDLER_HPP
#define WIFI_REQUEST_HANDLER_HPP

#include <WiFi.h>
#include <M5Stack.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Product.hpp"
#include "config.h"  // Configuration WiFi et API (à modifier selon vos besoins)

class WiFiRequestHandler
{
public:
    WiFiRequestHandler();
    ~WiFiRequestHandler();

    bool connectWiFi();
    String getLocalIP();

    Product getProductById(int id);
    void postStockMovement(Product product);

private:
    WiFiMulti wiFiMulti;
};

#endif
