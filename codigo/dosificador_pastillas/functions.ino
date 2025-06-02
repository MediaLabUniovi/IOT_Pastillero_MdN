#include "functions.h"
//----------------------------------------------------
// WiFi y portal cautivo
//----------------------------------------------------
void WiFi_setup(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME);
  Serial.println("Iniciando AP...");
  Serial.println("IP: " + WiFi.softAPIP().toString());
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(300);
  if (dnsServer.start(53, "*", WiFi.softAPIP())){
    Serial.println("Inciado servidor DNS en modo portal cautivo");
  } else {
    Serial.println("Error: No se pudo iniciar el servidor DNS");
  }
  server.begin();
}

void WiFi_ap_desconectar() {
  if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA){
    Serial.println();
    dnsServer.stop();
    Serial.println(); 
    server.stop();
    Serial.println();         // Detiene el servidor DNS
    WiFi.softAPdisconnect(true);
    Serial.println();  // Apaga el Access Point y desconecta
    WiFi.mode(WIFI_OFF);      // Apaga completamente el WiFi
    Serial.println("AP y DNS desactivados.");
    //server.close();
  }
}

void server_config(){
 // Página principal
  // Redirige cualquier URL no registrada al portal
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", html_page);
  });

  server.on("/setConfig", HTTP_GET, []() {
    Preferences localPref;
    localPref.begin("Configuration", false);

    int hm = server.hasArg("hm") ? server.arg("hm").toInt() : -1;
    int mm = server.hasArg("mm") ? server.arg("mm").toInt() : -1;
    int ht = server.hasArg("ht") ? server.arg("ht").toInt() : -1;
    int mt = server.hasArg("mt") ? server.arg("mt").toInt() : -1;
    int hn = server.hasArg("hn") ? server.arg("hn").toInt() : -1;
    int mn = server.hasArg("mn") ? server.arg("mn").toInt() : -1;
    long long ts = server.hasArg("ts") ? strtoll(server.arg("ts").c_str(), nullptr, 10) : 0;

    bool configSaved = false;

    if (hm >= 0 && mm >= 0) {
      hora_morning = hm;
      minuto_morning = mm;
      localPref.putInt("h_m", hm);
      localPref.putInt("m_m", mm);
      configSaved = true;
    } else {
      hora_morning = -1;
      minuto_morning = -1;
    }
    if (ht >= 0 && mt >= 0) {
      hora_tarde = ht;
      minuto_tarde = mt;
      localPref.putInt("h_t", ht);
      localPref.putInt("m_t", mt);
      configSaved = true;
    } else {
      hora_tarde = -1;
      minuto_tarde = -1;
    }
    if (hn >= 0 && mn >= 0) {
      hora_noche = hn;
      minuto_noche = mn;
      localPref.putInt("h_n", hn);
      localPref.putInt("m_n", mn);
      configSaved = true;
    } else {
      hora_noche = -1;
      minuto_noche = -1;
    }

    localPref.putBool("config_saved", configSaved);
    localPref.end();
    // Guarda la hora actual en el RTC como antes...
    server.send(200, "text/plain", "Configuración de horarios recibida correctamente");
  });

  server.on("/confirmacion", HTTP_GET, []() {
    server.send(200, "text/html", html_confirm);
    config = false;
    apagarAP.start();
  });

  server.onNotFound([]() {
    server.send(200, "text/html", html_page);
  });

  
}

//----------------------------------------------------
// Funciones generales
//----------------------------------------------------

// ==================== CONTROL DE LEDS ==================== //
void modoConfig(){
  for (int i = 0; i <= 3; i++){
    encederTodosLosLEDs();
    delay(500);
    apagarTodosLosLEDs();
    delay(500);
  }
}

void configurado(){
  const int delayTiempo = 100; // milisegundos
  const int ciclos = 2; // cuántas veces se repite la secuencia

  for (int c = 0; c < ciclos; c++) {
    // Encendido progresivo
    for (int i = 0; i < NUM_LEDS; i++) {
      encenderLED(i);
      encenderLED2(i);
      encenderLED3(i);
      delay(delayTiempo);
    }

    // Apagado progresivo
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      led.setPixelColor(i, 0, 0, 0);
      led2.setPixelColor(i, 0, 0, 0);
      led3.setPixelColor(i, 0, 0, 0);
      led.show();
      led2.show();
      led3.show();
      delay(delayTiempo);
    }
  }
}

