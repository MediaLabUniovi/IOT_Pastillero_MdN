#include "configuration.h"

void IRAM_ATTR ButtonInterrupt() {
  t_pulsado = millis();  // Guardamos el tiempo en que se pulsó
  intFlag = true;
}

void setup() {
  Serial.begin(115200);
  peripherals_setup(); // RTC, botón, Buzzer y LEDs
  server_config();
  pref.begin("Configuration",true); 
  bool configSaved = pref.getBool("config_saved", false);
  if (configSaved){
    hora_morning    = pref.getInt("h_m", 0);
    minuto_morning  = pref.getInt("m_m", 0);
    hora_tarde      = pref.getInt("h_t", 0);
    minuto_tarde    = pref.getInt("m_t", 0);
    hora_noche      = pref.getInt("h_n", 0);
    minuto_noche    = pref.getInt("m_n", 0);
    Serial.println("Hora guardada");
    Serial.println(hora_morning);
  } else {
    WiFi_setup();
    config = true;
  }
  chequeohora.start();  // Iniciar el chequeo de hora
  pref.end();
}

void loop() {
  // Botón soltado
  if ((millis() - t_pulsado > 3000) && (digitalRead(BUTTON) == LOW)){
    intFlag = true;
    modoConfig();
    WiFi_setup(); // Se vuelve a activar la WiFi
    config = true;  // Activamos modo config
    timeoutCP.start();
  }
  if (config){
    dnsServer.processNextRequest();
    server.handleClient();
  }
  chequeohora.update();
  apagarAP.update();
  timeoutCP.update();
}