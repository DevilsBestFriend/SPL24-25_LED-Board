#include <Arduino.h>
#include "joystickLib.h"

// Task-Handles
TaskHandle_t TaskAHandle = NULL;
TaskHandle_t TaskBHandle = NULL;
TaskHandle_t TaskCHandle = NULL;
TaskHandle_t MenuTaskHandle = NULL;

// Taster-Objekt
EntprellterTaster taster(32); // Pin 25 für den Taster

// Aktuelle Task-Nummer
volatile int aktuelleTaskNummer = 0;

// Task A: Gibt den Buchstaben "a" im Halbsekunden-Takt aus
void TaskA(void *parameter) {
  delay(10);
  while (1) {
    Serial.println("a");
    vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms warten
  }
}

// Task B: Gibt den Buchstaben "b" im Halbsekunden-Takt aus
void TaskB(void *parameter) {
  delay(10);
  while (1) {
    Serial.println("b");
    vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms warten
  }
}

// Task C: Gibt den Buchstaben "c" im Halbsekunden-Takt aus
void TaskC(void *parameter) {
  delay(10);
  while (1) {
    Serial.println("c");
    vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms warten
  }
}

// Funktion, um zwischen den Tasks zu wechseln
void wechsleTask() {
  switch (aktuelleTaskNummer) {
    case 0:
      vTaskSuspend(TaskAHandle);
      vTaskResume(TaskBHandle);
      aktuelleTaskNummer = 1;
      break;
    case 1:
      vTaskSuspend(TaskBHandle);
      vTaskResume(TaskCHandle);
      aktuelleTaskNummer = 2;
      break;
    case 2:
      vTaskSuspend(TaskCHandle);
      vTaskResume(TaskAHandle);
      aktuelleTaskNummer = 0;
      break;
  }
}

// Menü-Task: Überwacht den langen Tastendruck und wechselt die Tasks
void MenuTask(void *parameter) {
  delay(10);
  while (1) {
    taster.aktualisiere(); // Taster-Status aktualisieren

    if (taster.wurdeLangeGedrueckt()) {
      Serial.println("Langer Tastendruck erkannt. Wechsel zwischen Tasks.");
      wechsleTask();
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // Kurze Verzögerung zur Entlastung des Prozessors
  }
}

void setup() {
  // Serial-Kommunikation starten
  Serial.begin(115200);
  delay(1000); // Kurze Wartezeit für den Serial-Monitor

  // Tasks erstellen
  xTaskCreate(
    TaskA,           // Funktion
    "Task A",        // Name (für Debugging)
    1000,            // Stack-Größe
    NULL,            // Parameter
    1,               // Priorität
    &TaskAHandle     // Task-Handle
  );

  xTaskCreate(
    TaskB,
    "Task B",
    1000,
    NULL,
    1,
    &TaskBHandle
  );

  xTaskCreate(
    TaskC,
    "Task C",
    1000,
    NULL,
    1,
    &TaskCHandle
  );

  xTaskCreate(
    MenuTask,       // Funktion
    "Menu Task",    // Name (für Debugging)
    1000,           // Stack-Größe
    NULL,           // Parameter
    2,              // Priorität (höher als die anderen Tasks)
    &MenuTaskHandle // Task-Handle
  );

  // Nur Task A soll anfangs laufen, die anderen Tasks pausieren
  vTaskSuspend(TaskBHandle);
  vTaskSuspend(TaskCHandle);
}

void loop() {
  // Die loop-Funktion bleibt leer
}
