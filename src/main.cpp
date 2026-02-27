#include "Motor.hpp"
#include "RFIDReader.hpp" // Assure-toi que ce fichier contient bien les déclarations des nouvelles fonctions
#include "ServoMotor.hpp"
#include "WiFiRequestHandler.hpp"
#include <M5Stack.h>
#include <Wire.h>

// Instance du moteur
Motor motor;
ServoMotor servoMotor;
WiFiRequestHandler wifiHandler;
// ❌ SUPPRIMÉ : RFIDReader rfidReader; (On n'utilise plus de classe)

// État du moteur
bool motorRunning = false;

// --- FONCTION DE TEST API ---
void testInternetConnection() {
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.println("Test de requete HTTP...");
  Serial.println("Lancement du test HTTP sur jsonplaceholder...");

  HTTPClient http;
  // Appel vers une fausse API gratuite
  http.begin("http://jsonplaceholder.typicode.com/todos/1");
  int httpCode = http.GET();

  if (httpCode > 0) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.printf("Succes ! Code: %d\n", httpCode);
    Serial.println("Reponse API de test :");
    Serial.println(http.getString()); // Affiche le JSON dans ton terminal
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("Erreur HTTP !");
    Serial.printf("Erreur HTTP: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  delay(3000); // Laisse le temps de lire le résultat
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
}
// ----------------------------
void setup() {
  M5.begin(true, true, true, true);
  M5.Power.begin();
  Wire.begin();
  M5.Speaker.setVolume(5);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("=== TEST MOTEUR ===");
  M5.Lcd.println();

  // ==========================================
  // 1. TEST RÉSEAU (WIFI + API)
  // ==========================================
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Demarrage WiFi...");

  if (wifiHandler.connectWiFi()) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.print("IP: ");
    M5.Lcd.println(wifiHandler.getLocalIP());
    delay(1500);

    // On lance le petit test réseau !
    testInternetConnection();
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("ERREUR WIFI !");
    M5.Speaker.tone(500, 1000); // Gros bip d'erreur
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

  // Initialisation du lecteur RFID
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Initialisation RFID...");

  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Initialisation Servo...");
  servoMotor.initialize();
  servoMotor.setAngle(90); // Position centrale par défaut
  delay(500);
  // ✅ NOUVELLE FONCTION
  rfid_init();
  delay(500);

  // ❌ SUPPRIMÉ : rfidReader.showDetails(); (N'existe plus dans le nouveau
  // code)

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

  // ✅ NOUVELLE FONCTION : Détection RFID
  if (rfid_is_card_detected()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("=== CARTE DETECTEE ===");
    M5.Lcd.println();

    // Type de carte (On met un texte générique car getCardType n'existe plus)
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.print("Type: ");
    M5.Lcd.println("Tag NFC/RFID");
    M5.Lcd.println();

    // Lire les données du produit
    char message[64] = {0};
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println("Lecture UID en cours...");
    M5.Lcd.println();

    // ✅ NOUVELLE FONCTION : Lecture de l'UID
    // ✅ Lecture de l'UID
    if (rfid_read_product_code(message, sizeof(message))) {
      // 1. FAIRE UN BIP DE DÉTECTION
      M5.Speaker.tone(2000, 150);

      // 2. COUPER LE MOTEUR IMMÉDIATEMENT (Le tapis s'arrête pendant qu'on
      // cherche)
      motorRunning = false;
      motor.handleMotorInstructions(true, false);

      // 3. AFFICHAGE "RECHERCHE EN COURS"
      M5.Lcd.setTextColor(YELLOW);
      M5.Lcd.println("Interrogation BDD...");
      M5.Lcd.println(message);
      Serial.println("Envoi de la requete API pour l'UID: " + String(message));

      // --- 4. LE VRAI CALL DOLIBARR ---
      // On interroge Ngrok/Dolibarr avec le badge scanné
      Product scannedProduct = wifiHandler.getProductByUid(String(message));
      // --------------------------------
      if (scannedProduct.id != "") {

        // On récupère l'ID de l'entrepôt (fk_default_warehouse)
        // Si c'est vide dans Dolibarr, on met 1 par défaut pour éviter de
        // casser le calcul
        int warehouseId = (scannedProduct.warehouse_id != "")
                              ? scannedProduct.warehouse_id.toInt()
                              : 1;

        // 4. ORIENTATION DU BRAS
        int targetAngle = servoMotor.calculateAngle(warehouseId);
        servoMotor.setAngle(targetAngle);

        // 5. AFFICHAGE DU RÉSULTAT
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.println("Produit trouve !");
        M5.Lcd.printf("Label: %s\n", scannedProduct.label.c_str());
        M5.Lcd.printf("-> Entrepot: %d\n", warehouseId);

        // Confirmation sonore et pause pour laisser le bras bouger
        M5.Speaker.tone(1000, 100);
        delay(1500);

        // 6. REDÉMARRAGE DU TAPIS
        M5.Lcd.setTextColor(CYAN);
        M5.Lcd.println("Evacuation du colis...");
        motorRunning = true;
        motor.handleMotorInstructions(true, true);

        Serial.printf("=== SUCCÈS ===\n");
        Serial.printf("UID: %s | Entrepot: %d | Angle: %d\n", message,
                      warehouseId, targetAngle);

      } else {
        // CAS OÙ LE PRODUIT N'EST PAS DANS DOLIBARR
        M5.Lcd.setTextColor(RED);
        M5.Lcd.println("PRODUIT INCONNU !");
        M5.Speaker.tone(500, 500);
        Serial.println("Erreur: Produit non trouve dans la base.");
        // Le moteur reste arrêté pour sécurité
      }
    } else {
      // Bip d'erreur (son plus grave et plus long)
      M5.Speaker.tone(500, 300);

      M5.Lcd.setTextColor(RED);
      M5.Lcd.println("Erreur de lecture!");
      Serial.println("Erreur: Impossible de lire l'UID");
    } // ✅ NOUVELLE FONCTION : Arrêt de la comm
    rfid_stop_communication();
    delay(3000);
  }

  // Bouton A: Démarrer le moteur
  if (M5.BtnA.wasPressed()) {
    motorRunning = true;
    motor.handleMotorInstructions(true, true);
    servoMotor.setAngle(115);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println("MOTEUR");
    M5.Lcd.println("EN MARCHE");
    M5.Lcd.setTextSize(2);
    M5.Lcd.println();
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println("Commande: G1 X10000 F500");
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
    M5.Lcd.println("  Axe: X");
    M5.Lcd.println("  Vitesse max: 500");
  }

  delay(100);
}
