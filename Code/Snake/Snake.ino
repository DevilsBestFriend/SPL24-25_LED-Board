#include <FastLED.h>    // Include this first
#include <LEDMatrix.h>  // Then include this

// LED Matrix configuration
#define NUM_LEDS_PER_STRIP 256   // 256 LEDs per strip (half of the total)
#define DATA_PIN_UPPER 25        // Upper matrix pin
#define DATA_PIN_LOWER 26        // Lower matrix pin
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

// Joystick pins
#define JOYSTICK_BUTTON_PIN 32
#define JOYSTICK_X_PIN 34
#define JOYSTICK_Y_PIN 35

// Game constants
#define INITIAL_SNAKE_LENGTH 3
#define MAX_SNAKE_LENGTH 100
#define GAME_SPEED_INITIAL 300  // Delay in milliseconds
#define GAME_SPEED_MIN 80       // Fastest game speed
#define SPEED_INCREASE_PER_FOOD 10

// Direction definitions
enum Direction { UP, RIGHT, DOWN, LEFT };

// Define two separate LED arrays for upper and lower parts
CRGB leds_upper[NUM_LEDS_PER_STRIP];
CRGB leds_lower[NUM_LEDS_PER_STRIP];

// Keep the original LED array for compatibility
CRGB leds_array[NUM_LEDS_PER_STRIP * 2];
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT/2, MATRIX_TYPE> leds;

// Structure to return both array pointer and index
struct LedAddress {
    CRGB* array;    // Pointer to the array (upper or lower)
    int index;      // Index within that array
};

// Game variables
int snakeX[MAX_SNAKE_LENGTH];
int snakeY[MAX_SNAKE_LENGTH];
int snakeLength;
Direction snakeDirection;
int foodX, foodY;
unsigned long lastMoveTime;
int gameSpeed;
bool gameOver;
int score = 0;

// Modified function to assign a color to a specific LED
// Returns a structure containing both the array and the position
LedAddress mapXY(int x, int y) {
  LedAddress result;
  int led;

  if (y < 8) {
    // Unteres Panel (normale Ausrichtung)
    led = x * 8;
    if ((x % 2) == 0) {
      led += y;
    } else {
      led += 7 - y;
    }

    result.array = leds_lower;
    result.index = led;

  } else {
    // Oberes Panel (180° gedreht: X und Y gespiegelt)
    int flippedX = MATRIX_WIDTH - 1 - x;
    int flippedY = 15 - y;  // oder: 7 - (y - 8)

    led = flippedX * 8;
    if ((flippedX % 2) == 0) {
      led += flippedY;
    } else {
      led += 7 - flippedY;
    }

    result.array = leds_upper;
    result.index = led;
  }

  return result;
}

// Read joystick input and update snake direction
void readJoystick() {
  int xValue = analogRead(JOYSTICK_X_PIN);
  int yValue = analogRead(JOYSTICK_Y_PIN);

  const int midPoint = 2048;
  const int threshold = 1000;

  Direction newDirection = snakeDirection;

  // ACHTUNG: Hier ist die korrigierte Zuweisung
  // Joystick nach oben → Y > mid → UP
  // Joystick nach rechts → X > mid → RIGHT

  if (yValue > midPoint + threshold && snakeDirection != LEFT) {
    newDirection = RIGHT;
  } else if (yValue < midPoint - threshold && snakeDirection != RIGHT) {
    newDirection = LEFT;
  } else if (xValue > midPoint + threshold && snakeDirection != DOWN) {
    newDirection = UP;
  } else if (xValue < midPoint - threshold && snakeDirection != UP) {
    newDirection = DOWN;
  }

  snakeDirection = newDirection;

  // Neustart bei Buttondruck
  if (gameOver && digitalRead(JOYSTICK_BUTTON_PIN) == LOW) {
    delay(200);  // Entprellen
    initGame();
  }
}


// Initialize the game
void initGame() {
    // Set initial snake position (center of the screen)
    snakeLength = INITIAL_SNAKE_LENGTH;
    for (int i = 0; i < snakeLength; i++) {
        snakeX[i] = MATRIX_WIDTH / 2 - i;
        snakeY[i] = MATRIX_HEIGHT / 2;
    }
    
    // Set initial direction
    snakeDirection = RIGHT;
    
    // Reset score
    score = 0;
    
    // Generate first food
    generateFood();
    
    // Set game speed
    gameSpeed = GAME_SPEED_INITIAL;
    
    // Reset game state
    gameOver = false;
    
    // Reset timer
    lastMoveTime = millis();
    
    Serial.println("New game started");
}

// Move the snake according to current direction
void moveSnake() {
    // Only move if enough time has passed
    if (millis() - lastMoveTime < gameSpeed || gameOver) {
        return;
    }
    
    lastMoveTime = millis();
    
    // Move body segments
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i-1];
        snakeY[i] = snakeY[i-1];
    }
    
    // Move head according to direction
    switch (snakeDirection) {
        case UP:
            snakeY[0]--;
            break;
        case RIGHT:
            snakeX[0]++;
            break;
        case DOWN:
            snakeY[0]++;
            break;
        case LEFT:
            snakeX[0]--;
            break;
    }
    
    // Check for collisions
    checkCollision();
}

