#include <FastLED.h>
#include <LEDMatrix.h>

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

// Spielkonstanten
#define INITIAL_SNAKE_LENGTH 3
#define MAX_SNAKE_LENGTH 100
#define GAME_SPEED_INITIAL 400
#define GAME_SPEED_MIN 80
#define SPEED_INCREASE_PER_FOOD 10

// Richtung
enum Direction { UP, RIGHT, DOWN, LEFT };

// LED Arrays
CRGB leds_upper[NUM_LEDS_PER_STRIP];
CRGB leds_lower[NUM_LEDS_PER_STRIP];
CRGB leds_array[NUM_LEDS_PER_STRIP * 2];
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// Struktur für LED-Adresse
struct LedAddress {
  CRGB* array;
  int index;
};

// Spielvariablen
int snakeX[MAX_SNAKE_LENGTH];
int snakeY[MAX_SNAKE_LENGTH];
int snakeLength;
Direction snakeDirection;
int foodX, foodY;
unsigned long lastMoveTime;
int gameSpeed;
bool gameOver;
int score = 0;

// Hilfsfunktion: Mappt (x,y) auf LED-Array und Index
LedAddress mapXY(int x, int y) {
  LedAddress result;
  int led;

  if (y < 8) {
    led = x * 8;
    led += (x % 2 == 0) ? y : 7 - y;
    result.array = leds_lower;
    result.index = led;
  } else {
    int flippedX = MATRIX_WIDTH - 1 - x;
    int flippedY = 15 - y;
    led = flippedX * 8;
    led += (flippedX % 2 == 0) ? flippedY : 7 - flippedY;
    result.array = leds_upper;
    result.index = led;
  }
  return result;
}

void generateFood() {
  bool validPosition;
  do {
    validPosition = true;
    foodX = random(MATRIX_WIDTH);
    foodY = random(MATRIX_HEIGHT);
    for (int i = 0; i < snakeLength; i++) {
      if (foodX == snakeX[i] && foodY == snakeY[i]) {
        validPosition = false;
        break;
      }
    }
  } while (!validPosition);
}

void initGame() {
  snakeLength = INITIAL_SNAKE_LENGTH;
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = MATRIX_WIDTH / 2 - i;
    snakeY[i] = MATRIX_HEIGHT / 2;
  }
  snakeDirection = RIGHT;
  score = 0;
  generateFood();
  gameSpeed = GAME_SPEED_INITIAL;
  gameOver = false;
  lastMoveTime = millis();
  Serial.println("New game started");
}

void readJoystick() {
  int xValue = analogRead(JOYSTICK_X_PIN);
  int yValue = analogRead(JOYSTICK_Y_PIN);
  const int midPoint = 2048;
  const int threshold = 1000;
  Direction newDirection = snakeDirection;

  if (yValue > midPoint + threshold && snakeDirection != LEFT) newDirection = RIGHT;
  else if (yValue < midPoint - threshold && snakeDirection != RIGHT) newDirection = LEFT;
  else if (xValue > midPoint + threshold && snakeDirection != DOWN) newDirection = UP;
  else if (xValue < midPoint - threshold && snakeDirection != UP) newDirection = DOWN;

  snakeDirection = newDirection;
}

void moveSnake() {
  if (millis() - lastMoveTime < gameSpeed || gameOver) return;
  lastMoveTime = millis();

  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }

  switch (snakeDirection) {
    case UP:    snakeY[0]--; break;
    case RIGHT: snakeX[0]++; break;
    case DOWN:  snakeY[0]++; break;
    case LEFT:  snakeX[0]--; break;
  }

  if (snakeX[0] < 0 || snakeX[0] >= MATRIX_WIDTH ||
      snakeY[0] < 0 || snakeY[0] >= MATRIX_HEIGHT) {
    gameOver = true;
    Serial.print("Game over! Score: ");
    Serial.println(score);
    return;
  }

  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      Serial.print("Game over! Score: ");
      Serial.println(score);
      return;
    }
  }

  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    if (snakeLength < MAX_SNAKE_LENGTH) snakeLength++;
    score++;
    Serial.print("Food eaten! Score: ");
    Serial.println(score);
    gameSpeed = max(GAME_SPEED_MIN, gameSpeed - SPEED_INCREASE_PER_FOOD);
    generateFood();
  }
}

void drawGame() {
  FastLED.clear();

  for (int i = 0; i < snakeLength; i++) {
    if (snakeX[i] >= 0 && snakeX[i] < MATRIX_WIDTH &&
        snakeY[i] >= 0 && snakeY[i] < MATRIX_HEIGHT) {
      LedAddress addr = mapXY(snakeX[i], snakeY[i]);
      if (i == 0) addr.array[addr.index] = CRGB::White;
      else {
        int brightness = map(i, 1, snakeLength - 1, 200, 50);
        addr.array[addr.index] = CRGB(0, brightness, 0);
      }
    }
  }

  if (!gameOver) {
    LedAddress addr = mapXY(foodX, foodY);
    int brightness = 150 + 100 * sin(millis() / 200.0);
    addr.array[addr.index] = CRGB(brightness, 0, 0);
  }

  if (gameOver && (millis() / 250) % 2 == 0) {
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] >= 0 && snakeX[i] < MATRIX_WIDTH &&
          snakeY[i] >= 0 && snakeY[i] < MATRIX_HEIGHT) {
        LedAddress addr = mapXY(snakeX[i], snakeY[i]);
        addr.array[addr.index] = CRGB::Red;
      }
    }
  }

  FastLED.show();
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

  initGame();
}

void loop() {
  readJoystick();
  moveSnake();
  drawGame();

  if (gameOver && millis() - lastMoveTime > 3000) {
    initGame();
  }

  delay(10);
}
