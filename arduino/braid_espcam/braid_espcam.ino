#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char* ssid = "12F-TP(E)-2.4G";
const char* password = "";
const char* uploadUrl = "http://192.168.35.58:5001/upload";
const char* checkTextUrl = "http://192.168.35.58:5001/check_text";

WebServer server(80);  // ESP32-CAM 기본 포트로 웹서버 실행

void connectToWiFi() {
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("WiFi 연결 중");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi 연결 완료");
  Serial.print("📶 ESP32 IP 주소: ");
  Serial.println(WiFi.localIP());
}

void initCamera() {
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 20;
  config.fb_count = 2;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("❌ 카메라 초기화 실패!");
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }

  Serial.println("✅ 카메라 초기화 성공");
}

bool checkIfTextRecognized() {
  HTTPClient http;
  http.begin(checkTextUrl);
  int code = http.GET();
  http.end();
  return (code == 200);
}

void uploadImage() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("📸 이미지 캡처 시작...");
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("❌ 이미지 캡처 실패");
      return;
    }

    HTTPClient http;
    http.begin(uploadUrl);
    http.setTimeout(10000);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(fb->buf, fb->len);

    if (httpResponseCode > 0) {
      Serial.printf("✅ 서버 응답 코드: %d\n", httpResponseCode);
    } else {
      Serial.printf("❌ 전송 실패. 오류 코드: %d (%s)\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
    }

    http.end();
    esp_camera_fb_return(fb);

    delay(2000);

    if (!checkIfTextRecognized()) {
      Serial.println("❗ 텍스트 없음. 다시 촬영합니다...");
      delay(1000);
      uploadImage();
    }
  } else {
    Serial.println("🚫 WiFi 연결이 끊어졌습니다.");
  }
}

void handleCaptureRequest() {
  Serial.println("📥 /capture 요청 수신됨 - 이미지 업로드 시작");
  uploadImage();
  server.send(200, "text/plain", "📷 이미지 캡처 완료");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("📷 ESP32-CAM 부팅 중...");
  connectToWiFi();
  initCamera();

  server.on("/capture", HTTP_GET, handleCaptureRequest);
  server.begin();
  Serial.println("🌐 ESP32-CAM HTTP 서버 시작됨 (포트 80)");
}

void loop() {
  server.handleClient();
  delay(10);
}