#include "gui.h"
#include "Components.h"

#define servo_pin 15

// TFT Display pins
#define TFT_CSL  25  // TFT Chip Select
#define TFT_DC   33  // TFT Data/Command
#define TFT_RST  -1  // Use -1 if RST not connected
#define CST_PIN 5  // Touchscreen PIN

#define MAX_VISUAL_DISTANCE_CM 200 // Max visual range
#define MAX_SCAN_ANGLE 180

TFTDisplay tft(TFT_CSL, TFT_DC, TFT_RST);
TFTTouch ts(CST_PIN);
SPIClass vspi(VSPI); // Use VSPI on ESP32

GUI gui(tft, ts, vspi); // Pass objects as parameters
Components components(servo_pin, MAX_VISUAL_DISTANCE_CM);

// Point for TFT touch
struct Point {
    int x = 0;
    int y = 0;
} p;

bool isSystemRunning = true;
bool usingRadarGraph = true; // Start the program with radar-like graph

// Motor driving parameters
unsigned long lastMove = 0;
int angle = 0;
int prev_angle = 0;
int dir = 5; // + = forward

int currentDistance = 0;

void setup() {
    Serial.begin(115200);
    components.begin();
    gui.begin();
}

void updateProgramStatus() {
    // Get touch coordinates
    p.x = ts.getX();
    p.y = ts.getY();
    if (p.x > 0 && p.x < 70) { // Check if touch is within the button area
        // STOP/START button coordinates
        if (p.y > 30 && p.y < 130) { 
            isSystemRunning = !isSystemRunning; // Toggle state
            gui.drawButtons(isSystemRunning); // Redraw buttons to update state
        }
        // DISPLAY MODE button
        else if (p.y > 160 && p.y < 290) {
            gui.clearScreen();
            delay(100);
            usingRadarGraph = !usingRadarGraph; // Change display mode from radar to distance meter and vice versa
            if (usingRadarGraph) {
                gui.drawRadarGraph();
                gui.drawButtons(isSystemRunning); // Update the buttons based on status of program
            }
            else {
                // If system running status is STOPPED only draw distance meter template
                gui.drawDistanceMeter(0);
                gui.drawButtons(isSystemRunning);
            }
        }
    }
    delay(200); // Debounce
}

void loop() {
    ts.service(); 
    if (ts.getPressure()) { // Wait for screen touch
        updateProgramStatus();
    }

    if (isSystemRunning) {
        unsigned long now = millis();
        if (now - lastMove >= 50) { // Adjust speed (ms)
            lastMove = now;
            //Serial.print("Distance: ");
            //Serial.println(String(currentDistance) + " mm");
            if (usingRadarGraph) { // true = radar-like graph | false = distance meter (dB like meter)
                components.setServoAngle(angle); // Rotate servo
                currentDistance = components.getDistance();
                // Erase the sweep line
                gui.updateSweepLine(prev_angle, ILI9341_BLACK);
                // Erase the object from the angle 
                gui.eraseDetectedObject(angle);
                // Update the graph
                gui.redrawRadar();
                // Draw current sweep line
                gui.updateSweepLine(angle, ILI9341_GREEN);
                // Draw current detected object
                gui.drawDetectedObject(angle, currentDistance);

                prev_angle = angle; // Storing previous angle for sweep line deletion
                angle += dir;
                if (angle >= 180 || angle <= 0) dir = -dir; // Change direction
            }
            else {
                currentDistance = components.getDistance();
                gui.drawDistanceMeter(currentDistance);
            }
        }
    }
}