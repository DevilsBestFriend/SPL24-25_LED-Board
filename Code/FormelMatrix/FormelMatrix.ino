#include <FastLED.h>    // Include this first
#include <LEDMatrix.h>  // Then include this

#define NUM_LEDS 512
#define DATA_PIN 25
#define SizeX 32
#define SizeY 16
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

// Define LED array
CRGB leds_array[NUM_LEDS];
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// Function to assign a color to a specific LED
int mapXY(int x, int y) {
    int led;
    if (y < 8) {
        led = x * 8;
        if ((x % 2) == 0) {
            led += y;
        } else {
            led += 7 - y;
        }
    } else {
        led = 8 * x + 32 * 8;
        if ((x % 2) == 0) {
            led += y - 8;
        } else {
            led += 7 - (y - 8);
        }
    }
    return led;
}

void setup() {
    // Initialize FastLED
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds_array, NUM_LEDS);
    FastLED.setBrightness(50); // Set brightness to 50/255
    leds.SetLEDArray(leds_array);
    
    // Clear all LEDs
    FastLED.clear();
    FastLED.show();
    
    delay(1000); // Wait a second before starting
}

void loop() {
    // Test mapXY by lighting up different positions with different colors
    
    // Clear all LEDs first
    FastLED.clear();
    
    // Test corner positions (0,0) = Red
    leds_array[mapXY(0, 0)] = CRGB(255, 0, 0);
    
    // Top right (31,0) = Green
    leds_array[mapXY(31, 0)] = CRGB(0, 255, 0);
    
    // Bottom left (0,15) = Blue
    leds_array[mapXY(0, 15)] = CRGB(0, 0, 255);
    
    // Bottom right (31,15) = Yellow
    leds_array[mapXY(31, 15)] = CRGB(255, 255, 0);
    
    // Center (16,8) = Purple
    leds_array[mapXY(16, 8)] = CRGB(128, 0, 128);
    
    // Pattern across the middle
    for (int x = 0; x < 32; x += 4) {
        leds_array[mapXY(x, 7)] = CRGB(255, 128, 0); // Orange
    }
    
    // Pattern down the middle
    for (int y = 0; y < 16; y += 4) {
        leds_array[mapXY(16, y)] = CRGB(0, 255, 255); // Cyan
    }
    
    // Show the updated LED colors
    FastLED.show();
    
    delay(500); // Wait half a second before updating
}
