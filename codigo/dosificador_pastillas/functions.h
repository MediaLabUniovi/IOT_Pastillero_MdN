#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "configuration.h"

void WiFi_setup();
void melodia_GOT();
void chequeo();
void comparar_horas();
void encenderLED(int pos);
void encenderLED2(int pos);
void encenderLED3(int pos);
bool playNote(int nota, int duracion);
void stopMelodia();
void apagarTodosLosLEDs();
void peripherals_setup();
void WiFi_ap_desconectar();
void encederTodosLosLEDs();
void modoConfig();
void timeout_AP();
String formatHora(int h, int m);
String generarConfirmacionHtml(int hm, int mm, int ht, int mt, int hn, int mn);
void dormir_tiempo();
void entrar_en_suspension(unsigned long tiempo_en_segundos);

#endif