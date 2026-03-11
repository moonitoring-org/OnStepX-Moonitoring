#include "../plugins/website/Website.h"
#include "../moonitoring/moonitoring.ino"

// ============================================================================
// MOONITORING.ORG WEB INTERFACE
// ============================================================================

// Página de configuración de Moonitoring
server.on("/moonitoring", HTTP_GET, []() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>OnStepX - Moonitoring</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { 
            background: #0a0a1a; 
            color: white; 
            font-family: 'Roboto', sans-serif;
            margin: 0;
            padding: 20px;
        }
        .container { 
            max-width: 500px; 
            margin: auto; 
            background: #1a1a2e; 
            padding: 30px; 
            border-radius: 15px;
            border: 1px solid #00c8ff;
        }
        h1 { 
            color: #ff9a00; 
            font-family: 'Orbitron', sans-serif;
            text-align: center;
            margin-bottom: 30px;
        }
        label {
            display: block;
            color: #00c8ff;
            margin: 15px 0 5px;
            font-weight: 500;
        }
        input { 
            width: 100%; 
            padding: 12px; 
            margin: 5px 0 15px; 
            background: #333; 
            border: 1px solid #555;
            border-radius: 8px;
            color: white;
            font-size: 16px;
            box-sizing: border-box;
        }
        input:focus {
            outline: none;
            border-color: #00c8ff;
        }
        button { 
            width: 100%; 
            padding: 15px; 
            background: #ff9a00; 
            color: black; 
            border: none;
            border-radius: 8px;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            margin: 20px 0 10px;
            transition: all 0.3s;
        }
        button:hover { 
            background: #00c8ff; 
            transform: translateY(-2px);
        }
        .info {
            background: #0d0d1a;
            padding: 15px;
            border-radius: 8px;
            margin: 20px 0;
            border-left: 4px solid #ff9a00;
            font-size: 14px;
            color: #ccc;
        }
        .device-id {
            background: #333;
            padding: 10px;
            border-radius: 5px;
            font-family: monospace;
            font-size: 14px;
            text-align: center;
            margin: 10px 0;
            color: #00c8ff;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌙 Moonitoring</h1>
        
        <div class="info">
            <strong>📍 Device ID:</strong>
            <div class="device-id">)" + deviceId + R"rawliteral(</div>
            <p style="margin:10px 0 0; font-size:12px;">
                Este ID identifica tu estación en Firebase.
            </p>
        </div>
        
        <form action="/savemoon" method="POST">
            <label>🎥 YouTube Channel ID</label>
            <input type="text" name="ytid" placeholder="Ej: UCwvlOrXHQjGc9uwc98BM2Ag" required>
            <p style="color:#aaa; font-size:12px; margin-top:-10px;">
                El ID de tu canal (no el nombre)
            </p>
            
            <label>🌍 Latitud</label>
            <input type="number" step="0.000001" name="lat" placeholder="Ej: 19.4326" required>
            
            <label>🌍 Longitud</label>
            <input type="number" step="0.000001" name="lon" placeholder="Ej: -99.1332" required>
            
            <button type="submit">🚀 ACTIVAR MOONITORING</button>
        </form>
        
        <div class="info">
            <p><strong>¿Cómo obtener estos datos?</strong></p>
            <p>1. Tu YouTube Channel ID lo encuentras en <a href="https://youtube.com/account_advanced" target="_blank" style="color:#00c8ff;">YouTube Avanzado</a></p>
            <p>2. Tus coordenadas las puedes obtener de Google Maps</p>
            <p>3. Una vez activado, la estación reportará automáticamente cada 5 minutos</p>
        </div>
    </div>
</body>
</html>
  )rawliteral";
  
  server.send(200, "text/html", html);
});

// Procesar el formulario de Moonitoring
server.on("/savemoon", HTTP_POST, []() {
  String ytid = server.arg("ytid");
  String latStr = server.arg("lat");
  String lonStr = server.arg("lon");
  
  double lat = latStr.toDouble();
  double lon = lonStr.toDouble();
  
  // Mostrar en monitor serial
  Serial.println("📝 Recibida configuración Moonitoring:");
  Serial.println("   YouTube ID: " + ytid);
  Serial.println("   Lat: " + String(lat, 6));
  Serial.println("   Lon: " + String(lon, 6));
  
  // Activar el modo Moonitoring
  #ifdef MOONITORING_ENABLED
    activarModoMoonitoring(lat, lon, ytid);
  #endif
  
  // Redirigir de vuelta a la página de configuración
  server.sendHeader("Location", "/moonitoring");
  server.send(303);
});

// Página de estado en JSON
server.on("/moonitoring/status", HTTP_GET, []() {
  String json = "{";
  json += "\"deviceId\":\"" + deviceId + "\",";
  json += "\"activo\":true,";
  json += "\"youtubeId\":\"" + youtubeChannelId + "\",";
  json += "\"lat\":" + String(currentLat, 6) + ",";
  json += "\"lon\":" + String(currentLon, 6);
  json += "}";
  
  server.send(200, "application/json", json);
});
