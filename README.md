# Smart Restricted Access Monitoring System

Ce projet propose un système de sécurité intelligent conçu pour surveiller des zones à accès restreint (comme des salles de serveurs, des locaux sensibles, etc.) à l'aide d'un module ESP32-CAM, d’un capteur ultrasonique, et d’une application mobile connectée à Firebase.

##  Objectifs

- Détecter automatiquement toute intrusion à distance réduite (capteur ultrason < 15 cm)
- Capturer une image via la caméra ESP32-CAM
- Chiffrer l’image et les données (XOR) avant leur transmission
- Envoyer l’alerte en temps réel vers Firebase Realtime Database
- Permettre à l’utilisateur de consulter, valider ou ignorer l’alerte via une application Android sécurisée

##  Fonctionnalités

-  Détection de mouvement (capteur ultrason)
-  Capture automatique d’image avec ESP32-CAM
-  Chiffrement simple (XOR) des données avant envoi
-  Alerte envoyée à Firebase et affichée dans une app mobile Android
-  Activation/désactivation à distance avec clé secrète
-  Indicateurs LED : vert (désarmé) / rouge (armé)
-  Buzzer d’alarme activé en cas d’intrusion
-  Stockage des événements avec horodatage

##  Application mobile Android

- Interface utilisateur simple et sécurisée
- Authentification Firebase
- Affichage des alertes en temps réel (image + timestamp)
- Armement/désarmement sécurisé avec clé utilisateur
- Historique des alertes accessible via RecyclerView
- Intégration de Glide pour l'affichage d'images

## 🛠️ Technologies utilisées

- **Microcontrôleur** : ESP32-CAM
- **Capteur** : Capteur ultrason HC-SR04
- **Base de données** : Firebase Realtime Database
- **Langages** : C++ (Arduino), Java (Android)
- **Outils** : Arduino IDE, Android Studio
- **Librairies Android** : Glide, Firebase Auth, Firebase Database



