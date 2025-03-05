#define SizeX 32
#define SizeY 16
// X von 0 bis 31
// Y von 0 bis 15 ab 8 ist es
// 0 Punkt rechts unten danach im zigzag

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
}

void loop()
{
}
