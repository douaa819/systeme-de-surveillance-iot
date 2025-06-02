#include "esp_camera.h"
#include <Firebase_ESP_Client.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// Configuration Wi-Fi
#define WIFI_SSID "TP-Link_A81E"
#define WIFI_PASSWORD "11161219"

// Configuration Firebase
#define API_KEY "AIzaSyC_oogZBySxMsNHiSZdzrSt8ytsy6PhyYs"
#define DATABASE_URL "https://smartsecurityfirebase-default-rtdb.firebaseio.com"
#define USER_EMAIL "douaazammit5@gmail.com"
#define USER_PASSWORD "92674208"

// Authentification Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Pins LED et buzzer
#define RED_LED_PIN 33
#define GREEN_LED_PIN 26
#define BUZZER_PIN 32

// Pins capteur ultrason
#define TRIG_PIN 13
#define ECHO_PIN 14

// Config caméra ESP32-CAM
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("\n[ERREUR] Caméra non initialisée.\n");
  } else {
    Serial.println("\n[OK] Caméra initialisée.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté. IP locale : " + WiFi.localIP().toString());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  timeClient.begin();
  timeClient.update();
  startCamera();

  
  server.on("/cam", HTTP_GET, []() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      server.send(500, "text/plain", "Erreur lors de la capture d'image");
      return;
    }
    server.sendHeader("Content-Type", "image/jpeg");
    server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
    server.send_P(200, "image/jpeg", (char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  server.begin();
}

void loop() {
  server.handleClient();  // Nécessaire pour gérer les requêtes HTTP

  timeClient.update();
  float distance = readDistanceCM();
  Serial.printf("\nDistance : %.2f cm", distance);

  if (distance < 15.0) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("\n[ERREUR] Capture image échouée !");
      return;
    } else {
      Serial.println("\n[SUCCESS] Image capturée.");
    }
    esp_camera_fb_return(fb);

    String timestamp = timeClient.getFormattedTime();
    String imageURL = "http://" + WiFi.localIP().toString() + "/cam";

    FirebaseJson json;
    json.set("timestamp", timestamp);
    json.set("distance", distance);
    json.set("imageUrl", imageURL);

    if (Firebase.RTDB.pushJSON(&fbdo, "/alerts", &json)) {
      Serial.println("[OK] Données envoyées à Firebase.");
    } else {
      Serial.println("[ERREUR] Envoi Firebase.");
      Serial.println(fbdo.errorReason());
    }
  }

  if (Firebase.RTDB.getBool(&fbdo, "/systemStatus/isArmed")) {
    bool armed = fbdo.boolData();
    if (armed) {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }

  delay(2000);
}


