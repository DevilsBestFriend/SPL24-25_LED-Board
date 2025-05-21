#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <FastLED.h>
#include <LEDMatrix.h>

// Hardware-Konfiguration
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16
#define JOYSTICK_BUTTON_PIN 32

// Funktionsprototypen
void snakeTask(void *pvParameters);
void initSnake();
void drawSnake();
void moveSnake();
void readJoystick();

#endif
