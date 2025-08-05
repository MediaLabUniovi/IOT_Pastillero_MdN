#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//=======================================================================//
//=======================================================================//
//                                                                       //
//                    Configuración manual de horas                      //
//                          y credenciales WiFi                          //
//                                                                       //
//=======================================================================//
//=======================================================================//

const char* AP_NAME = "Pastillero"; 
const char* PASSWORD = "loripastis";

//=======================================================================//
//=======================================================================//

#include <WiFi.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>
#include <TickTwo.h>
#include <Wire.h>
#include <RTClib.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>

#include "functions.h"
#include "paginaWeb.hpp"

// ==================== DEFINICIONES ==================== //
#define LED_PIN 17
#define LED_PIN2 18
#define LED_PIN3 19
#define NUM_LEDS 7
#define BUZZER 5
#define BUTTON 26

#define uS_TO_S_FACTOR 1000000ULL

Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led2 = Adafruit_NeoPixel(NUM_LEDS, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led3 = Adafruit_NeoPixel(NUM_LEDS, LED_PIN3, NEO_GRB + NEO_KHZ800);

TickTwo chequeo_hora(chequeo, 60000, 0, MILLIS);  // Cada 60 segundos
TickTwo apagarAP(WiFi_ap_desconectar, 5000, 1, MILLIS); // 5 segundos
TickTwo timeoutCP(WiFi_ap_desconectar, 600000 , 1, MILLIS); // 10 mins 600000

RTC_DS3231 rtc;
DNSServer dnsServer;
WebServer server(80);
Preferences pref;

// Estado del botón/interrupción
volatile bool intFlag = false;
volatile bool config = false; // En el restart empezamos en modo configuración
volatile bool botonDetectado = false;
unsigned long t_pulsado = 0;
unsigned long timer = 0;

bool apago_por_config = false;

int hora_morning = 9;
int minuto_morning = 0;

int hora_tarde = 14;
int minuto_tarde = 0;

int hora_noche = 20;
int minuto_noche = 30;

#endif