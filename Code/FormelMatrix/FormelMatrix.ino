#include <LEDMatrix.h>

#define SizeX 32
#define SizeY 16
// X von 0 bis 31
// Y von 0 bis 15 ab 8 ist es
// 0 Punkt rechts unten danach im zigzag

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

int mapXY(int x, int y)
{
    if (y < 8)
    {
        int led = x * 8;
        if ((x % 2) == 0)
        {
            led += y;
        }
        else
        {
            led += 7 - y;
        }
    }
    else
    {
        int led = 8 * x + 32 * 8;
        if ((x % 2) == 0)
        {
            led += y;
        }
        else
        {
            led += 7 - y;
        }
    }
}

void setup()
{
    // test code
    leds[mapXY(0, 0)] = CRGB::Red;
    leds[mapXY(8, 32)] = CRGB::Red;
    leds[mapXY(31, 0)] = CRGB::Red;
    leds[mapXY(0, 15)] = CRGB::Red;
}

void loop()
{
}
