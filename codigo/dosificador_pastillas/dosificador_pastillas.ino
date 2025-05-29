#include "configuration.h"

void IRAM_ATTR ButtonInterrupt() {
  t_pulsado = millis();  // Guardamos el tiempo en que se pulsó
  intFlag = true;
}

void setup() {
  Serial.begin(115200);
  peripherals_setup(); // RTC, botón, Buzzer y LEDs
  WiFi_setup();
  server_config();
  chequeohora.start();  // Iniciar el chequeo de hora
}

void loop() {
  // Botón soltado
  if ((millis() - t_pulsado > 3000) && (digitalRead(BUTTON) == LOW)){
    modoConfig();
    WiFi_setup(); // Se vuelve a activar la WiFi
    config = true;  // Activamos modo config
  }
  if (config){
    dnsServer.processNextRequest();
    server.handleClient();
  }
  chequeohora.update();
  apagarAP.update();
}