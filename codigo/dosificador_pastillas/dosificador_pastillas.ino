#include <WiFi.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>
#include "TickTwo.h"
#include <Wire.h>
#include "RTClib.h"

// ==================== DEFINICIONES ==================== //
#define LED_PIN 17
#define LED_PIN2 18
#define LED_PIN3 19
#define NUM_LEDS 7
#define BUZZER 5
#define BUTTON 26

Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led2 = Adafruit_NeoPixel(NUM_LEDS, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led3 = Adafruit_NeoPixel(NUM_LEDS, LED_PIN3, NEO_GRB + NEO_KHZ800);

// WiFi Configuración
const char* ssid = "MediaLab guest";
const char* password = "medialab2019";

// RTC
RTC_DS3231 rtc;

// Estado del botón/interrupción
volatile bool intFlag = false;
bool usarRTC = false;

// ==================== FUNCIONES ==================== //

// Interrupción del botón
void ButtonInterrupt() {
  intFlag = true;
}

void apagarTodosLosLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    led.setPixelColor(i, 0, 0, 0);
    led2.setPixelColor(i, 0, 0, 0);
    led3.setPixelColor(i, 0, 0, 0);
  }
  led.show();
  led2.show();
  led3.show();
}

// ==================== DETENER MELODÍA ==================== //
void stopMelodia() {
  intFlag = false;
  noTone(BUZZER);
  apagarTodosLosLEDs();
  Serial.println("Melodía interrumpida y LEDs apagados");
}

bool playNote(int nota, int duracion) {
  tone(BUZZER, nota);
  int t_transcurrido = 0;
  int t_interval = 50;
  while (t_transcurrido < duracion) {
    if (intFlag) {
      stopMelodia();
      return false;
    }
    delay(t_interval);
    t_transcurrido += t_interval;
  }
  noTone(BUZZER);
  return true;
}

// ==================== CONTROL DE LEDS ==================== //
void encenderLED(int pos) {
  led.setPixelColor(pos, led.Color(255, 255, 255));
  led.show();
}

void encenderLED2(int pos) {
  led2.setPixelColor(pos, led.Color(255, 255, 255));
  led2.show();
}

void encenderLED3(int pos) {
  led3.setPixelColor(pos, led.Color(255, 255, 255));
  led3.show();
}

// ==================== MELODÍA ==================== //
void melodia_GOT() {
  Serial.println("Melodía GOT iniciada");

  for (int i = 0; i < 3; i++) {
    if (!playNote(294, 500)) return;
    if (!playNote(196, 500)) return;
    if (!playNote(233, 250)) return;
    if (!playNote(262, 250)) return;
  }

  if (!playNote(294, 1500)) return;
  if (!playNote(349, 1500)) return;
  if (!playNote(233, 1000)) return;
  if (!playNote(311, 250)) return;
  if (!playNote(294, 250)) return;
  if (!playNote(349, 1000)) return;
  if (!playNote(233, 1000)) return;
  if (!playNote(311, 250)) return;
  if (!playNote(294, 250)) return;
  if (!playNote(262, 500)) return;

  for (int i = 0; i < 3; i++) {
    if (!playNote(208, 250)) return;
    if (!playNote(233, 250)) return;
    if (!playNote(262, 500)) return;
    if (!playNote(175, 500)) return;
  }

  if (!playNote(392, 1000)) return;
  if (!playNote(262, 1000)) return;
  if (!playNote(311, 250)) return;
  if (!playNote(349, 250)) return;

  if (!playNote(392, 1000)) return;
  if (!playNote(262, 1000)) return;
  if (!playNote(311, 250)) return;
  if (!playNote(349, 250)) return;

  if (!playNote(294, 500)) return;

  for (int i = 0; i < 4; i++) {
    if (!playNote(196, 500)) return;
    if (!playNote(233, 250)) return;
    if (!playNote(262, 250)) return;
    if (!playNote(294, 500)) return;
  }

  noTone(BUZZER);  // Asegura que se apaga el buzzer al final
  Serial.println("Melodía GOT finalizada");
}

// ==================== CHEQUEO DE HORA ==================== //
void chequeo() {
  struct tm tiempo;

  if (usarRTC) {
    DateTime now = rtc.now();
    tiempo.tm_hour = now.hour();
    tiempo.tm_min = now.minute();
    tiempo.tm_wday = now.dayOfTheWeek();
  } else {
    if (!getLocalTime(&tiempo)) {
      Serial.println("Error al obtener la hora.");
      delay(1000);
      return;
    }
  }

  // Evento 1
  if (tiempo.tm_hour == 9 && tiempo.tm_min == 0) {
    int pos = (tiempo.tm_wday == 0) ? 6 : (tiempo.tm_wday - 1);
    encenderLED(pos);
    melodia_GOT();
  }

  // Evento 2
  if (tiempo.tm_hour == 14 && tiempo.tm_min == 0) {
    int pos = (tiempo.tm_wday == 0) ? 6 : (tiempo.tm_wday - 1);
    encenderLED2(pos);
    melodia_GOT();
    Serial.print("Tira 2");
  }

  // Evento 3
  if (tiempo.tm_hour == 21 && tiempo.tm_min == 0) {
    int pos = (tiempo.tm_wday == 0) ? 6 : (tiempo.tm_wday - 1);
    encenderLED3(pos);
    melodia_GOT();
    
  }
}

// ==================== TICKER ==================== //
TickTwo chequeohora(chequeo, 60000, 0, MILLIS);  // Cada 60 seg

// ==================== SETUP ==================== //
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), ButtonInterrupt, FALLING);

  led.begin();
  led2.begin();
  led3.begin();
  led.show();
  led2.show();
  led3.show();
  apagarTodosLosLEDs();

  Wire.begin(21, 22);  // Pines I2C del ESP32

  if (!rtc.begin()) {
    Serial.println("No se encontró el RTC");
  }

  // ==================== Conexión WiFi ==================== //
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");

    // Configuración de zona horaria España
    configTime(0, 0, "pool.ntp.org");
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();

    struct tm tiempo;
    while (!getLocalTime(&tiempo)) {
      Serial.println("Sincronizando la hora...");
      delay(1000);
    }

    // Guardar hora en RTC
    rtc.adjust(DateTime(tiempo.tm_year + 1900, tiempo.tm_mon + 1, tiempo.tm_mday, tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec));
    Serial.println("Hora actual guardada en RTC");
    usarRTC = false;

  } else {
    Serial.println("\nNo se pudo conectar a WiFi. Usando hora del RTC.");
    usarRTC = true;
    DateTime now = rtc.now();
    Serial.printf("Hora actual: %02d:%02d:%02d\n", now.hour(), now.minute(), now.second());
    delay(1000);
  }

  // Iniciar el chequeo de hora
  chequeohora.start();
}

// ==================== LOOP ==================== //
void loop() {
  if (intFlag) {
    intFlag = false;
    stopMelodia();
  }

  chequeohora.update();
}