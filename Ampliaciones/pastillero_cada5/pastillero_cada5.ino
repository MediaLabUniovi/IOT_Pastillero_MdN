#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49 
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include "RTClib.h"
#include <driver/ledc.h>

#define PIN_NEO_PIXEL 19
#define NUM_PIXELS 21
#define PIN_BUTTON 26
#define PIN_BUZZER 13

#define DELAY_INTERVAL 250
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  5

RTC_DS3231 rtc;
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

int ordenLED[21] = { 0, 13, 14, 1, 12, 15, 2, 11, 16, 3, 10, 17, 4, 9, 18, 5, 8, 19, 6, 7, 20 };
int indexPastilla = 0;
int pastillaTomada = 1;
bool alarmaActivada = false;
int tempo = 114;
 
int melody[] = {
  NOTE_FS4,8, REST,8, NOTE_A4,8, NOTE_CS5,8, REST,8, NOTE_A4,8, REST,8, NOTE_FS4,8,
  NOTE_D4,8, NOTE_D4,8, NOTE_D4,8, REST,8, REST,4, REST,8, NOTE_CS4,8,
  NOTE_D4,8, NOTE_FS4,8, NOTE_A4,8, NOTE_CS5,8, REST,8, NOTE_A4,8, REST,8, NOTE_F4,8,
  NOTE_E5,-4, NOTE_DS5,8, NOTE_D5,8, REST,8, REST,4,
  NOTE_GS4,8, REST,8, NOTE_CS5,8, NOTE_FS4,8, REST,8, NOTE_CS5,8, REST,8, NOTE_GS4,8,
  REST,8, NOTE_CS5,8, NOTE_G4,8, NOTE_FS4,8, REST,8, NOTE_E4,8, REST,8,
  NOTE_E4,8, NOTE_E4,8, NOTE_E4,8, REST,8, REST,4, NOTE_E4,8, NOTE_E4,8,
  NOTE_E4,8, REST,8, REST,4, NOTE_DS4,8, NOTE_D4,8,
  NOTE_CS4,8, REST,8, NOTE_A4,8, NOTE_CS5,8, REST,8, NOTE_A4,8, REST,8, NOTE_FS4,8,
  NOTE_D4,8, NOTE_D4,8, NOTE_D4,8, REST,8, NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, REST,8,
  REST,8, NOTE_FS4,8, NOTE_A4,8, NOTE_CS5,8, REST,8, NOTE_A4,8, REST,8, NOTE_F4,8,
  NOTE_E5,2, NOTE_D5,8, REST,8, REST,4,
  NOTE_B4,8, NOTE_G4,8, NOTE_D4,8, NOTE_CS4,4, NOTE_B4,8, NOTE_G4,8, NOTE_CS4,8,
  NOTE_A4,8, NOTE_FS4,8, NOTE_C4,8, NOTE_B3,4, NOTE_F4,8, NOTE_D4,8, NOTE_B3,8,
  NOTE_E4,8, NOTE_E4,8, NOTE_E4,8, REST,4, REST,4, NOTE_AS4,4,
  NOTE_CS5,8, NOTE_D5,8, NOTE_FS5,8, NOTE_A5,8, REST,8, REST,4,
  REST,2, NOTE_A3,4, NOTE_AS3,4,
  NOTE_A3,-4, NOTE_A3,8, NOTE_A3,2,
  REST,4, NOTE_A3,8, NOTE_AS3,8, NOTE_A3,8, NOTE_F4,4, NOTE_C4,8,
  NOTE_A3,-4, NOTE_A3,8, NOTE_A3,2,
  REST,2, NOTE_B3,4, NOTE_C4,4,
  NOTE_CS4,-4, NOTE_C4,8, NOTE_CS4,2,
  REST,4, NOTE_CS4,8, NOTE_C4,8, NOTE_CS4,8, NOTE_GS4,4, NOTE_DS4,8,
  NOTE_CS4,-4, NOTE_DS4,8, NOTE_B3,1,
  NOTE_E4,4, NOTE_E4,4, NOTE_E4,4, REST,8,
  NOTE_FS4,8, REST,8, NOTE_A4,8, NOTE_CS5,8, REST,8, NOTE_A4,8, REST,8, NOTE_FS4,8
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
unsigned long previousNoteTime = 0;
int currentNote = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  NeoPixel.begin();

  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  DateTime now = rtc.now();
  setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year() - 2000);

  Alarm.timerRepeat(2*60, Repeats);           // timer for every 5 mins

  NeoPixel.clear();
  NeoPixel.show();

  ledc_timer_config_t ledc_timer;
  ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
  ledc_timer.freq_hz = 5000;
  ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledc_timer.timer_num = LEDC_TIMER_0;
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel;
  ledc_channel.channel = LEDC_CHANNEL_0;
  ledc_channel.duty = 0;
  ledc_channel.gpio_num = PIN_BUZZER;
  ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledc_channel.hpoint = 0;
  ledc_channel.timer_sel = LEDC_TIMER_0;
  ledc_channel_config(&ledc_channel);
}

