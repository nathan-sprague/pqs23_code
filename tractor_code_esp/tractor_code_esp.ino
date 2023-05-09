#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include "SPIFFS.h"
#include "SD_MMC.h"            // SD Card ESP32


// definitions
#define MAIN_FS SPIFFS
#define GT_PIN 27

// global variables

bool fs_working = false;
unsigned long lastSpeedCalcTime = 0;
unsigned long lastTickTime = 0;

// inputs
float pos[2] = {0, 0}; // joystick position (-100 to +100 in X, Y)
bool gripPressed = false;
unsigned int tickCount = 0;
float shaftRPM = 0;

// outputs
float targetThrottle = 0; // 0-100 (0 is retracted (full throttle), 100 is extended (idle))
float targetSteer = 0; // -100 to +100 (-100 is retracted, 100 is extended)


// function prototypes
void init_CAN();
void read_CAN();
void send_CAN();
void init_fs();
void saveData();
void init_geartooth();
void tick();
void calcSpeeds();


void setup() {
  Serial.begin(115200);
  init_CAN();
  init_geartooth();
}



bool changed = true;


void loop() {
  read_CAN();

  send_CAN();

  calcSpeeds();

  delay(20);


}
