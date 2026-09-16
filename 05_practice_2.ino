const int LED_PIN = 7;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  // 1. 처음 1초 동안 LED 켜기
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  // 2. 다음 1초 동안 LED 5번 깜빡이기
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);

    digitalWrite(LED_PIN, LOW);
    delay(100);
  }

  // 3. LED 끄기
  digitalWrite(LED_PIN, HIGH);

  // 4. 여기서 영원히 정지
  while (1) {
  }
}
