#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

const char* ssid = "11F-4";
const char* password = "";
const char* flaskServer = "http://192.168.35.58:5001/";
WebServer server(8080);

#define STBY_PIN   14
#define AIN1_PIN   23
#define AIN2_PIN   22
#define BIN1_PIN   1
#define BIN2_PIN   3
#define CIN1_PIN   21
#define CIN2_PIN   19
#define DIN1_PIN   18
#define DIN2_PIN   5
#define EIN1_PIN   17
#define EIN2_PIN   16
#define FIN1_PIN   4
#define FIN2_PIN   2
#define GIN1_PIN   32
#define GIN2_PIN   33
#define HIN1_PIN   25
#define HIN2_PIN   26
#define PWMA_PIN   27
#define BUTTON_PIN 34
#define VIBE_PIN1 12
#define VIBE_PIN2 13

int IN1_PINS[8] = {EIN1_PIN, AIN1_PIN, FIN1_PIN, BIN1_PIN, GIN1_PIN, CIN1_PIN, HIN1_PIN, DIN1_PIN};
int IN2_PINS[8] = {EIN2_PIN, AIN2_PIN, FIN2_PIN, BIN2_PIN, GIN2_PIN, CIN2_PIN, HIN2_PIN, DIN2_PIN};

int currentSpeed = 600;
volatile unsigned long lastPressTime = 0;
volatile int pressCount = 0;

void IRAM_ATTR handleButtonPress() {
  unsigned long now = millis();
  if (now - lastPressTime < 500) {
    pressCount++;
  } else {
    pressCount = 1;
  }
  lastPressTime = now;
}

int getDelayForSpeed(int speed) {
  switch (speed) {
    case 1: return 1000;
    case 2: return 800;
    case 3: return 600;
    case 4: return 400;
    case 5: return 200;
    default: return 600;
  }
}

void activatePin(int index, int delayTime) {
  digitalWrite(STBY_PIN, HIGH);
  digitalWrite(IN1_PINS[index], HIGH);
  digitalWrite(IN2_PINS[index], LOW);
  delay(delayTime);
  digitalWrite(IN1_PINS[index], LOW);
  digitalWrite(IN2_PINS[index], LOW);
  digitalWrite(STBY_PIN, LOW);
}

void deactivateAllPins() {
  digitalWrite(STBY_PIN, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(IN1_PINS[i], LOW);
    digitalWrite(IN2_PINS[i], HIGH);
    delay(100);
    digitalWrite(IN2_PINS[i], LOW);
  }
  digitalWrite(STBY_PIN, LOW);
  Serial.println("🔽 모든 점자 셀 내림 완료");
}

void vibrate() {
  digitalWrite(VIBE_PIN1, HIGH);
  digitalWrite(VIBE_PIN2, HIGH);
  delay(200);
  digitalWrite(VIBE_PIN1, LOW);
  digitalWrite(VIBE_PIN2, LOW);
}

void notifyFlaskWordDone() {
  HTTPClient http;
  http.begin(String(flaskServer) + "/word_done");
  http.setTimeout(3000);  // 응답 대기시간 증가
  int code = http.POST("");
  if (code > 0) {
    Serial.println("📨 서버에 완료 알림 전송 완료");
  } else {
    Serial.println("❌ 완료 알림 실패");
  }
  http.end();
}

void outputBraille(String word) {
  Serial.println("📤 점자 출력: " + word);
  for (int i = 0; i < word.length() && i < 8; i++) {
    if (word[i] != ' ') {
      activatePin(i, currentSpeed);
    }
  }
  delay(1000);
  deactivateAllPins();
  vibrate();
  notifyFlaskWordDone();
}

String getWordFromFlask(String endpoint) {
  HTTPClient http;
  String serverUrl = String(flaskServer) + "/" + endpoint;
  http.begin(serverUrl);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("📥 받은 단어: " + payload);
    return payload;
  } else {
    Serial.println("❌ 단어 요청 실패");
    return "";
  }
  http.end();
}

