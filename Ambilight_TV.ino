#include <FastLED.h>

//----------------------НАСТРОЙКИ-----------------------
#define NUM_LEDS 206        // число светодиодов в ленте
#define DI_PIN 13           // пин, к которому подключена лента
#define OFF_TIME 10         // время (секунд), через которое лента выключится при пропадаании сигнала
#define serialRate 115200   // скорость связи с ПК
//----------------------НАСТРОЙКИ-----------------------

int new_bright, new_bright_f;
unsigned long bright_timer, off_timer;
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;  // кодовое слово Ada для связи
CRGB leds[NUM_LEDS];  // создаём ленту
boolean led_state = true;  // флаг состояния ленты


void setup()
{
  FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);  // инициализация светодиодов
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  Serial.begin(serialRate);
  Serial.print("Ada\n");     // Связаться с компом
}

void check_connection() {
  if (led_state) {
    if (millis() - off_timer > (OFF_TIME * 1000)) {
      led_state = false;
      FastLED.clear();
      FastLED.show();
    }
  }
}

void loop() {
  if (!led_state) led_state = true;
  off_timer = millis();  

  for (i = 0; i < sizeof prefix; ++i) {
    waitLoop: while (!Serial.available()) check_connection();;
    if (prefix[i] == Serial.read()) continue;
    i = 0;
    goto waitLoop;
  }

  while (!Serial.available()) check_connection();;
  hi = Serial.read();
  while (!Serial.available()) check_connection();;
  lo = Serial.read();
  while (!Serial.available()) check_connection();;
  chk = Serial.read();
  if (chk != (hi ^ lo ^ 0x55))
  {
    i = 0;
    goto waitLoop;
  }

  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    byte r, g, b;
    // читаем данные для каждого цвета
    while (!Serial.available()) check_connection();
    r = Serial.read();
    Serial.write(r);
    while (!Serial.available()) check_connection();
    g = Serial.read();
    while (!Serial.available()) check_connection();
    b = Serial.read();
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
  FastLED.show();  // записываем цвета в ленту
}