// Check for collisions with walls, self, or food
void checkCollision() {
    // Get head position
    int headX = snakeX[0];
    int headY = snakeY[0];
    
    // Check wall collision
    if (headX < 0 || headX >= MATRIX_WIDTH || headY < 0 || headY >= MATRIX_HEIGHT) {
        gameOver = true;
        Serial.print("Game over! Final score: ");
        Serial.println(score);
        return;
    }
    
    /*// Check self collision (start from 1 to skip head)
    for (int i = 1; i < snakeLength; i++) {
        if (headX == snakeX[i] && headY == snakeY[i]) {
            gameOver = true;
            Serial.print("Game over! Final score: ");
            Serial.println(score);
            return;
        }
    }*/
    
    // Check food collision
    if (headX == foodX && headY == foodY) {
        // Increase snake length
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snakeLength++;
        }
        
        // Increase score
        score++;
        
        Serial.print("Food eaten! Score: ");
        Serial.println(score);
        
        // Increase game speed
        gameSpeed = max(GAME_SPEED_MIN, gameSpeed - SPEED_INCREASE_PER_FOOD);
        
        // Generate new food
        generateFood();
    }
}

// Generate food at a random position that's not on the snake
void generateFood() {
    bool validPosition;
    
    do {
        validPosition = true;
        
        // Generate random position
        foodX = random(MATRIX_WIDTH);
        foodY = random(MATRIX_HEIGHT);
        
        // Check if food position overlaps with snake
        for (int i = 0; i < snakeLength; i++) {
            if (foodX == snakeX[i] && foodY == snakeY[i]) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

// Draw the game on the LED matrix
void drawGame() {
    // Clear the matrix
    FastLED.clear();
    
    // Draw snake
    for (int i = 0; i < snakeLength; i++) {
        if (snakeX[i] >= 0 && snakeX[i] < MATRIX_WIDTH && 
            snakeY[i] >= 0 && snakeY[i] < MATRIX_HEIGHT) {
            
            LedAddress addr = mapXY(snakeX[i], snakeY[i]);
            
            // Head is white, body is green
            if (i == 0) {
                addr.array[addr.index] = CRGB(255, 255, 255); // White for head
            } else {
                // Create a gradient from head to tail
                int brightness = map(i, 1, snakeLength-1, 200, 50);
                addr.array[addr.index] = CRGB(0, brightness, 0); // Green gradient for body
            }
        }
    }
    
    // Draw food (red)
    if (!gameOver) {
        LedAddress addr = mapXY(foodX, foodY);
        
        // Make food pulsate
        int brightness = 150 + 100 * sin(millis() / 200.0);
        addr.array[addr.index] = CRGB(brightness, 0, 0); // Pulsating red for food
    }
    
    // If game over, flash the matrix
    if (gameOver) {
        if ((millis() / 250) % 2 == 0) {  // Flash every 250ms
            for (int i = 0; i < snakeLength; i++) {
                if (snakeX[i] >= 0 && snakeX[i] < MATRIX_WIDTH && 
                    snakeY[i] >= 0 && snakeY[i] < MATRIX_HEIGHT) {
                    
                    LedAddress addr = mapXY(snakeX[i], snakeY[i]);
                    addr.array[addr.index] = CRGB(255, 0, 0); // Red for game over
                }
            }
        }
    }
    
    // Update the display
    FastLED.show();
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    Serial.println("Snake Game starting...");
    
    // Initialize joystick pins
    pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    
    // Initialize FastLED for both LED strips
    FastLED.addLeds<WS2812B, DATA_PIN_UPPER, GRB>(leds_upper, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, DATA_PIN_LOWER, GRB>(leds_lower, NUM_LEDS_PER_STRIP);
    FastLED.setBrightness(50); // Set brightness to 50/255
    
    // Map the upper and lower arrays to the global array for LEDMatrix library
    for(int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
        leds_array[i] = leds_upper[i];
        leds_array[i + NUM_LEDS_PER_STRIP] = leds_lower[i];
    }
    
    leds.SetLEDArray(leds_array);
    
    // Clear all LEDs
    FastLED.clear();
    FastLED.show();
    
    // Initialize random number generator
    randomSeed(analogRead(36)); // Use an unused analog pin
    
    // Initialize the game
    initGame();
}
void debugVisualisierung() {
  FastLED.clear();

  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      LedAddress addr = mapXY(x, y);

      // Farbverlauf: Rot nach Blau über X, Helligkeit über Y
      uint8_t r = map(x, 0, MATRIX_WIDTH - 1, 0, 255);
      uint8_t g = 0;
      uint8_t b = map(x, 0, MATRIX_WIDTH - 1, 255, 0);
      uint8_t brightness = map(y, 0, MATRIX_HEIGHT - 1, 50, 255);

      addr.array[addr.index] = CRGB(r, g, b).nscale8(brightness);
    }
  }

  FastLED.show();
}


void loop() {
    // Read joystick
    readJoystick();
    
    // Move snake
    moveSnake();
    
    // Draw the game
    drawGame();
    
    // Small delay to stabilize
    delay(10);

}