
 // Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100
#define _DIST_MAX 300

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.5
#define MEDIAN_N 3 // 3, 10, 30으로 변경

// global variables
unsigned long last_sampling_time;
float dist_ema;
bool ema_initialized = false;

// 중위수 필터용 배열
float median_samples[MEDIAN_N];
int median_count = 0;

void addMedianSample(float value) {
  if (median_count < MEDIAN_N) {
    median_samples[median_count] = value;
    median_count++;
  } else {
    for (int i = 0; i < MEDIAN_N - 1; i++) {
      median_samples[i] = median_samples[i + 1];
    }
    median_samples[MEDIAN_N - 1] = value;
  }
}

// 중위수 계산
float getMedian() {
  float sorted[MEDIAN_N];

  // 배열 복사
  for (int i = 0; i < median_count; i++) {
    sorted[i] = median_samples[i];
  }

  // 삽입 정렬
  for (int i = 1; i < median_count; i++) {
    float key = sorted[i];
    int j = i - 1;

    while (j >= 0 && sorted[j] > key) {
      sorted[j + 1] = sorted[j];
      j--;
    }
    sorted[j + 1] = key;
  }

  // 홀수인 경우
  if (median_count % 2 == 1) {
    return sorted[median_count / 2];
  }

  // 짝수인 경우
  int right = median_count / 2;
  return (sorted[right - 1] + sorted[right]) / 2.0;
}

void setup() {

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_median;

  if (millis() - last_sampling_time < INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // 중위수 필터 적용
  addMedianSample(dist_raw);
  dist_median = getMedian();

  // EMA 계산 (중위수 필터와 독립)
  if (!ema_initialized) {
    dist_ema = dist_raw;
    ema_initialized = true;
  } else {
    dist_ema = _EMA_ALPHA * dist_raw
             + (1.0 - _EMA_ALPHA) * dist_ema;
  }

  // output the distance to the serial port
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",raw:");
  Serial.print(dist_raw);

  Serial.print(",ema:");
  Serial.print(dist_ema);

  Serial.print(",median:");
  Serial.print(dist_median);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println("");

  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1); // LED OFF
  else
    digitalWrite(PIN_LED, 0); // LED ON

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
