#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CalSpot";
const char* password = "ESPCALHACKER";

WebServer server(80);

// Serial1 pins
#define RXD2 16
#define TXD2 17 // I swapped these two

// Function to add CORS headers
void addCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void setup() {
  Serial.begin(115200);  // Debug
  // Serial.begin(9600);
  // Serial1.begin(115200);
  // HardwareSerial.begin();
  Serial1.begin(9600, SERIAL_8N1, -1, 17);  // Communicate with Uno

  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  // Handle preflight OPTIONS for all routes
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      addCORSHeaders();
      server.send(204); // No Content
    } else {
      server.send(404, "text/plain", "Not found");
    }
  });

  server.on("/forward", HTTP_GET, []() {
    Serial1.write("F");
    addCORSHeaders();
    server.send(200, "text/plain", "Forward");
    Serial.println("\nForward");
  });

  server.on("/backward", HTTP_GET, []() {
    Serial1.println("B");
    addCORSHeaders();
    server.send(200, "text/plain", "Backward");
    Serial.println("\nBackward");
  });

  server.on("/left", HTTP_GET, []() {
    Serial1.println("L");
    addCORSHeaders();
    server.send(200, "text/plain", "Left");
    Serial.println("\nLeft");
  });

  server.on("/right", HTTP_GET, []() {
    Serial1.println("R");
    addCORSHeaders();
    server.send(200, "text/plain", "Right");
    Serial.println("\nRight");
  });

  server.on("/stop", HTTP_GET, []() {
    Serial1.println("S");
    addCORSHeaders();
    server.send(200, "text/plain", "Stop");
    Serial.println("\nStop");
  });

  server.on("/ok", HTTP_GET, []() {
    Serial1.println("O");
    addCORSHeaders();
    server.send(200, "text/plain", "ok");
    Serial.println("\nOk");
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