void configurado2(){
  const int totalLeds = 7;
  const int delayPaso = 150; // ms entre pasos
  const int ciclos = 5;
  for (int c = 0; c < ciclos; c++) {
  // Fase 1: Encendido progresivo en rojo
  for (int i = 0; i < totalLeds; i++) {
    led.setPixelColor(i, led.Color(255, 0, 0));
    led2.setPixelColor(i, led.Color(255, 0, 0));
    led3.setPixelColor(i, led.Color(255, 0, 0));
    led.show(); led2.show(); led3.show();
    delay(delayPaso);
  }

  // Fase 2: Alternancia de azul y verde
  for (int i = 0; i < totalLeds; i++) {
    uint32_t colorA = (i % 2 == 0) ? led.Color(0, 0, 255) : led.Color(0, 255, 0);
    led.setPixelColor(i, colorA);
    led2.setPixelColor(i, colorA);
    led3.setPixelColor(i, colorA);
  }
  led.show(); led2.show(); led3.show();
  delay(1000);

  // Fase 3: Arcoíris en barrido
  for (int j = 0; j < 256; j += 36) {  // 0 a 255 con paso 36 (7 pasos aprox)
    for (int i = 0; i < totalLeds; i++) {
      uint8_t r = sin((i * 36 + j) * 3.14 / 128.0) * 127 + 128;
      uint8_t g = sin((i * 36 + j + 85) * 3.14 / 128.0) * 127 + 128;
      uint8_t b = sin((i * 36 + j + 170) * 3.14 / 128.0) * 127 + 128;

      led.setPixelColor(i, led.Color(r, g, b));
      led2.setPixelColor(i, led.Color(r, g, b));
      led3.setPixelColor(i, led.Color(r, g, b));
    }
    led.show(); led2.show(); led3.show();
    delay(300);
  }

  // Fase 4: Apagado en reversa
  for (int i = totalLeds - 1; i >= 0; i--) {
    led.setPixelColor(i, 0, 0, 0);
    led2.setPixelColor(i, 0, 0, 0);
    led3.setPixelColor(i, 0, 0, 0);
    led.show(); led2.show(); led3.show();
    delay(delayPaso);
  }
  }
}

