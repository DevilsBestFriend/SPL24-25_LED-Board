// FreeRTOS auf ESP32: 3 Tasks geben 'a', 'b' und 'c' in unterschiedlichen Intervallen aus

void taskA(void *pvParameters) {
  // Setup-Teil für Task A
  Serial.println("Task A gestartet");

  // Loop-Teil für Task A
  while (1) {
    Serial.println("a");
    vTaskDelay(2000 / portTICK_PERIOD_MS); // 2 Sekunden warten
  }
}

void taskB(void *pvParameters) {
  // Setup-Teil für Task B
  Serial.println("Task B gestartet");

  // Loop-Teil für Task B
  while (1) {
    Serial.println("b");
    vTaskDelay(3000 / portTICK_PERIOD_MS); // 3 Sekunden warten
  }
}

void taskC(void *pvParameters) {
  // Setup-Teil für Task C
  Serial.println("Task C gestartet");

  // Loop-Teil für Task C
  while (1) {
    Serial.println("c");
    vTaskDelay(5000 / portTICK_PERIOD_MS); // 5 Sekunden warten
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Kurze Wartezeit zur Stabilisierung der seriellen Ausgabe

  // Tasks starten
  xTaskCreate(taskA, "Task A", 1000, NULL, 1, NULL);
  xTaskCreate(taskB, "Task B", 1000, NULL, 1, NULL);
  xTaskCreate(taskC, "Task C", 1000, NULL, 1, NULL);
}

void loop() {
  // Der Hauptloop bleibt leer, da FreeRTOS alle Tasks verwaltet
  vTaskDelay(portMAX_DELAY);
}
