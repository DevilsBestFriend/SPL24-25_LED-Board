#include <FastLED.h>
#include <LEDMatrix.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// LED Matrix Konfiguration
#define NUM_LEDS_PER_STRIP 256
#define DATA_PIN_UPPER 25
#define DATA_PIN_LOWER 26
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

// Joystick Pins
#define JOYSTICK_BUTTON_PIN 32
#define JOYSTICK_X_PIN 34
#define JOYSTICK_Y_PIN 35

const byte font5x7[][5] = {
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
  {0x7F, 0x20, 0x10, 0x20, 0x7F}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
  {0x00, 0x00, 0x00, 0x00, 0x00}, // SPACE
};
// DHT-Sensor
#define SENSOR_PIN 33
#define SENSOR_TYP DHT22

// WLAN-Zugangsdaten
const char* wlanName = "iPhone von David";
const char* wlanPasswort = "Fortnite4life";

// Webhook-URL
const char* webAppUrl = "https://script.google.com/macros/s/AKfycbw7HoXQEsoIS8YQ29FizEQhf903ahI2iOmNgdAKw1pcOHSjS4KIFGk_WvB23RArKvmC/exec";

// Richtung
enum Direction { UP, RIGHT, DOWN, LEFT };

// LED Arrays
CRGB leds_upper[NUM_LEDS_PER_STRIP];
CRGB leds_lower[NUM_LEDS_PER_STRIP];
CRGB leds_array[NUM_LEDS_PER_STRIP * 2];
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// Temperaturklasse
class TemperaturSensor {
public:
  TemperaturSensor(uint8_t pin, uint8_t typ) : dht(pin, typ) {}
  void starten() { dht.begin(); }
  float leseTemperatur() { return dht.readTemperature(); }
private:
  DHT dht;
};

// Temperatur-Objekt jetzt nach Klassendefinition
TemperaturSensor temperaturSensor(SENSOR_PIN, SENSOR_TYP);

// Snake-Variablen
int snakeX[100], snakeY[100];
int snakeLength = 3;
Direction snakeDirection = RIGHT;
int foodX, foodY;
unsigned long lastMoveTime = 0;
int gameSpeed = 400;
bool gameOver = false;
int score = 0;

// Task-Auswahl
volatile int currentTask = 0;

// Mapping-Funktion
struct LedAddress { CRGB* array; int index; };

LedAddress mapXY(int x, int y) {
  LedAddress result;
  int led;
  if (y < 8) {
    led = x * 8 + ((x % 2 == 0) ? y : 7 - y);
    result.array = leds_lower;
    result.index = led;
  } else {
    int flippedX = MATRIX_WIDTH - 1 - x;
    int flippedY = 15 - y;
    led = flippedX * 8 + ((flippedX % 2 == 0) ? flippedY : 7 - flippedY);
    result.array = leds_upper;
    result.index = led;
  }
  return result;
}

// Snake-Logik
void generateFood() {
  bool valid;
  do {
    valid = true;
    foodX = random(MATRIX_WIDTH);
    foodY = random(MATRIX_HEIGHT);
    for (int i = 0; i < snakeLength; i++) {
      if (foodX == snakeX[i] && foodY == snakeY[i]) valid = false;
    }
  } while (!valid);
}

void initGame() {
  snakeLength = 3;
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = MATRIX_WIDTH / 2 - i;
    snakeY[i] = MATRIX_HEIGHT / 2;
  }
  snakeDirection = RIGHT;
  score = 0;
  generateFood();
  gameSpeed = 400;
  gameOver = false;
  lastMoveTime = millis();
}

void readJoystick() {
  int x = analogRead(JOYSTICK_X_PIN);
  int y = analogRead(JOYSTICK_Y_PIN);
  const int mid = 2048;
  const int thresh = 1000;

  if (y > mid + thresh && snakeDirection != LEFT) snakeDirection = RIGHT;
  else if (y < mid - thresh && snakeDirection != RIGHT) snakeDirection = LEFT;
  else if (x > mid + thresh && snakeDirection != DOWN) snakeDirection = UP;
  else if (x < mid - thresh && snakeDirection != UP) snakeDirection = DOWN;
}

void moveSnake() {
  if (millis() - lastMoveTime < gameSpeed || gameOver) return;
  lastMoveTime = millis();

  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (snakeDirection) {
    case UP: snakeY[0]--; break;
    case RIGHT: snakeX[0]++; break;
    case DOWN: snakeY[0]++; break;
    case LEFT: snakeX[0]--; break;
  }

  if (snakeX[0] < 0 || snakeX[0] >= MATRIX_WIDTH ||
      snakeY[0] < 0 || snakeY[0] >= MATRIX_HEIGHT) {
    gameOver = true;
    return;
  }

  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      return;
    }
  }

  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    if (snakeLength < 100) snakeLength++;
    score++;
    gameSpeed = max(80, gameSpeed - 10);
    generateFood();
  }
}

