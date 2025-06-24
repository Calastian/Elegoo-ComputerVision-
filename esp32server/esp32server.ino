#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Replace with your Wi-Fi credentials
const char* ssid = "cal123";
const char* password = "cornrows";

// Camera pin configuration (same as yours)
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM   4
#define SIOC_GPIO_NUM   5
#define Y9_GPIO_NUM    16
#define Y8_GPIO_NUM    17
#define Y7_GPIO_NUM    18
#define Y6_GPIO_NUM    12
#define Y5_GPIO_NUM    10
#define Y4_GPIO_NUM     8
#define Y3_GPIO_NUM     9
#define Y2_GPIO_NUM    11
#define VSYNC_GPIO_NUM  6
#define HREF_GPIO_NUM   7
#define PCLK_GPIO_NUM  13

WebServer server(80);

void startCameraServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html",
      "<html><body><h1>ESP32 Stream</h1><img src='/stream' /></body></html>");
  });

  server.on("/stream", HTTP_GET, []() {
    WiFiClient client = server.client();
    String boundary = "boundarydonotcross";
    String header = "HTTP/1.1 200 OK\r\n" \
                    "Content-Type: multipart/x-mixed-replace; boundary=" + boundary + "\r\n\r\n";
    client.print(header);

    while (true) {
      camera_fb_t *fb = esp_camera_fb_get();
      if (!fb) continue;

      client.printf("--%s\r\n", boundary.c_str());
      client.print("Content-Type: image/jpeg\r\n");
      client.printf("Content-Length: %u\r\n\r\n", fb->len);
      client.write(fb->buf, fb->len);
      client.print("\r\n");

      esp_camera_fb_return(fb);

      if (!client.connected()) break;
    }
  });

  server.begin();
}

void setup() {
  Serial.begin(115200);

  camera_config_t config = {
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
    .pixel_format   = PIXFORMAT_JPEG,
    .frame_size     = FRAMESIZE_CIF,
    .jpeg_quality   = 12,
    .fb_count       = 1,
    .fb_location    = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM
  };

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("Stream URL: http://"); Serial.println(WiFi.localIP());

  startCameraServer();
}

void loop() {
  server.handleClient();
}
