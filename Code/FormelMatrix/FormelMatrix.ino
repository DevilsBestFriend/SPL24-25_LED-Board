#include <FastLED.h>    // Include this first
#include <LEDMatrix.h>  // Then include this

#define NUM_LEDS_PER_STRIP 256   // 256 LEDs per strip (half of the total)
#define DATA_PIN_UPPER 25        // Upper matrix pin
#define DATA_PIN_LOWER 26        // Lower matrix pin
#define SizeX 32
#define SizeY 16
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

// Define two separate LED arrays for upper and lower parts
CRGB leds_upper[NUM_LEDS_PER_STRIP];
CRGB leds_lower[NUM_LEDS_PER_STRIP];

// Keep the original LED array for compatibility
CRGB leds_array[NUM_LEDS_PER_STRIP * 2];
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// Structure to return both array pointer and index
struct LedAddress {
    CRGB* array;    // Pointer to the array (upper or lower)
    int index;      // Index within that array
};

// Modified function to assign a color to a specific LED
// Returns a structure containing both the array and the position
LedAddress mapXY(int x, int y) {
    LedAddress result;
    int led;
    
    if (y < 8) {
        // Upper half of the matrix (Pin 25)
        led = x * 8;
        if ((x % 2) == 0) {
            led += y;
        } else {
            led += 7 - y;
        }
        result.array = leds_upper;
        result.index = led;
    } else {
        // Lower half of the matrix (Pin 26)
        led = x * 8;
        if ((x % 2) == 0) {
            led += (y - 8);
        } else {
            led += 7 - (y - 8);
        }
        result.array = leds_lower;
        result.index = led;
    }
    
    return result;
}


void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    Serial.println("Dual-pin LED Matrix starting...");
    
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
    
    delay(1000); // Wait a second before starting
}

void loop() {
    // Clear all LEDs first
    FastLED.clear();
    
    // -- CORNER HIGHLIGHTS --
    
    // Top left corners in GREEN
    LedAddress addr = mapXY(0, 0);  // Upper panel top left
    addr.array[addr.index] = CRGB(0, 255, 0);
    
    addr = mapXY(0, 8);  // Lower panel top left
    addr.array[addr.index] = CRGB(0, 255, 0);
    
    // Bottom right corners in RED
    addr = mapXY(31, 7);  // Upper panel bottom right
    addr.array[addr.index] = CRGB(255, 0, 0);
    
    addr = mapXY(31, 15);  // Lower panel bottom right
    addr.array[addr.index] = CRGB(255, 0, 0);
    
    // -- HORIZONTAL LINES --
    
    // Middle line of upper panel (y=4)
    for (int x = 0; x < 32; x++) {
        addr = mapXY(x, 4);
        addr.array[addr.index] = CRGB(255, 255, 255); // White
    }
    
    // Middle line of lower panel (y=12)
    for (int x = 0; x < 32; x++) {
        addr = mapXY(x, 12);
        addr.array[addr.index] = CRGB(255, 255, 255); // White
    }
    
    // -- VERTICAL LINES --
    
    // Left quarter line (x=8)
    for (int y = 0; y < 16; y++) {
        addr = mapXY(8, y);
        addr.array[addr.index] = CRGB(0, 0, 255); // Blue
    }
    
    // Center line (x=16)
    for (int y = 0; y < 16; y++) {
        addr = mapXY(16, y);
        addr.array[addr.index] = CRGB(0, 0, 255); // Blue
    }
    
    // Right quarter line (x=24) 
    for (int y = 0; y < 16; y++) {
        addr = mapXY(24, y);
        addr.array[addr.index] = CRGB(0, 0, 255); // Blue
    }
    
    // Show the updated LED colors
    FastLED.show();
    
    delay(100); // Update quickly for responsive display
}
