#include <Arduino.h>
#include "SnakeGame.h"
#include "TextScroller.h"

// Globale Variablen
TaskHandle_t snakeTaskHandle = NULL;
TaskHandle_t textTaskHandle = NULL;
volatile bool gameChangeRequested = false;

// Interrupt-Service-Routine für Joystick-Click
void IRAM_ATTR buttonISR() {
  gameChangeRequested = true;
}

void switchGame() {
  if(snakeTaskHandle != NULL && textTaskHandle != NULL) {
    if(eTaskGetState(snakeTaskHandle) == eRunning) {
      vTaskSuspend(snakeTaskHandle);
      vTaskResume(textTaskHandle);
    } else {
      vTaskSuspend(textTaskHandle);
      vTaskResume(snakeTaskHandle);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Hardware-Initialisierung
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(JOYSTICK_BUTTON_PIN), buttonISR, FALLING);

  // Tasks erstellen (beide zunächst pausiert)
  xTaskCreatePinnedToCore(
    snakeTask,
    "Snake",
    4096,
    NULL,
    2,
    &snakeTaskHandle,
    1
  );
  
  xTaskCreatePinnedToCore(
    textTask,
    "Text",
    4096,
    NULL,
    1,
    &textTaskHandle,
    1
  );

  // Starte mit Snake-Spiel
  vTaskResume(snakeTaskHandle);
  vTaskSuspend(textTaskHandle);
}

void loop() {
  if(gameChangeRequested) {
    switchGame();
    gameChangeRequested = false;
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);
}
