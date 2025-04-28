#define STBY_PIN   14   // 드라이버 활성화를 위한 Standby 핀
#define AIN1_PIN   23  // 모터 채널 A의 IN1
#define AIN2_PIN   22   // 모터 채널 A의 IN2
#define BIN1_PIN   1
#define BIN2_PIN   3
#define CIN1_PIN   21
#define CIN2_PIN   19
#define DIN1_PIN   18 
#define DIN2_PIN   5  
#define EIN1_PIN   17
#define EIN2_PIN   16
#define FIN1_PIN   4 //불들어옴
#define FIN2_PIN   2
#define GIN1_PIN   32
#define GIN2_PIN   33
#define HIN1_PIN   25
#define HIN2_PIN   26
#define PWMA_PIN   27  // 모터 채널 A의 PWM (속도 제어용)
#define P1   34
#define P2   35
#define P3   12
#define P4   13

void setup() {
  // 드라이버 제어에 사용되는 핀들을 출력 모드로 설정
  pinMode(P1, INPUT);
  pinMode(P3, OUTPUT);
  pinMode(P4, OUTPUT);
  digitalWrite(STBY_PIN, LOW);
  digitalWrite(P3, LOW);
  digitalWrite(P4, LOW);
  
}

void loop() {
  // === 모터 정방향 (앞으로) 회전 ===
  if (digitalRead(P1) == HIGH) {
    digitalWrite(P3, HIGH);
    } else {
    digitalWrite(P3, LOW);    // 비활성 → 소비 최소, 발열 ↓
  }
  delay(100);
  // (선택 사항) 모터 정지 상태: 모터를 잠시 정지시키고 싶다면 아래와 같이 처리할 수 있음.
  
}