void animacionLoca() {
  const int totalLeds = 7;
  const int totalTiempo = 60000; // 1 minuto
  unsigned long start = millis();

  while (millis() - start < totalTiempo) {
    // Fase 1: Barrido rojo hacia adelante
    for (int i = 0; i < totalLeds; i++) {
      led.setPixelColor(i, led.Color(255, 0, 0));
      led2.setPixelColor(i, led.Color(255, 0, 0));
      led3.setPixelColor(i, led.Color(255, 0, 0));
      led.show(); led2.show(); led3.show();
      delay(100);
    }

    // Fase 2: Alternancia azul-verde intermitente
    for (int rep = 0; rep < 5; rep++) {
      for (int i = 0; i < totalLeds; i++) {
        uint32_t color = (i + rep) % 2 == 0 ? led.Color(0, 0, 255) : led.Color(0, 255, 0);
        led.setPixelColor(i, color);
        led2.setPixelColor(i, color);
        led3.setPixelColor(i, color);
      }
      led.show(); led2.show(); led3.show();
      delay(200);
    }

    // Fase 3: Arcoíris dinámico
    for (int shift = 0; shift < 256; shift += 10) {
      for (int i = 0; i < totalLeds; i++) {
        uint8_t r = sin((i * 20 + shift) * 3.14 / 128.0) * 127 + 128;
        uint8_t g = sin((i * 20 + shift + 85) * 3.14 / 128.0) * 127 + 128;
        uint8_t b = sin((i * 20 + shift + 170) * 3.14 / 128.0) * 127 + 128;
        uint32_t color = led.Color(r, g, b);
        led.setPixelColor(i, color);
        led2.setPixelColor(i, color);
        led3.setPixelColor(i, color);
      }
      led.show(); led2.show(); led3.show();
      delay(100);
    }

    // Fase 4: Efecto teatro (encendido 1 de cada 3 LEDs)
    for (int shift = 0; shift < 3; shift++) {
      for (int i = 0; i < totalLeds; i++) {
        uint32_t color = (i + shift) % 3 == 0 ? led.Color(255, 255, 0) : led.Color(0, 0, 0);
        led.setPixelColor(i, color);
        led2.setPixelColor(i, color);
        led3.setPixelColor(i, color);
      }
      led.show(); led2.show(); led3.show();
      delay(200);
    }

    // Fase 5: Rebote LED en blanco
    for (int i = 0; i < totalLeds; i++) {
      for (int j = 0; j < totalLeds; j++) {
        uint32_t color = (j == i) ? led.Color(255, 255, 255) : led.Color(0, 0, 0);
        led.setPixelColor(j, color);
        led2.setPixelColor(j, color);
        led3.setPixelColor(j, color);
      }
      led.show(); led2.show(); led3.show();
      delay(100);
    }
    for (int i = totalLeds - 1; i >= 0; i--) {
      for (int j = 0; j < totalLeds; j++) {
        uint32_t color = (j == i) ? led.Color(255, 255, 255) : led.Color(0, 0, 0);
        led.setPixelColor(j, color);
        led2.setPixelColor(j, color);
        led3.setPixelColor(j, color);
      }
      led.show(); led2.show(); led3.show();
      delay(100);
    }

    // Fase 6: Flash blanco
    for (int rep = 0; rep < 4; rep++) {
      for (int i = 0; i < totalLeds; i++) {
        led.setPixelColor(i, led.Color(255, 255, 255));
        led2.setPixelColor(i, led.Color(255, 255, 255));
        led3.setPixelColor(i, led.Color(255, 255, 255));
      }
      led.show(); led2.show(); led3.show();
      delay(150);

      for (int i = 0; i < totalLeds; i++) {
        led.setPixelColor(i, 0, 0, 0);
        led2.setPixelColor(i, 0, 0, 0);
        led3.setPixelColor(i, 0, 0, 0);
      }
      led.show(); led2.show(); led3.show();
      delay(150);
    }

    // Fase 7: Desvanecimiento lento
    for (int brillo = 255; brillo >= 0; brillo -= 15) {
      for (int i = 0; i < totalLeds; i++) {
        led.setPixelColor(i, led.Color(brillo, 0, brillo));
        led2.setPixelColor(i, led.Color(brillo, 0, brillo));
        led3.setPixelColor(i, led.Color(brillo, 0, brillo));
      }
      led.show(); led2.show(); led3.show();
      delay(100);
    }
  }

  // Apagado final
  for (int i = 0; i < totalLeds; i++) {
    led.setPixelColor(i, 0, 0, 0);
    led2.setPixelColor(i, 0, 0, 0);
    led3.setPixelColor(i, 0, 0, 0);
  }
  led.show(); led2.show(); led3.show();
}

