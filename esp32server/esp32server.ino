/*
  ESP32S3 Snapshot Web-Server
  ===========================
  • Board → ESP32S3 Dev Module  
  • Tools → PSRAM → Enabled  
  • Tools → Port → your /dev/cu.usbmodem…  
  • Serial Monitor → 115200 baud  

  1) The ESP connects to your Wi-Fi.
  2) It hosts two URLs:
     – “/”         serves a tiny HTML page with an <img> tag.
     – “/capture”  returns the last camera frame as a JPEG.
  3) Point your browser at http://<IP_ADDRESS>/ and watch the snapshot.
*/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// ←––– REPLACE these with your network’s SSID + PASSWORD –––→
const char* ssid     = "cal123";
const char* password = "cornrows"; 

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

WebServer server(80);

void handleRoot() {
  // Simple page with an <img> that hits /capture
  server.send(200, "text/html",
    "<!DOCTYPE html><html><body>"
    "<h1>ESP32S3 Snapshot</h1>"
    "<img src=\"/capture\" />"
    "<p>Refresh your browser to grab a new frame.</p>"
    "</body></html>"
  );
}

void handleCapture() {
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  // serve the JPEG buffer directly
  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // 1) initialize camera exactly as before
  bool hasPS = psramFound();
  camera_config_t cfg = {
    .pin_pwdn       = PWDN_GPIO_NUM,
    .pin_reset      = RESET_GPIO_NUM,
    .pin_xclk       = XCLK_GPIO_NUM,
    .pin_sscb_sda   = SIOD_GPIO_NUM,
    .pin_sscb_scl   = SIOC_GPIO_NUM,
    .pin_d7         = Y9_GPIO_NUM,
    .pin_d6         = Y8_GPIO_NUM,
    .pin_d5         = Y7_GPIO_NUM,
    .pin_d4         = Y6_GPIO_NUM,
    .pin_d3         = Y5_GPIO_NUM,
    .pin_d2         = Y4_GPIO_NUM,
    .pin_d1         = Y3_GPIO_NUM,
    .pin_d0         = Y2_GPIO_NUM,
    .pin_vsync      = VSYNC_GPIO_NUM,
    .pin_href       = HREF_GPIO_NUM,
    .pin_pclk       = PCLK_GPIO_NUM,
    .xclk_freq_hz   = 20000000,
    .ledc_timer     = LEDC_TIMER_0,
    .ledc_channel   = LEDC_CHANNEL_0,
    .pixel_format   = hasPS ? PIXFORMAT_JPEG    : PIXFORMAT_RGB565,
    .frame_size     = hasPS ? FRAMESIZE_CIF     : FRAMESIZE_QQVGA,
    .jpeg_quality   = hasPS ? 12                : 30,
    .fb_count       = 1,
    
    // where to allocate that buffer
    .fb_location    = hasPS ? CAMERA_FB_IN_PSRAM
                             : CAMERA_FB_IN_DRAM
  };
  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%02X\n", err);
    while(1) delay(1000);
  }
  Serial.println("Camera initialized.");

  // 2) it scans for all nearby SSIDs and prints them
  Serial.println("\nScanning for Wi-Fi networks…");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("  → No networks found!");
  } else {
    for (int i = 0; i < n; ++i) {
      Serial.printf("%2d: %s  (RSSI %d)\n", i+1,
                    WiFi.SSID(i).c_str(), WiFi.RSSI(i));
      delay(10);
    }
  }
  Serial.println("Scan complete.\n");

  // 3) connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Ready! Open this URL in your browser:\n  http://");
  Serial.println(WiFi.localIP());

  // 4) mount our two handlers
  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.begin();
}

void loop() {
  server.handleClient();
}
