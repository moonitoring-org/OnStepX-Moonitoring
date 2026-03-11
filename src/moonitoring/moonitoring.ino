#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

String deviceId;
String youtubeChannelId = "";
double currentLat = 0.0;
double currentLon = 0.0;
unsigned long lastReport = 0;

void setupMoonitoring() {
  // Obtener MAC como ID único
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  deviceId = String(macStr);
  
  Serial.println("🔭 Moonitoring ID: " + deviceId);
}

String getCurrentISO8601() {
  time_t now = time(nullptr);
  struct tm* timeinfo = gmtime(&now);
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", timeinfo);
  return String(buffer);
}

void reportToFirebase() {
  if (millis() - lastReport < REPORT_INTERVAL * 1000) return;
  lastReport = millis();
  
  HTTPClient http;
  String url = "https://firestore.googleapis.com/v1/projects/" + 
               String(FIREBASE_PROJECT_ID) + 
               "/databases/(default)/documents/reportes?key=" + 
               String(FIREBASE_API_KEY);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  // Crear JSON con los datos
  StaticJsonDocument<512> doc;
  JsonObject fields = doc.createNestedObject("fields");
  
  fields["deviceId"]["stringValue"] = deviceId;
  fields["timestamp"]["timestampValue"] = getCurrentISO8601();
  fields["lat"]["doubleValue"] = currentLat;
  fields["lon"]["doubleValue"] = currentLon;
  fields["youtubeId"]["stringValue"] = youtubeChannelId;
  fields["activo"]["booleanValue"] = true;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpCode = http.POST(jsonString);
  
  if (httpCode == 200) {
    Serial.println("✅ Reporte enviado a Firebase");
  } else {
    Serial.println("❌ Error enviando reporte: " + String(httpCode));
  }
  
  http.end();
}

void activarModoMoonitoring(double lat, double lon, String ytId) {
  currentLat = lat;
  currentLon = lon;
  youtubeChannelId = ytId;
  
  // Reportar inmediatamente al activar
  reportToFirebase();
  
  Serial.println("🌕 MODO MOONITORING ACTIVADO");
  Serial.println("   Lat: " + String(lat, 6));
  Serial.println("   Lon: " + String(lon, 6));
  Serial.println("   YouTube: " + ytId);
}