void animacionUltraLoca() {
  const int totalLeds = 7;
  const unsigned long duracion = 60000; // 1 minuto
  unsigned long inicio = millis();

  while (millis() - inicio < duracion) {
    // Fase 1: Cada LED cambia a un color aleatorio cada 150 ms
    for (int rep = 0; rep < 10; rep++) {
      for (int i = 0; i < totalLeds; i++) {
        led.setPixelColor(i, led.Color(random(256), random(256), random(256)));
        led2.setPixelColor(i, led.Color(random(256), random(256), random(256)));
        led3.setPixelColor(i, led.Color(random(256), random(256), random(256)));
      }
      led.show(); led2.show(); led3.show();
      delay(150);
    }

    // Fase 2: Onda de color desfasada por tira
    for (int paso = 0; paso < 20; paso++) {
      for (int i = 0; i < totalLeds; i++) {
        int r1 = (sin((i + paso) * 0.5) + 1) * 127;
        int g1 = (sin((i + paso + 2) * 0.6) + 1) * 127;
        int b1 = (sin((i + paso + 4) * 0.7) + 1) * 127;

        int r2 = (cos((i + paso) * 0.7) + 1) * 127;
        int g2 = (cos((i + paso + 3) * 0.4) + 1) * 127;
        int b2 = (cos((i + paso + 5) * 0.5) + 1) * 127;

        int r3 = (sin((i + paso + 1) * 0.9) + 1) * 127;
        int g3 = (cos((i + paso + 1) * 0.3) + 1) * 127;
        int b3 = (sin((i + paso + 1) * 0.8) + 1) * 127;

        led.setPixelColor(i, led.Color(r1, g1, b1));
        led2.setPixelColor(i, led.Color(r2, g2, b2));
        led3.setPixelColor(i, led.Color(r3, g3, b3));
      }
      led.show(); led2.show(); led3.show();
      delay(100);
    }

    // Fase 3: Parpadeo asincrónico independiente por LED
    for (int ciclo = 0; ciclo < 15; ciclo++) {
      for (int i = 0; i < totalLeds; i++) {
        if (random(2)) led.setPixelColor(i, led.Color(random(256), 0, random(256)));
        else led.setPixelColor(i, 0);
        if (random(2)) led2.setPixelColor(i, led.Color(0, random(256), random(256)));
        else led2.setPixelColor(i, 0);
        if (random(2)) led3.setPixelColor(i, led.Color(random(256), random(256), 0));
        else led3.setPixelColor(i, 0);
      }
      led.show(); led2.show(); led3.show();
      delay(120);
    }

    // Fase 4: LEDs giran en color tipo ruleta
    for (int shift = 0; shift < 15; shift++) {
      for (int i = 0; i < totalLeds; i++) {
        int hue = (i * 36 + shift * 20) % 360;
        int r, g, b;
        float s = 1.0, v = 1.0;
        float C = v * s;
        float X = C * (1 - abs((hue / 60.0f) - (int)(hue / 60) * 2 - 1));
        float m = v - C;

        switch ((hue / 60) % 6) {
          case 0: r = C * 255; g = X * 255; b = 0; break;
          case 1: r = X * 255; g = C * 255; b = 0; break;
          case 2: r = 0; g = C * 255; b = X * 255; break;
          case 3: r = 0; g = X * 255; b = C * 255; break;
          case 4: r = X * 255; g = 0; b = C * 255; break;
          case 5: r = C * 255; g = 0; b = X * 255; break;
        }

        led.setPixelColor(i, led.Color(r, g, b));
        led2.setPixelColor(i, led.Color(b, r, g));
        led3.setPixelColor(i, led.Color(g, b, r));
      }
      led.show(); led2.show(); led3.show();
      delay(100);
    }

    // Fase 5: Fade aleatorio (cada LED cambia suavemente a otro color)
    for (int paso = 0; paso < 25; paso++) {
      for (int i = 0; i < totalLeds; i++) {
        uint8_t r = random(256), g = random(256), b = random(256);
        led.setPixelColor(i, led.Color(r, g, b));
        led2.setPixelColor(i, led.Color(255 - r, g, 255 - b));
        led3.setPixelColor(i, led.Color(b, 255 - g, r));
      }
      led.show(); led2.show(); led3.show();
      delay(250);
    }
  }

  // Apagado final
  for (int i = 0; i < totalLeds; i++) {
    led.setPixelColor(i, 0, 0, 0);
    led2.setPixelColor(i, 0, 0, 0);
    led3.setPixelColor(i, 0, 0, 0);
  }
  led.show(); led2.show(); led3.show();
}

void encederTodosLosLEDs(){
  for (int i = 0; i < NUM_LEDS; i++) {
    led.setPixelColor(i, 255, 0, 0);
    led2.setPixelColor(i, 0, 255, 0);
    led3.setPixelColor(i, 0, 0, 255);
  }
  led.show();
  led2.show();
  led3.show();
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
void stopMelodia() {
  intFlag = false;
  noTone(BUZZER); // Detiene el PWM en ese pin.
  apagarTodosLosLEDs();
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

void melodia_GOT() {
  Serial.println("Melodía GOT iniciada");
  intFlag = false;
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
void chequeo() { // Esto es lo que voy a tener que cambiar
  struct tm tiempo;

    DateTime now = rtc.now();
    tiempo.tm_hour = now.hour();
    tiempo.tm_min = now.minute();
    tiempo.tm_wday = now.dayOfTheWeek();

  // Evento 1
  if (tiempo.tm_hour == hora_morning  && tiempo.tm_min == minuto_morning) {
    int pos = (tiempo.tm_wday == 0) ? 6 : (tiempo.tm_wday - 1);
    encenderLED(pos);
    melodia_GOT();
  }

  // Evento 2
  if (tiempo.tm_hour == hora_tarde && tiempo.tm_min == minuto_tarde) {
    int pos = (tiempo.tm_wday == 0) ? 6 : (tiempo.tm_wday - 1);
    encenderLED2(pos);
    melodia_GOT();
  }

  // Evento 3
  if (tiempo.tm_hour == hora_noche && tiempo.tm_min == minuto_noche) {
    int pos = (tiempo.tm_wday == 0) ? 6 : (tiempo.tm_wday - 1);
    encenderLED3(pos);
    melodia_GOT();
    
  }
}

void peripherals_setup(){
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

  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  if (!rtc.begin()) {
    Serial.println("No se encontró el RTC");
    while(1);
  }
}