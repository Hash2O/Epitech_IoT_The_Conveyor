# 📋 Guide de Configuration du Projet Conveyor

Ce document explique comment configurer votre projet pour le faire fonctionner avec votre propre matériel et votre instance Dolibarr.

## 🔧 Fichier de Configuration

Toutes les informations sensibles sont centralisées dans le fichier **`include/config.h`**

---

## 📡 1. Configuration WiFi

### Où trouver ces informations ?

#### WIFI_SSID (Nom du réseau WiFi)
- **Sur iPhone/iPad** : Réglages → WiFi → Le nom du réseau connecté
- **Sur Android** : Paramètres → WiFi → Le nom du réseau connecté
- **Sur Windows** : Paramètres → Réseau et Internet → WiFi → Propriétés
- **Sur Mac** : Préférences Système → Réseau → WiFi

**Exemple** : `"MonReseauWiFi"`, `"iPhone de Julien"`, `"Epitech-Student"`

#### WIFI_PASSWORD (Mot de passe WiFi)
- C'est le mot de passe que vous utilisez pour vous connecter au réseau WiFi
- Si vous utilisez le partage de connexion d'un téléphone, c'est le mot de passe du point d'accès

**Exemple** : `"monMotDePasse123"`

### Configuration dans `include/config.h`
```cpp
#define WIFI_SSID "VOTRE_SSID_WIFI"
#define WIFI_PASSWORD "VOTRE_MOT_DE_PASSE"
```

---

## 🌐 2. Configuration Dolibarr API

### BASE_URL (URL de l'API Dolibarr)

#### Où trouver cette information ?

1. **Si Dolibarr est installé localement** :
   - Ouvrez votre navigateur
   - Accédez à votre instance Dolibarr (ex: `http://localhost/dolibarr`)
   - L'URL de l'API sera : `http://localhost/dolibarr/api/index.php`

2. **Si Dolibarr est sur un serveur distant** :
   - Demandez l'adresse IP ou le nom de domaine à votre administrateur
   - Format : `http://ADRESSE_IP_OU_DOMAINE/chemin/api/index.php`

**Exemples** :
- Local : `"http://localhost/dolibarr/api/index.php"`
- Réseau local : `"http://192.168.1.100/dolibarr/api/index.php"`
- Serveur distant : `"http://34.163.18.219/api/index.php"`
- Avec nom de domaine : `"https://erp.monentreprise.com/api/index.php"`

### API_KEY (Clé d'API Dolibarr)

#### Comment générer une clé API dans Dolibarr ?

1. **Connectez-vous à Dolibarr** avec un compte administrateur

2. **Activez le module API REST** :
   - Menu **Accueil** → **Configuration** → **Modules/Applications**
   - Recherchez "API/Services Web REST"
   - Cliquez sur **Activer** si ce n'est pas déjà fait

3. **Générez une clé API** :
   - Menu **Accueil** → **Utilisateurs & Groupes**
   - Sélectionnez l'utilisateur pour lequel vous voulez générer une clé
   - Cliquez sur l'onglet **"Token API"** ou **"Clés API"**
   - Cliquez sur **"Générer un nouveau token"**
   - Donnez un nom à la clé (ex: "M5Stack Conveyor")
   - Copiez la clé générée (elle ressemble à : `a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6`)

4. **⚠️ IMPORTANT** : Conservez cette clé en lieu sûr, elle ne sera affichée qu'une seule fois !

**Exemple de clé** : `"8tq9sVP2YEzbFk0k6H99UWoL49UtS1uw"`

### Configuration dans `include/config.h`
```cpp
#define BASE_URL "http://192.168.1.100/dolibarr/api/index.php"
#define API_KEY "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

---

## 🚀 Étapes de Configuration Complètes

1. **Ouvrez le fichier** `include/config.h`

2. **Modifiez les 4 valeurs** :
   ```cpp
   #define WIFI_SSID "VotreReseauWiFi"           // ← Votre WiFi
   #define WIFI_PASSWORD "VotreMotDePasse"       // ← Votre mot de passe WiFi
   #define BASE_URL "http://192.168.1.100/dolibarr/api/index.php"  // ← URL de votre Dolibarr
   #define API_KEY "votre_cle_api_generee"       // ← Votre clé API Dolibarr
   ```

3. **Sauvegardez le fichier**

4. **Compilez et uploadez** le projet sur le M5Stack :
   ```bash
   pio run --target upload
   ```

---

## 🔒 Sécurité

### ⚠️ NE JAMAIS commiter vos informations sensibles sur Git !

1. **Ajoutez** `include/config.h` dans votre `.gitignore` :
   ```
   include/config.h
   ```

2. **Créez un fichier d'exemple** `include/config.h.example` avec des valeurs factices :
   ```cpp
   #define WIFI_SSID "VOTRE_SSID_WIFI"
   #define WIFI_PASSWORD "VOTRE_MOT_DE_PASSE"
   #define BASE_URL "http://VOTRE_IP_SERVEUR/api/index.php"
   #define API_KEY "VOTRE_CLE_API_DOLIBARR"
   ```

3. **Commitez le fichier d'exemple**, pas le fichier réel !

---

## 🧪 Tester la Configuration

### Test WiFi
1. Uploadez le code sur le M5Stack
2. L'écran doit afficher "WiFi connected"
3. Si vous voyez "Connection failed!", vérifiez :
   - Le SSID est correct (sensible à la casse)
   - Le mot de passe est correct
   - Le M5Stack est à portée du réseau WiFi

### Test API Dolibarr
1. Scannez un tag RFID avec un ID de produit valide
2. L'écran doit afficher les informations du produit
3. Si vous voyez "HTTP GET request failed", vérifiez :
   - L'URL de base est correcte
   - La clé API est valide
   - Le serveur Dolibarr est accessible depuis le réseau WiFi

---

## 📞 Besoin d'Aide ?

### Problèmes courants

| Problème | Solution |
|----------|----------|
| "Connection failed!" | Vérifiez SSID et mot de passe WiFi |
| "HTTP GET request failed with code: 401" | Clé API invalide ou expirée |
| "HTTP GET request failed with code: 404" | URL de base incorrecte ou produit inexistant |
| "GET request failed: -1" | Serveur Dolibarr inaccessible (vérifiez l'IP/URL) |

### Logs de débogage
- Le M5Stack affiche les erreurs sur son écran LCD
- Utilisez le moniteur série pour voir plus de détails :
  ```bash
  pio device monitor
  ```

---

## 📚 Ressources Utiles

- [Documentation Dolibarr API](https://wiki.dolibarr.org/index.php/Module_Web_Services_API_REST_(developer))
- [M5Stack Documentation](https://docs.m5stack.com/)
- [PlatformIO Documentation](https://docs.platformio.org/)

---

**Dernière mise à jour** : 2026-02-27

