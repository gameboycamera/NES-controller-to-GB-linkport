#define GBP_SO_PIN  10
#define GBP_SC_PIN  9

#define PIN_LED     8

#define PIN_A       7
#define PIN_B       6
#define PIN_SELECT  5
#define PIN_START   4
#define PIN_UP      3
#define PIN_DOWN    2
#define PIN_LEFT    1
#define PIN_RIGHT   0

#define J_START     0x80u
#define J_SELECT    0x40u
#define J_B         0x20u
#define J_A         0x10u
#define J_DOWN      0x08u
#define J_UP        0x04u
#define J_LEFT      0x02u
#define J_RIGHT     0x01u

// Attiny:
// https://www.youtube.com/watch?v=TUlzOD9T3nI


inline uint8_t count_parity(uint8_t v) {
  uint8_t c = 0, b = 1;
  for (uint8_t i = 0; i != 4; i++, b <<= 1) {
    c += (v & b) ? 1 : 0;
  }    
  return c & 1;
}


void send_bit(bool b) {
  digitalWrite(GBP_SO_PIN, b);
  digitalWrite(GBP_SC_PIN, true);
  delay(1);
  digitalWrite(GBP_SC_PIN, false);   
  delay(1);
}

void send_byte(uint8_t b) {
  for (int i = 0; i != 8; i++, b <<= 1) {
    send_bit(b & 0x80);
  }
}

void send_joypad(uint8_t j) {
  send_byte((j & 0x0f) | ((count_parity(j)) ? 0x90 : 0x80));      // send D-pad
  send_byte((j >> 4) | ((count_parity(j >> 4)) ? 0xB0 : 0xA0));   // send buttons
}

void setup() {
  pinMode(PIN_LED, INPUT_PULLUP);
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_SELECT, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_LEFT, INPUT_PULLUP);
  pinMode(PIN_RIGHT, INPUT_PULLUP);
  
  pinMode(GBP_SO_PIN, OUTPUT);
  pinMode(GBP_SC_PIN, OUTPUT);

  pinMode(PIN_LED, OUTPUT);

  send_joypad(0x00);

  for (uint8_t i = 0; i < 25; i++) {
    digitalWrite(PIN_LED, i % 2);
    delay(30);
  }
}

uint8_t value = 1;
uint8_t prevValue = 0;

void loop() {
  value = (
    (digitalRead(PIN_A) ? 0 : J_A) +
    (digitalRead(PIN_B) ? 0 : J_B) +
    (digitalRead(PIN_SELECT) ? 0 : J_SELECT) +
    (digitalRead(PIN_START) ? 0 : J_START) +
    (digitalRead(PIN_UP) ? 0 : J_UP) +
    (digitalRead(PIN_DOWN) ? 0 : J_DOWN) +
    (digitalRead(PIN_LEFT) ? 0 : J_LEFT) +
    (digitalRead(PIN_RIGHT) ? 0 : J_RIGHT)
  );

  if (value != prevValue) {
    send_joypad(value);
    prevValue = value;
  }

  digitalWrite(PIN_LED, value ? HIGH : LOW);
  //digitalWrite(PIN_LED, (millis() >> 10) & 0x01); // Blink every (~1s)
}
