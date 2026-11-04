#include "GUI.h"

GUI::GUI(TFTDisplay& tft, TFTTouch &ts, SPIClass &vspi) : _tft(tft), _ts(ts), _vspi(vspi) {} 

void GUI::begin() {
    _vspi.begin(18, 19, 23); // Initialize VSPI with SCK, MISO, MOSI
    _vspi.setFrequency(40000000);

    _tft.begin();
    _ts.begin();
    _ts.calibrate(540, 1000, 34, 460); // Calibrate touch screen

    _tft.setRotation(1);
    _tft.fillScreen(ILI9341_BLACK);
    drawRadarGraph();
    drawButtons(true); // Initial state = true
}

void GUI::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}

// Draw half-circle radar graph
void GUI::drawRadarGraph() {
  // Draw arcs starting at 50, increments of 50
  for (int cm = 50; cm <= MAX_DISTANCE_CM; cm += 50) {
      int r = cm * SCALE;
      // Labels at 30° line
      float labelRad = radians(180 - 30);
      int labelX = CENTER_X + (r + 5) * cos(labelRad) - 10;
      int labelY = CENTER_Y - (r + 5) * sin(labelRad) - 4;

      _tft.setTextColor(ILI9341_WHITE);
      _tft.setTextSize(1);
      _tft.setCursor(labelX, labelY);
      _tft.print(String(cm) + "cm");

      // Draw arcs
      for (int angle = 0; angle <= 180; angle++) {
          float rad = radians(180 - angle);
          int x = CENTER_X + r * cos(rad);
          int y = CENTER_Y - r * sin(rad);
          _tft.drawPixel(x, y, ILI9341_GREEN);
      }
  }

  // Draw radial lines
  for (int a = 0; a <= 180; a += 45) {
    float rad = radians(180 - a);
    int x = CENTER_X + RADIUS * cos(rad);
    int y = CENTER_Y - RADIUS * sin(rad);
    _tft.drawLine(CENTER_X, CENTER_Y, x, y, ILI9341_GREEN);

    // Angle labels -90°, -45°, 0°, 45°, 90°
    int labelX = CENTER_X + (RADIUS + 10) * cos(rad);
    int labelY = CENTER_Y - (RADIUS + 10) * sin(rad);

    _tft.setTextColor(ILI9341_WHITE);
    _tft.setTextSize(1);

    int displayAngle = 0;
    String angleStr = "";

    if (a == 0) {
        // Manually set coords
        labelX = 10;
        labelY = 195;
        displayAngle = -90;
    } else if (a == 45) {
        displayAngle = -45;
    } else if (a == 90) {
        // Manually set
        displayAngle = 0;
        _tft.setCursor(157, 9);
        _tft.print("0");
        continue;
    } else if (a == 135) {
        displayAngle = 45;
    } else if (a == 180) {
        // Manually set coords
        labelX = 303;
        labelY = 195;
        displayAngle = 90;
    }
    
    angleStr = String(displayAngle);    
    
    // Adjust cursor for better centering based on your display and text size
    int textWidth = angleStr.length() * 6; // Approximate width for size 1 text
    int textHeight = 8; // Approximate height for size 1 text

    _tft.setCursor(labelX - textWidth / 2, labelY - textHeight / 2);
    Serial.println("a: " + String(a) + ", displayAngle: " + String(displayAngle) + ", labelX | labelY : " + String(labelX) + " | " + String(labelY));
    _tft.print(angleStr);

    // Draw degree symbol as small circle
    _tft.drawCircle(labelX - textWidth / 2 + textWidth + 2, labelY - textHeight / 2, 2, ILI9341_WHITE);     // Radius 1 pixel for degree symbol
  }
}

void GUI::drawButtons(bool radarRunning) {
    uint16_t btnW = 100, btnH = 40;
    uint16_t spacing = 40;
    uint16_t y = _tft.height() - btnH - 10;

    uint16_t x1 = spacing;
    uint16_t x2 = _tft.width() - spacing - btnW;

    _tft.fillRect(x1, y, btnW, btnH, ILI9341_DARKGREY);
    _tft.setCursor(x1 + 10, y + btnH / 2 - 8);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setTextSize(2);
    _tft.print(radarRunning ? "STOP" : "START");

    _tft.fillRect(x2, y, btnW, btnH, ILI9341_DARKGREY);
    _tft.setCursor(x2 + 10, y + btnH / 2 - 8);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setTextSize(2);
    _tft.print("DISPLAY");
}

void GUI::updateSweepLine(int sweep_angle, uint16_t color) {
    float mappedAngleForDisplay = 90.0 - sweep_angle;
    float rad = radians(mappedAngleForDisplay + 90.0);

    int OUTER_X = CENTER_X + RADIUS * cos(rad);
    int OUTER_Y = CENTER_Y - RADIUS * sin(rad);
    _tft.drawLine(CENTER_X, CENTER_Y, OUTER_X, OUTER_Y, color);
}

