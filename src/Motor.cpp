#include "Motor.hpp"

Motor::Motor() : grbl(STEPMOTOR_I2C_ADDR), isMotorPaused(true) {}

void Motor::initialize() {
  grbl.Init();
  delay(500);

  grbl.sendGcode((char *)"$X"); // Déverrouillage
  delay(100);

  // --- AJOUTS POUR LE CONTINU ---
  grbl.sendGcode(
      (char *)"$111=500"); // Vitesse max axe Y (mm/min) - Ajuste si besoin
  delay(50);
  grbl.sendGcode((char *)"$121=50"); // Accélération axe Y (mm/sec²) - Plus
                                     // c'est bas, plus c'est doux
  delay(50);
  // ------------------------------

  grbl.sendGcode((char *)"G91"); // Mode relatif
  delay(100);

  // M3 S1000 n'est utile que si tu as une broche de fraisage ou un laser.
  // Pour un tapis roulant ou un simple moteur, tu peux l'enlever ou le laisser.
  grbl.sendGcode((char *)"M3 S1000");
  delay(100);
}
void Motor::handleMotorInstructions(bool isProductReferenced,
                                    bool shouldMotorBeActive) {
  if (shouldMotorBeActive) {
    // On n'envoie la commande que si le moteur était à l'arrêt
    if (isMotorPaused) {
      // 1. Sortir GRBL de son état de pause/Hold (créé par le "!")
      grbl.sendGcode((char *)"~");
      delay(50);

      // 2. Envoyer le mouvement de l'axe Y
      grbl.sendGcode((char *)"G1 Y10000 F100");

      isMotorPaused = false;
      M5.Lcd.println("Moteur: MARCHE FORCEE");
    }
  } else {
    // On n'envoie le stop que si le moteur tournait
    if (!isMotorPaused) {
      grbl.sendGcode((char *)"!"); // Coupe le mouvement immédiatement (Hold)
      isMotorPaused = true;
      M5.Lcd.println("Moteur: ARRET");
    }
  }
}
void Motor::setMotorPaused(bool paused) { isMotorPaused = paused; }

bool Motor::isPaused() const { return isMotorPaused; }
