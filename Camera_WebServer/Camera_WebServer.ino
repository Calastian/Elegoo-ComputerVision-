#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "cal123";
const char* password = "cornrows";
HardwareSerial mySerial(2);
WebServer server(80);


// Serial1 pins
#define RXD2 -1
#define TXD2 4 // I swapped these two

// Function to add CORS headers
void addCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void setup() {

  Serial.begin(115200);  // Debug
  Serial.begin(9600);
  // Serial1.begin(115200);
  // HardwareSerial.begin();
  // mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2);  // Communicate with Uno

  // Serial
  // mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2);






  delay(1000);

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
      server.send(204);  // No Content
    } else {
      server.send(404, "text/plain", "Not found");
    }
  });

  server.on("/forward", HTTP_GET, []() {
    SerialPrint('F');
    addCORSHeaders();
    server.send(200, "text/plain", "Forward");
    // Serial.println("\nForward");
  });

  server.on("/back", HTTP_GET, []() {
    SerialPrint('B');
    addCORSHeaders();
    server.send(200, "text/plain", "Backward");
    // Serial.println("\nBackward");
  });

  server.on("/left", HTTP_GET, []() {
    SerialPrint('L');
    addCORSHeaders();
    server.send(200, "text/plain", "Left");
    // Serial.println("\nLeft");
  });

  server.on("/right", HTTP_GET, []() {
    SerialPrint('R');
    addCORSHeaders();
    server.send(200, "text/plain", "Right");
    // Serial.println("\nRight");
  });

  server.on("/stop", HTTP_GET, []() {
    SerialPrint('S');
    addCORSHeaders();
    server.send(200, "text/plain", "Stop");
    // Serial.println("\nStop");
  });

  server.on("/ok", HTTP_GET, []() {
    SerialPrint('O');
    addCORSHeaders();
    server.send(200, "text/plain", "ok");
    // Serial.println("\nOk");
  });

  server.begin();
  Serial.println("Server started");
}




void loop() {
  server.handleClient();
  // mySerial.print('L');

  // Serial.print("lol");
}




void SerialPrint(char lol) {
  // mySerial.write(lol);
  Serial.write(lol);
}
