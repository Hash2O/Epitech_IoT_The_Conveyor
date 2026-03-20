#include "WiFiRequestHandler.hpp"

WiFiRequestHandler::WiFiRequestHandler() {
  wiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
}

WiFiRequestHandler::~WiFiRequestHandler() {}

bool WiFiRequestHandler::connectWiFi() {
  int attemptCount = 0;
  while (wiFiMulti.run() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(300);
    attemptCount++;
    if (attemptCount > 20) {
      M5.Lcd.println("\nConnection failed!");
      return false;
    }
  }
  M5.Lcd.println("\nWiFi connected");
  return true;
}

String WiFiRequestHandler::getLocalIP() { return WiFi.localIP().toString(); }

Product WiFiRequestHandler::getProductByUid(String uid) {
  Product product;

  uid.toUpperCase();

  String endpoint =
      "/products?sortfield=t.ref&sortorder=ASC&limit=1&sqlfilters=(t.ref%3D'" +
      uid + "')";
  String url = BASE_URL + endpoint;

  WiFiClient client;
  HTTPClient http;

  Serial.println("Appel API local : " + url);

  if (http.begin(client, url)) {
    http.addHeader("DOLAPIKEY", API_KEY);

    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();

        Serial.println("Succes ! Reponse brute: " + response);

        product = Product::parseFromJson(response);
      } else {
        Serial.printf("Erreur HTTP: %d\n", httpCode);
      }
    } else {
      Serial.printf("Erreur connexion: %s\n",
                    http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Impossible d'initialiser la connexion HTTP");
  }

  return product;
}