void loop() {
  Alarm.delay(100);

  int buttonState = digitalRead(PIN_BUTTON);
  if (buttonState == 0) {
    NeoPixel.setPixelColor(ordenLED[indexPastilla - 1], NeoPixel.Color(0, 0, 0));
    NeoPixel.show();
    noTone(PIN_BUZZER);
    Serial.println("Se ha pulsado el botón (Se tomó la pastilla)");
    pastillaTomada = 1;
    alarmaActivada = false;
  }

  if (alarmaActivada) {
    playMelody();
  }
}

void checkAlarm() {}


void Repeats() {
  takePill();
}


void takePill() {
  alarmaActivada = true;
  pastillaTomada = 0;

  Serial.print(indexPastilla);
  Serial.print("      led:");
  Serial.println(ordenLED[indexPastilla]);
  NeoPixel.setPixelColor(ordenLED[indexPastilla], NeoPixel.Color(0, 200, 0));
  NeoPixel.show();
  Serial.print(minute());
  Serial.print("  mins         ");
  Serial.print(second());
  Serial.println(" sec");


  indexPastilla++;

  if (pastillaTomada == 0) {
    Alarm.timerRepeat(7*60, playMelody);
    Alarm.timerOnce(2.5*60, noTomo);
  }
}

void noTomo() {
  int buttonState = digitalRead(PIN_BUTTON);
  if (pastillaTomada == 0 && alarmaActivada) {
    Serial.println("No tomo pastilla");
    NeoPixel.setPixelColor(ordenLED[indexPastilla - 1], NeoPixel.Color(150, 40, 0));
    NeoPixel.show();
    Alarm.timerOnce(4*60+50, demasiadoTarde);// meter +59?
  }
}

void demasiadoTarde(void) {
  if (pastillaTomada == 0) {
   Serial.println("Demasiado tarde");
  NeoPixel.setPixelColor(ordenLED[indexPastilla - 1], NeoPixel.Color(200, 0, 0));
  NeoPixel.show();
  }
}

void playMelody() {
  unsigned long currentMillis = millis();
  if (currentNote < notes) {
    if (currentMillis - previousNoteTime >= noteDuration) {
      divider = melody[currentNote * 2 + 1];
      if (divider > 0) {
        noteDuration = (wholenote) / divider;
      } else {
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5;
      }
      previousNoteTime = currentMillis;

      if (melody[currentNote * 2] == REST) {
        noTone(PIN_BUZZER);
      } else {
        tone(PIN_BUZZER, melody[currentNote * 2], noteDuration * 0.9);
      }
      currentNote++;
    }
  } else {
    noTone(PIN_BUZZER);
    currentNote = 0;
  }
}

