#include <WiFi.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>
#include "Ticker.h"

// ==================== DEFINICIONES ==================== //
#define LED_PIN 19
#define NUM_LEDS 21
#define BUZZER 13
#define BUTTON 26

Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi Configuración
const char* ssid = "MediaLab guest";
const char* password = "medialab2019";

// Estado del botón/interrupción
volatile bool intFlag = false;

// ==================== FUNCIONES ==================== //

// Interrupción del botón
void ButtonInterrupt() {
  intFlag = true;
}

// Comprobador de interrupción para la melodía
bool checkStopMelodia() {
  if(intFlag) {
    stopMelodia();
    return true;
  }
  return false;
}

// Toca una nota, revisando cada cierto tiempo si hay interrupción
bool playNote(int nota, int duracion) {
  tone(BUZZER, nota);

  int t_transcurrido = 0;
  int t_interval = 50;  // Intervalo de comprobación de interrupción (ms)

  while (t_transcurrido < duracion) {
    if (intFlag) {
      stopMelodia();
      return false;
    }
    delay(50);
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

void apagarTodosLosLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    led.setPixelColor(i, 0, 0, 0);
  }
  led.show();
}

// ==================== DETENER MELODÍA ==================== //
void stopMelodia() {
  intFlag = false;
  noTone(BUZZER);
  apagarTodosLosLEDs();
  Serial.println("Melodía interrumpida y LEDs apagados");
}

// ==================== MELODÍA ==================== //
void melodia_GOT() {
  Serial.println("Melodía GOT iniciada");

  // Primera parte
  for (int i = 0; i < 4; i++) {
    if (!playNote(392, 500)) return;
    if (!playNote(262, 500)) return;
    if (!playNote(311, 250)) return;
    if (!playNote(349, 250)) return;
  }

  // Segunda parte
  for (int i = 0; i < 4; i++) {
    if (!playNote(392, 500)) return;
    if (!playNote(262, 500)) return;
    if (!playNote(330, 250)) return;
    if (!playNote(349, 250)) return;
  }

  if (!playNote(392, 1500)) return;
  if (!playNote(262, 1500)) return;
  if (!playNote(330, 250)) return;
  if (!playNote(349, 250)) return;
  if (!playNote(392, 1000)) return;
  if (!playNote(262, 1000)) return;
  if (!playNote(311, 250)) return;
  if (!playNote(349, 250)) return;

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
  if (!getLocalTime(&tiempo)) {
    Serial.println("Error al obtener la hora.");
    delay(1000);
    return;
  }

  // Evento 1 - 21:00
  if (tiempo.tm_hour == 21 && tiempo.tm_min == 0) {
    int pos = (tiempo.tm_wday == 0) ? 14 : 14 + (7 - tiempo.tm_wday);
    encenderLED(pos);
    melodia_GOT();
  }

  // Evento 2 - 14:00
  if (tiempo.tm_hour == 14 && tiempo.tm_min == 0) {
    int pos = (tiempo.tm_wday == 0) ? 13 : 6 + tiempo.tm_wday;
    encenderLED(pos);
    melodia_GOT();
  }

  // Evento 3 - 9:00
  if (tiempo.tm_hour == 9 && tiempo.tm_min == 0) {
    int pos = (tiempo.tm_wday == 0) ? 0 : 7 - tiempo.tm_wday;
    encenderLED(pos);
    melodia_GOT();
  }
}

// ==================== TICKER ==================== //
Ticker chequeohora(chequeo, 60000, 0, MILLIS);  // Cada 60 seg, sin límite de repeticiones

// ==================== SETUP ==================== //
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), ButtonInterrupt, FALLING);

  led.begin();
  led.show();
  apagarTodosLosLEDs();

  // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  // Configuración de NTP para España peninsular
  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  struct tm tiempo;
  while (!getLocalTime(&tiempo)) {
    Serial.println("Sincronizando la hora...");
    delay(1000);
  }
  Serial.println("Hora sincronizada correctamente.");

  // Iniciar el chequeo de hora
  chequeohora.start();
}

// ==================== LOOP ==================== //
void loop() {
  if (intFlag) {
    intFlag = false;
    Serial.println("Botón pulsado: Apagando LEDs y melodía");
    stopMelodia();
  }

  chequeohora.update();  // Llama a la función chequeo() cada minuto
}

