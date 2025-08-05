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
  print_wakeup_reason();
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0){ // Esto ocurrirá cuando quiera ponerlo en modo config o al parar la alarma
    t_pulsado = millis();
    while (digitalRead(BUTTON) == LOW){ //cutre pero funciona
      if (millis() - t_pulsado >= 3000){ // Se mantiene el botón pulsado por 3 segundos
        modoConfig();
        WiFi_setup();
        config = true;
        timeoutCP.start();
      }
    } 
    Serial.printf("valor de intFlag: %d\n", intFlag);
    if (!config && !intFlag){ // Solo si no ha entrado en modo configuracion. intflag no debería de ser true porque se habría puesto a false al parar la música.
      Serial.println("Aquí no debería de entrar"); //Pero entra
      unsigned long temporizador = 60000 + millis(); // Temporizador para que no suene la musica todo el minuto y luego se duerma
      while (temporizador > millis()){
        delay(10);
      }
      dormir_tiempo();
    }
  } else{ // Esto ocurrirá cuando se inicie la primera vez o cuando se despierte por timer
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
      chequeo(); // Si hay configuración, miro si me toca sonar y sino me duermo.
      dormir_tiempo();
    } else { // Esto solo debería de hacerlo la primera vez que arranque
      modoConfig();
      WiFi_setup();
      config = true;
      timeoutCP.start();
    }
  } 
  pref.end();
}

void loop() {
  if (config){
    dnsServer.processNextRequest();
    server.handleClient();
  }
  // Chequeo de los timers
  apagarAP.update();
  timeoutCP.update();
}