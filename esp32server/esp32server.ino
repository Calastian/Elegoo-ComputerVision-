
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <esp32cam.h>

// camera pin definitions for your S3-Cam-LCD on the Dev board:
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM   4
#define SIOC_GPIO_NUM   5
#define Y2_GPIO_NUM    11
#define Y3_GPIO_NUM     9
#define Y4_GPIO_NUM     8
#define Y5_GPIO_NUM    10
#define Y6_GPIO_NUM    12
#define Y7_GPIO_NUM    18
#define Y8_GPIO_NUM    17
#define Y9_GPIO_NUM    16
#define VSYNC_GPIO_NUM  6
#define HREF_GPIO_NUM   7
#define PCLK_GPIO_NUM  13
#define RXD2 16
#define TXD2 17

// ←––– REPLACE these with your network’s SSID + PASSWORD –––→
const char* ssid     = "CalSpot";
const char* password = "ESPCALHACKER"; 
const char* URL = "/stream";
const int FRAMERATE = 10;
const auto RESOLUTION = esp32cam::Resolution::find(1024, 768);
WebServer server(80);



void handleStream() {
  static char head[128];
  WiFiClient client = server.client();

  server.sendContent("HTTP/1.1 200 OK\r\n"
                     "Content-Type: multipart/x-mixed-replace; "
                     "boundary=frame\r\n\r\n");

  while (client.connected()) {
    auto frame = esp32cam::capture();
    if (frame) {
      sprintf(head, 
              "--frame\r\n"
              "Content-Type: image/jepg\r\n",
              frame->size());
      client.write(head, strlen(head));
      frame->writeTo(client);
      client.write("\r\n");
    }
    delay(1000/FRAMERATE);
  }
}

void initCam() {
  using namespace esp32cam;
  Config cfg;
  cfg.setPins(pins::AiThinker);
  cfg.setResolution(RESOLUTION);
  cfg.setBufferCount(2);
  Camera.begin(cfg);
}

void initWifi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.printf("Stream at: http://%s%s\n", WiFi.localIP().toString().c_str(), URL);
  }

void initServer() {
  server.on(URL, handleStream);
  server.begin();
}

void setup() {
  Serial.begin(115200);
  initWifi();
  initCam();
  initServer();
}

void loop() {
  server.handleClient();
}


//void handleRoot() {
//  // Simple page with an <img> that hits /capture
//  
//   server.send(200, "text/html",
//  "<!DOCTYPE html><html><head>"
//  "<style>"
//  ".flipped {"
//  "  transform: rotate(180deg);"
//  "}"
//  "</style>"
//  // "<meta http-equiv='refresh' content='.5'>"  // Uncomment if needed
//  "</head><body>"
//  "<h1>ESP32S3 Snapshot</h1>"
// "<img src=\"/capture\" width=\"200\" />"
//  "<img src=\"/capture\" alt=\"Flipped Image\" class=\"flipped\" width=\"1200\" />"
//  "<p>Refresh your browser to grab a new frame.</p>"
//  "</body></html>"
//);

  
  
//}

//void handleCapture() {
//  camera_fb_t *fb = esp_camera_fb_get();
//  if(!fb) {
//    server.send(500, "text/plain", "Camera capture failed");
//    return;
//  }
//  // serve the JPEG buffer directly
//  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
//  esp_camera_fb_return(fb);
//}

//void setup() {
//  Serial.begin(115200);
//  delay(100);

//   // 1) initialize camera exactly as before
//   bool hasPS = psramFound();http://10.42.0.215
//   camera_config_t cfg = {
//     .pin_pwdn       = PWDN_GPIO_NUM,
//     .pin_reset      = RESET_GPIO_NUM,
//     .pin_xclk       = XCLK_GPIO_NUM,
//     .pin_sscb_sda   = SIOD_GPIO_NUM,
//     .pin_sscb_scl   = SIOC_GPIO_NUM,
//     .pin_d7         = Y9_GPIO_NUM,
//     .pin_d6         = Y8_GPIO_NUM,
//     .pin_d5         = Y7_GPIO_NUM,
//     .pin_d4         = Y6_GPIO_NUM,
//     .pin_d3         = Y5_GPIO_NUM,
//     .pin_d2         = Y4_GPIO_NUM,
//     .pin_d1         = Y3_GPIO_NUM,
//     .pin_d0         = Y2_GPIO_NUM,
//     .pin_vsync      = VSYNC_GPIO_NUM,
//     .pin_href       = HREF_GPIO_NUM,
//     .pin_pclk       = PCLK_GPIO_NUM,
//     .xclk_freq_hz   = 20000000,
//     .ledc_timer     = LEDC_TIMER_0,
//     .ledc_channel   = LEDC_CHANNEL_0,
//     .pixel_format   = hasPS ? PIXFORMAT_JPEG    : PIXFORMAT_RGB565,
//     .frame_size     = hasPS ? FRAMESIZE_CIF     : FRAMESIZE_QQVGA,
//     .jpeg_quality   = hasPS ? 12                : 30,
//     .fb_count       = 1,
    
//     // where to allocate that buffer
//     .fb_location    = hasPS ? CAMERA_FB_IN_PSRAM
//                              : CAMERA_FB_IN_DRAM
//   };
//   esp_err_t err = esp_camera_init(&cfg);
//   if (err != ESP_OK) {
//     Serial.printf("Camera init failed: 0x%02X\n", err);
//     while(1) delay(1000);
//   }
//   Serial.println("Camera initialized.");

//   // 2) it scans for all nearby SSIDs and prints them
//   Serial.println("\nScanning for Wi-Fi networks…");
//   int n = WiFi.scanNetworks();
//   if (n == 0) {
//     Serial.println("  → No networks found!");
//   } else {
//     for (int i = 0; i < n; ++i) {
//       Serial.printf("%2d: %s  (RSSI %d)\n", i+1,
//                     WiFi.SSID(i).c_str(), WiFi.RSSI(i));
//       delay(10);
//     }
//   }
//   Serial.println("Scan complete.\n");

//   // 3) connect to Wi-Fi
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println();
//   Serial.print("Ready! Open this URL in your browser:\n  http://");
//   Serial.println(WiFi.localIP());

//   // 4) mount our two handlers
//   server.on("/", handleRoot);
//   server.on("/capture", handleCapture);
//   server.begin();
// }

//void loop() {
//  server.handleClient();
  
//}
