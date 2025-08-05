#include "configuration.h"

extern "C" {
  #include "esp_sleep.h"
  #include "driver/rtc_io.h"
}

void IRAM_ATTR ButtonInterrupt() {
  intFlag = true;
}

void setup() {
  Serial.begin(115200);
  peripherals_setup(); // RTC, botón, Buzzer y LEDs
  server_config();
  //print_wakeup_reason();
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason != ESP_SLEEP_WAKEUP_EXT0){ // Esto ocurrirá cuando se inicie la primera vez o cuando se despierte por timer
    pref.begin("Configuration",true);
    bool configSaved = pref.getBool("config_saved", false);
    if (configSaved){
      hora_morning    = pref.getInt("h_m", 0);
      minuto_morning  = pref.getInt("m_m", 0);
      hora_tarde      = pref.getInt("h_t", 0);
      minuto_tarde    = pref.getInt("m_t", 0);
      hora_noche      = pref.getInt("h_n", 0);
      minuto_noche    = pref.getInt("m_n", 0);
      Serial.println("Configuración ya guardada comprobando hora");
      Serial.print(hora_morning);
      Serial.print(" ");
      Serial.println(minuto_morning);
      // Aquí puede que necesite un delay
      chequeo(); // Si hay configuración, miro si me toca sonar y sino me duermo.
      dormir_tiempo();
    } else { // Esto solo debería de hacerlo la primera vez que arranque
      modoConfig();
      WiFi_setup();
      config = true;
      timeoutCP.start();
    }
    chequeo_hora.update(); // Este se pone aquí para en caso de ser despertado por botón y que no se haga nada durante los 10 minutos de espera para dormir, pueda sonar la alarma.
    pref.end();
  } 
  t_pulsado = millis();
}

void loop() { // Al loop solo voy a llegar si no me despierto por timer es decir: si es la primera vez que arranco o si quiero entrar en modo config.
  // Botón soltado
  if ((millis() - t_pulsado > 3000) && (digitalRead(BUTTON) == LOW)){
    intFlag = true; // Porque??
    modoConfig();
    WiFi_setup(); // Se vuelve a activar la WiFi
    config = true;  // Activamos modo config
    timeoutCP.start();
  }
  if (digitalRead(BUTTON) == LOW) timer = millis(); // Se actualiza timer cada vez que se pulse.
  if (!config && millis() - timer > 60000) dormir_tiempo(); // Si después de un minuto no se hace nada, vuelve a dormirse
  if (config){
    dnsServer.processNextRequest();
    server.handleClient();
  }
  // Chequeo de los timers
  apagarAP.update();
  timeoutCP.update();
  chequeo_hora.update();
}