void GUI::drawTriangleAtAngle(int angle, int distance, uint16_t color, float angularOffsetDegrees) {
    float mappedAngle1 = (90.0 - (angle - angularOffsetDegrees)) + 90.0;
    float rad1 = radians(mappedAngle1);
    int x1 = CENTER_X + (distance * SCALE * cos(rad1));
    int y1 = CENTER_Y - (distance * SCALE * sin(rad1));

    float mappedAngle2 = (90.0 - (angle + angularOffsetDegrees)) + 90.0;
    float rad2 = radians(mappedAngle2);
    int x2 = CENTER_X + (distance * SCALE * cos(rad2));
    int y2 = CENTER_Y - (distance * SCALE * sin(rad2));

    _tft.fillTriangle(CENTER_X, CENTER_Y, x1, y1, x2, y2, color);
}

void GUI::drawDetectedObject(int angle, int distance) {
    uint16_t color = ILI9341_RED;
    if (distance > 150) {
        color = ILI9341_YELLOW;
    }
    if (distance > MAX_DISTANCE_CM) {
        distance = 200; // Set to 200 for drawing purposes
    }
    drawTriangleAtAngle(angle, distance, color, 2.0); // Draw with a small angular offset
}

void GUI::eraseDetectedObject(int angle) {
    // Erase the object by drawing over it in black, with a slightly larger offset
    drawTriangleAtAngle(angle, MAX_DISTANCE_CM, ILI9341_BLACK, 2.4); 
}

void GUI::redrawRadar() {
    for (int cm = 50; cm <= MAX_DISTANCE_CM; cm += 50) {
        int r = cm * SCALE;
        // Label at 30° line
        float labelRad = radians(180 - 30);  // 30° mirrored
        int labelX = CENTER_X + (r + 5) * cos(labelRad) - 10;
        int labelY = CENTER_Y - (r + 5) * sin(labelRad) - 4;

        _tft.setTextColor(ILI9341_WHITE);
        _tft.setTextSize(1);
        _tft.setCursor(labelX, labelY);
        _tft.print(String(cm) + "cm");
        // Draw arc
        for (int angle = 0; angle <= 180; angle++) {
            float rad = radians(180 - angle);
            int x = CENTER_X + r * cos(rad);
            int y = CENTER_Y - r * sin(rad);
            _tft.drawPixel(x, y, ILI9341_GREEN);
        }
    }
    // Draw radial lines
    for (int a = 0; a <= 180; a += 45) {
        float rad = radians(180 - a); // flip for rotation 3
        int x = CENTER_X + RADIUS * cos(rad);
        int y = CENTER_Y - RADIUS * sin(rad);
        _tft.drawLine(CENTER_X, CENTER_Y, x, y, ILI9341_GREEN);
    }
}

// dB meter
void GUI::drawDistanceMeter(int distance) {
    const int meterX = 20;           // Starting X position
    const int meterY = 80;           // Starting Y position
    const int meterWidth = 280;      // Total width of the meter
    const int meterHeight = 60;      // Height of one segment
    const int totalSegments = 20;    // Number of vertical segments
    const int segmentGap = 2;
    const int segmentWidth = (meterWidth - (totalSegments - 1) * segmentGap) / totalSegments;

    // Print distance above the meter
    _tft.setCursor(meterX + 50, meterY - 50);
    _tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    _tft.setTextSize(2);
    if (distance >= MAX_DISTANCE_CM) {
        _tft.print("Distance: +200cm");
    } else {
        // Update to remove 'cmm' !!!!!!
        _tft.printf("");
        _tft.printf("Distance: %3dcm", distance);
    }

    // Calculate how many segments to light up based on distance
    int segmentsToLight = 0;
    if (distance > 0 && distance <= MAX_DISTANCE_CM) {
        float step = (float)MAX_DISTANCE_CM / totalSegments;
        segmentsToLight = totalSegments - (distance / step);
        segmentsToLight = constrain(segmentsToLight, 0, totalSegments);
    }

    // Draw the segments
    for (int i = 0; i < totalSegments; ++i) {
        _tft.fillRoundRect(meterX + 0 * (segmentWidth + segmentGap), meterY, segmentWidth, meterHeight, 5, ILI9341_GREEN); // Always draw one segment
        int currentX = meterX + i * (segmentWidth + segmentGap);
        uint16_t segmentColor;

        if (i < totalSegments / 3) {
            segmentColor = ILI9341_GREEN;
        } else if (i < 2 * totalSegments / 3) {
            segmentColor = ILI9341_YELLOW;
        } else {
            segmentColor = ILI9341_RED;
        }

        if (i < segmentsToLight) {
            _tft.fillRoundRect(currentX, meterY, segmentWidth, meterHeight, 5, segmentColor);
        } else {
            _tft.fillRoundRect(currentX, meterY, segmentWidth, meterHeight, 5, ILI9341_BLACK); // Unlit  segment
        }
    }
}