void handlePost() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "❌ No body found");
    return;
  }

  String input = server.arg("plain");
  input.trim();

  Serial.println("📥 받은 HTTP 데이터:");
  Serial.println(input);

  if (input.startsWith("test-speed-")) {
    int speed = input.substring(11).toInt();
    currentSpeed = getDelayForSpeed(speed);
    Serial.print("🧪 테스트 속도: ");
    Serial.println(speed);

    int braille[5][8] = {
      {1,0,1,1,0,1,0,0},
      {1,0,0,0,0,1,0,0},
      {1,0,0,0,0,1,1,0},
      {1,0,0,0,0,1,1,0},
      {1,0,1,0,1,1,0,0}
    };

    for (int i = 0; i < 5; i++) {
      Serial.printf("🔠 출력 중: %c\n", "HELLO"[i]);
      for (int j = 0; j < 8; j++) {
        if (braille[i][j] == 1) {
          activatePin(j, currentSpeed);
        }
      }
      delay(1000);
      deactivateAllPins();
      vibrate();
    }

    server.send(200, "text/plain", "✅ HELLO 점자 출력 완료");
    return;
  }

  if (input.startsWith("speed-")) {
    int colonIndex = input.indexOf(":");
    if (colonIndex == -1) {
      server.send(400, "text/plain", "Invalid format");
      return;
    }

    String speedStr = input.substring(6, colonIndex);
    String brailleDataStr = input.substring(colonIndex + 1);

    int speed = speedStr.toInt();
    currentSpeed = getDelayForSpeed(speed);

    brailleDataStr.replace("[", "");
    brailleDataStr.replace("]", "");
    brailleDataStr.replace(" ", "");

    int brailleBits[8] = {0};
    int index = 0;
    char *ptr = strtok((char*)brailleDataStr.c_str(), ",");

    while (ptr != NULL && index < 8) {
      brailleBits[index++] = atoi(ptr);
      ptr = strtok(NULL, ",");
    }

    Serial.print("⏱ 속도 단계: ");
    Serial.println(speed);
    Serial.print("📤 점자 배열: ");
    Serial.println(brailleDataStr);

    for (int i = 0; i < 8; i++) {
      if (brailleBits[i] == 1) {
        activatePin(i, currentSpeed);
      }
    }
    delay(1000);
    deactivateAllPins();
    vibrate();
    server.send(200, "text/plain", "✅ Braille output done");
    return;
  }

  server.send(400, "text/plain", "❌ Unknown command");
}

void handlePing() {
  server.send(200, "text/plain", "pong");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("📶 WiFi 연결 시도 중...");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi 연결 완료");
    Serial.print("📡 ESP32 IP 주소: ");
    Serial.println(WiFi.localIP());

    pinMode(STBY_PIN, OUTPUT);
    pinMode(PWMA_PIN, OUTPUT);
    analogWrite(PWMA_PIN, 200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(VIBE_PIN1, OUTPUT);
    pinMode(VIBE_PIN2, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

    for (int i = 0; i < 8; i++) {
      pinMode(IN1_PINS[i], OUTPUT);
      pinMode(IN2_PINS[i], OUTPUT);
      digitalWrite(IN1_PINS[i], LOW);
      digitalWrite(IN2_PINS[i], LOW);
    }

    server.on("/receive", HTTP_POST, handlePost);
    server.on("/ping", HTTP_GET, handlePing);
    server.begin();
    Serial.println("🌐 HTTP 서버 시작됨 (포트 8080)");
  } else {
    Serial.println("\n❌ WiFi 연결 실패!");
  }
}

void loop() {
  server.handleClient();

  if (pressCount > 0) {
    delay(100);
    if (pressCount == 1) {
      String nextWord = getWordFromFlask("next_word");
      if (nextWord == "CAPTURE") {
        Serial.println("📸 재촬영 요청!");
      } else if (nextWord != "") {
        outputBraille(nextWord);
      }
    } else if (pressCount >= 2) {
      String prevWord = getWordFromFlask("prev_word");
      if (prevWord != "") {
        outputBraille(prevWord);
      }
    }
    pressCount = 0;
  }
}