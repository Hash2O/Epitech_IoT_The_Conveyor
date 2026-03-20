#include "Product.hpp"
#include <M5Stack.h>

Product::Product() {}

Product::Product(String id, String ref, String label, String warehouse_id,
                 String entity)
    : id(id), ref(ref), label(label), warehouse_id(warehouse_id),
      entity(entity) {}

Product Product::parseFromJson(const String &jsonResponse) {
  DynamicJsonDocument doc(2048);

  DeserializationError error = deserializeJson(doc, jsonResponse);

  if (error) {
    M5.Lcd.printf("Erreur JSON: %s\n", error.c_str());
    return Product();
  }

  if (doc.is<JsonArray>() && doc.size() > 0) {
    JsonObject firstProduct = doc[0];

    return Product(firstProduct["id"].as<String>(),
                   firstProduct["ref"].as<String>(),
                   firstProduct["label"].as<String>(),
                   firstProduct["fk_default_warehouse"]
                       .as<String>(), // Ton champ magique !
                   firstProduct["entity"].as<String>());
  }

  return Product(doc["id"].as<String>(), doc["ref"].as<String>(),
                 doc["label"].as<String>(),
                 doc["fk_default_warehouse"].as<String>(),
                 doc["entity"].as<String>());
}
void Product::printProduct() const {
  M5.Lcd.printf(
      "Id: %s\nRef: %s\nLabel: %s\nfk_default_warehouse: %s\nEntity: %s\n\n",
      id.c_str(), ref.c_str(), label.c_str(), warehouse_id.c_str(),
      entity.c_str());
}
