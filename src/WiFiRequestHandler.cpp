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

  // 1. On s'assure que l'UID est en MAJUSCULES pour correspondre à Dolibarr
  uid.toUpperCase();

  // 2. Construction de l'URL locale (BASE_URL doit être
  // http://172.20.10.11:8080/api/index.php)
  String endpoint =
      "/products?sortfield=t.ref&sortorder=ASC&limit=1&sqlfilters=(t.ref%3D'" +
      uid + "')";
  String url = BASE_URL + endpoint;

  // 3. Utilisation du client standard (HTTP) au lieu de WiFiClientSecure
  WiFiClient client;
  HTTPClient http;

  Serial.println("Appel API local : " + url);

  if (http.begin(client, url)) {
    // 4. Headers nécessaires pour Dolibarr
    http.addHeader("DOLAPIKEY", API_KEY);
    // On retire le header ngrok car on est en local

    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();

        // Log de debug pour voir le JSON sur le moniteur série
        Serial.println("Succes ! Reponse brute: " + response);

        // On parse le produit
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
void WiFiRequestHandler::postStockMovement(Product product) {
  if (product.id == "" || product.warehouse_id == "") {
    M5.Lcd.println("Cannot post stock movement: invalid product");
    return;
  }

  String endpoint = "/stockmovements";
  String url = BASE_URL + endpoint;
  String payload = "{\"product_id\": " + product.id +
                   ",\"warehouse_id\": " + product.warehouse_id +
                   ",\"qty\": -1}";

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("DOLAPIKEY", API_KEY);

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
      M5.Lcd.println("Stock movement posted successfully");
    } else {
      M5.Lcd.printf("POST request failed with code: %d\n", httpCode);
    }
  } else {
    M5.Lcd.printf("POST request failed: %s\n",
                  http.errorToString(httpCode).c_str());
  }

  http.end();
}
