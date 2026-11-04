/*
  GUI class for managing the graphical interface of a radar system
  displayed on a 2.4" TFT LCD touchscreen. 
  
  It contains functions for rendering the radar graph, sweep animation, distance indicators, buttons, and
  detected object markers. 

  It provides methods for initializing the display, drawing dynamic radar elements, and updating visuals based
  on sensor input and user interaction.
*/

#ifndef GUI_h
#define GUI_h

#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "TFT-LCD-Breakout-2.4-With-Touch-SOLDERED.h"

class GUI {
  public:
    GUI(TFTDisplay &tft, TFTTouch &ts, SPIClass &vspi);

    void begin();
    void clearScreen();
    void drawRadarGraph();
    void drawButtons(bool radarRunning);
    void updateSweepLine(int sweep_angle, uint16_t color);
    void erasePreviousSweep();
    void drawCurrentSweep(int currentAngle);
    void drawDetectedObject(int angle, int distance);
    void eraseDetectedObject(int angle);
    void redrawRadar();
    void drawDistanceMeter(int distance);

  private:
    TFTDisplay& _tft;
    TFTTouch& _ts;
    SPIClass& _vspi;

    // Constants
    static const int CENTER_X = 160;
    static const int CENTER_Y = 180;
    static const int RADIUS = 160;
    static const int MAX_DISTANCE_CM = 200;
    static constexpr float SCALE = (RADIUS / (float)MAX_DISTANCE_CM); // constexpr for compile-time constant

    void drawTriangleAtAngle(int angle, int distance, uint16_t color, float angularOffsetDegrees);
};

#endif