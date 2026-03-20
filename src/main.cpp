#include "Motor.hpp"
#include "RFIDReader.hpp"
#include "ServoMotor.hpp"
#include "WiFiRequestHandler.hpp"
#include <M5Stack.h>
#include <Wire.h>

// Instances des modules
Motor motor;
ServoMotor servoMotor;
WiFiRequestHandler wifiHandler;

// État du moteur
bool motorRunning = false;
void setup() {
  M5.begin(true, true, true, true);
  M5.Power.begin();
  Wire.begin();
  M5.Speaker.setVolume(5);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("=== INITIALISATION ===");
  M5.Lcd.println();

  // Connexion WiFi
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Demarrage WiFi...");

  if (wifiHandler.connectWiFi()) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.print("IP: ");
    M5.Lcd.println(wifiHandler.getLocalIP());
    delay(1500);
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("ERREUR WIFI !");
    M5.Speaker.tone(500, 1000);
    delay(3000);
  }

  // Vérification du module GRBL
  Wire.beginTransmission(0x70);
  if (Wire.endTransmission() == 0) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Module GRBL OK (0x70)");
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("ERREUR: Module non trouve!");
    M5.Lcd.println("Verifiez le cablage I2C");
    while (1) {
      delay(1000);
    }
  }

  // Initialisation du moteur
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Initialisation moteur...");
  motor.initialize();
  delay(500);

  // Initialisation du servo moteur
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Initialisation Servo...");
  servoMotor.initialize();
  servoMotor.setAngle(90);
  delay(500);

  // Initialisation du lecteur RFID
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Initialisation RFID...");
  rfid_init();
  delay(500);

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.println();
  M5.Lcd.println("Pret!");
  M5.Lcd.println();
  M5.Lcd.println("Bouton A: START");
  M5.Lcd.println("Bouton B: STOP");
  M5.Lcd.println("Bouton C: INFO");
}

void loop() {
  M5.update();

  // Détection RFID
  if (rfid_is_card_detected()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("=== CARTE DETECTEE ===");
    M5.Lcd.println();

    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.print("Type: ");
    M5.Lcd.println("Tag NFC/RFID");
    M5.Lcd.println();

    char message[64] = {0};
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println("Lecture UID en cours...");
    M5.Lcd.println();

    if (rfid_read_product_code(message, sizeof(message))) {
      // Bip de détection
      M5.Speaker.tone(2000, 150);

      // Arrêt du moteur pendant la recherche
      motorRunning = false;
      motor.handleMotorInstructions(true, false);

      // Affichage de la recherche
      M5.Lcd.setTextColor(YELLOW);
      M5.Lcd.println("Interrogation BDD...");
      M5.Lcd.println(message);
      Serial.println("Envoi de la requete API pour l'UID: " + String(message));

      // Appel Dolibarr
      Product scannedProduct = wifiHandler.getProductByUid(String(message));

      if (scannedProduct.id != "") {
        // Récupération de l'ID entrepôt
        int warehouseId = (scannedProduct.warehouse_id != "")
                              ? scannedProduct.warehouse_id.toInt()
                              : 1;

        // Orientation du bras
        int targetAngle = servoMotor.calculateAngle(warehouseId);
        servoMotor.setAngle(targetAngle);

        // Affichage du résultat
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.println("Produit trouve !");
        M5.Lcd.printf("Label: %s\n", scannedProduct.label.c_str());
        M5.Lcd.printf("-> Entrepot: %d\n", warehouseId);

        // Confirmation sonore
        M5.Speaker.tone(1000, 100);
        delay(1500);

        // Redémarrage du tapis
        M5.Lcd.setTextColor(CYAN);
        M5.Lcd.println("Evacuation du colis...");
        motorRunning = true;
        motor.handleMotorInstructions(true, true);

        Serial.printf("=== SUCCÈS ===\n");
        Serial.printf("UID: %s | Entrepot: %d | Angle: %d\n", message,
                      warehouseId, targetAngle);

      } else {
        // Produit non trouvé
        M5.Lcd.setTextColor(RED);
        M5.Lcd.println("PRODUIT INCONNU !");
        M5.Speaker.tone(500, 500);
        Serial.println("Erreur: Produit non trouve dans la base.");
      }
    } else {
      // Erreur de lecture
      M5.Speaker.tone(500, 300);
      M5.Lcd.setTextColor(RED);
      M5.Lcd.println("Erreur de lecture!");
      Serial.println("Erreur: Impossible de lire l'UID");
    }

    rfid_stop_communication();
    delay(3000);
  }

  // Bouton A: Démarrer le moteur
  if (M5.BtnA.wasPressed()) {
    motorRunning = true;
    motor.handleMotorInstructions(true, true);
    servoMotor.setAngle(90);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println("MOTEUR");
    M5.Lcd.println("EN MARCHE");
    M5.Lcd.setTextSize(2);
    M5.Lcd.println();
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println("Commande: G1 Y10000 F150");
  }

  // Bouton B: Arrêter le moteur
  if (M5.BtnB.wasPressed()) {
    motorRunning = false;
    motor.handleMotorInstructions(true, false);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println("MOTEUR");
    M5.Lcd.println("ARRETE");
  }

  // Bouton C: Afficher info
  if (M5.BtnC.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("=== INFORMATIONS ===");
    M5.Lcd.println();
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.printf("Etat: %s\n", motorRunning ? "EN MARCHE" : "ARRETE");
    M5.Lcd.println();
    M5.Lcd.println("Config GRBL:");
    M5.Lcd.println("  Axe: Y");
    M5.Lcd.println("  Vitesse max: 500");
  }

  delay(100);
}
