#define LED_PIN 7

int pwm_period = 1000;
int pwm_duty = 50;

void set_period(int period) {
    pwm_period = period;
    Serial.begin(115200);
}

void set_duty(int duty) {
    pwm_duty = duty;
    Serial.println(pwm_duty); //시각적 확인을 위한
}

void pwm_once() {

    unsigned long on_time =
        (unsigned long)pwm_period * pwm_duty / 100;

    unsigned long off_time =
        pwm_period - on_time;

    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(on_time);

    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(off_time);
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
}

void loop() {

    set_period(100);

    for (int duty = 0; duty <= 100; duty++) {
        set_duty(duty);

        for (int i = 0; i < 5; i++) {
            pwm_once();
        }
    }

    for (int duty = 99; duty >= 0; duty--) {
        set_duty(duty);

        for (int i = 0; i < 5; i++) {
            pwm_once();
        }
    }
}
