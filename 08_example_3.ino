// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10

#define _DIST_MIN 100.0
#define _DIST_MID 200.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time = 0;


// =====================================================
// setup
// =====================================================
void setup()
{
  // GPIO initialization
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  // LED OFF at start
  // Active Low : 255 = OFF
  analogWrite(PIN_LED, 255);

  // Serial
  Serial.begin(57600);
}


// =====================================================
// loop
// =====================================================
void loop()
{
  float distance;
  int pwm;


  // -----------------------------------------
  // 25ms sampling interval
  // -----------------------------------------
  if (millis() - last_sampling_time < INTERVAL)
    return;

  last_sampling_time += INTERVAL;


  // -----------------------------------------
  // Distance measurement
  // -----------------------------------------
  distance = USS_measure(PIN_TRIG, PIN_ECHO);


  // -----------------------------------------
  // Calculate LED brightness
  // -----------------------------------------

  // Measurement failure
  // or outside desired range
  if ((distance == 0.0) ||
      (distance < _DIST_MIN) ||
      (distance > _DIST_MAX))
  {
    pwm = 255;          // LED OFF
  }


  // -----------------------------------------
  // 100mm ~ 200mm
  //
  // 100mm -> PWM 255 -> OFF
  // 150mm -> PWM 128 -> 50%
  // 200mm -> PWM   0 -> maximum brightness
  // -----------------------------------------
  else if (distance <= _DIST_MID)
  {
    pwm = (int)(
      255.0 -
      ((distance - _DIST_MIN)
      / (_DIST_MID - _DIST_MIN))
      * 255.0
      + 0.5
    );
  }


  // -----------------------------------------
  // 200mm ~ 300mm
  //
  // 200mm -> PWM   0 -> maximum brightness
  // 250mm -> PWM 128 -> 50%
  // 300mm -> PWM 255 -> OFF
  // -----------------------------------------
  else
  {
    pwm = (int)(
      ((distance - _DIST_MID)
      / (_DIST_MAX - _DIST_MID))
      * 255.0
      + 0.5
    );
  }


  // Safety limit
  pwm = constrain(pwm, 0, 255);


  // -----------------------------------------
  // Apply PWM to LED
  // -----------------------------------------
  analogWrite(PIN_LED, pwm);


  // -----------------------------------------
  // Serial Plotter output
  // -----------------------------------------
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",distance:");
  Serial.print(distance);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.print(",pwm:");
  Serial.print(pwm);

  Serial.println();
}


// =====================================================
// Ultrasonic Sensor
// Returns distance in millimeters
// =====================================================
float USS_measure(int TRIG, int ECHO)
{
  // Trigger pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);

  digitalWrite(TRIG, LOW);


  // Measure echo duration
  unsigned long duration =
    pulseIn(ECHO, HIGH, TIMEOUT);


  // Convert duration to distance
  return duration * SCALE;
}