void drawGame() {
  FastLED.clear();
  for (int i = 0; i < snakeLength; i++) {
    if (snakeX[i] >= 0 && snakeX[i] < MATRIX_WIDTH &&
        snakeY[i] >= 0 && snakeY[i] < MATRIX_HEIGHT) {
      LedAddress addr = mapXY(snakeX[i], snakeY[i]);
      addr.array[addr.index] = (i == 0) ? CRGB::White :
        CRGB(0, map(i, 1, snakeLength - 1, 200, 50), 0);
    }
  }

  if (!gameOver) {
    LedAddress addr = mapXY(foodX, foodY);
    int brightness = 150 + 100 * sin(millis() / 200.0);
    addr.array[addr.index] = CRGB(brightness, 0, 0);
  }

  FastLED.show();
}

// Tasks
void snakeTask(void* pv) {
  while (1) {
    if (currentTask == 0) {
      readJoystick();
      moveSnake();
      drawGame();
      if (gameOver && millis() - lastMoveTime > 3000) initGame();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void sendeDaten(float temperatur) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(webAppUrl) + "?temperatur=" + temperatur;
    http.begin(url);
    int code = http.GET();
    http.end();
    Serial.printf("HTTP Status: %d\n", code);
  }
}

void drawChar5x7(int x, int y, char c, CRGB color) {
  if (c >= 'A' && c <= 'Z') {
    const byte* bitmap = font5x7[c - 'A'];
    for (int col = 0; col < 5; col++) {
      byte column = bitmap[col];
      for (int row = 0; row < 7; row++) {
        if (column & (1 << row)) {
  int drawX = x + col;
  int drawY = y + (6 - row);

  // Spiegelung horizontal rückgängig machen
  drawX = MATRIX_WIDTH - 1 - drawX;

  if (drawX >= 0 && drawX < MATRIX_WIDTH &&
      drawY >= 0 && drawY < MATRIX_HEIGHT) {
    LedAddress addr = mapXY(drawX, drawY);
    addr.array[addr.index] = color;
  }
}

      }
    }
  }
}


void googleSheetsTask(void* pv) {
  static int scrollOffset = MATRIX_WIDTH;

  while (true) {
    if (currentTask == 1) {
      FastLED.clear();

      const char* text = " GOOGLE SHEETS ";
      int len = strlen(text);

      for (int i = 0; i < len; i++) {
        drawChar5x7(scrollOffset + i * 6, 0, text[i], CRGB::White);  // y=0: untere Hälfte
      }

      FastLED.show();

      scrollOffset--;
      if (scrollOffset < -len * 6) scrollOffset = MATRIX_WIDTH;

      vTaskDelay(30 / portTICK_PERIOD_MS);  // ca. 33 FPS → flüssig & lesbar
    } else {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
}



void joystickTask(void* pv) {
  bool lastState = HIGH;
  while (1) {
    bool currentState = digitalRead(JOYSTICK_BUTTON_PIN);
    if (lastState == HIGH && currentState == LOW) {
      currentTask = (currentTask + 1) % 2; // Snake ↔ Google Sheets
      Serial.print("Wechsle zu Task ");
      Serial.println(currentTask);
      delay(200);  // Entprellung
    }
    lastState = currentState;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void taskWLAN(void* pv) {
  WiFi.begin(wlanName, wlanPasswort);
  Serial.println("WLAN verbinden...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  Serial.println("\nWLAN verbunden!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  vTaskDelete(NULL);
}



void setup() {
  Serial.begin(115200);
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);

  FastLED.addLeds<WS2812B, DATA_PIN_UPPER, GRB>(leds_upper, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_LOWER, GRB>(leds_lower, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(50);

  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds_array[i] = leds_lower[i];
    leds_array[i + NUM_LEDS_PER_STRIP] = leds_upper[i];
  }
  leds.SetLEDArray(leds_array);

  FastLED.clear();
  FastLED.show();
  randomSeed(analogRead(36));

  temperaturSensor.starten();
  initGame();

  xTaskCreatePinnedToCore(snakeTask, "Snake", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(googleSheetsTask, "GoogleSheets", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(joystickTask, "Joystick", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(taskWLAN, "WLAN", 4096, NULL, 1, NULL, 0);
}

void loop() {
  // leer
}
