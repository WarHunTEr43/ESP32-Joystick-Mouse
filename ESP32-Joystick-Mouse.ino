#include <BleMouse.h>

//Doppel button: Scroll Modus (Nochmal drücken um umzukehren)
//Langer Button press: Rechte Maus Taste

// Joystick Pins
const int joyX = 36;
const int joyY = 39;

BleMouse bleMouse;

// Neutralwerte
const int X_NEUTRAL = 1830;
const int Y_NEUTRAL = 1850;
const int DEADZONE = 50;

const float speedFactor = 0.01;

// Button-Timing
unsigned long buttonPressStart = 0;
bool buttonWasPressed = false;
bool clickHandled = false;
const unsigned long LONG_PRESS_TIME = 500;

// Doppelklick-Erkennung
int clickCount = 0;
unsigned long lastClickTime = 0;
const unsigned long DOUBLE_CLICK_TIME = 400; // ms zwischen Klicks

// Modus
bool scrollMode = false;

// Scroll-Geschwindigkeit
const float scrollFactor = 0.002;
float scrollAccumulator = 0;

void setup() {
  Serial.begin(115200);
  bleMouse.begin();
  Serial.println("BLE Mouse ready!");
}

void loop() {
  if (bleMouse.isConnected()) {
    int xVal = analogRead(joyX);
    int yVal = analogRead(joyY);

    bool buttonPressed = false;

    // Button-Erkennung
    if (yVal > 4000) {
      buttonPressed = true;
      xVal = X_NEUTRAL;
      yVal = Y_NEUTRAL;
    }
    if (xVal >= 4095) {
      buttonPressed = true;
      xVal = X_NEUTRAL;
    }

    // --- Button-Logik ---
    if (buttonPressed && !buttonWasPressed) {
      // Flanke: gerade gedrückt
      buttonPressStart = millis();
      clickHandled = false;
    }
    else if (buttonPressed && buttonWasPressed) {
      // Gehalten → Rechtsklick (nur im normalen Modus sinnvoll)
      if (!clickHandled && !scrollMode &&
          (millis() - buttonPressStart >= LONG_PRESS_TIME)) {
        bleMouse.click(MOUSE_RIGHT);
        clickHandled = true;
        Serial.println("RECHTSKLICK");
      }
    }
    else if (!buttonPressed && buttonWasPressed) {
      // Losgelassen
      unsigned long pressDuration = millis() - buttonPressStart;

      if (!clickHandled) {
        // Nur kurze Drücke zählen für Doppelklick-Erkennung
        if (pressDuration < LONG_PRESS_TIME) {
          unsigned long now = millis();

          if ((now - lastClickTime) < DOUBLE_CLICK_TIME) {
            clickCount++;
          } else {
            clickCount = 1;
          }
          lastClickTime = now;

          // Doppelklick erkannt → Modus wechseln
          if (clickCount >= 2) {
            clickCount = 0;
            scrollMode = !scrollMode;
            scrollAccumulator = 0;

            if (scrollMode) {
              Serial.println("MODUS: Scroll");
            } else {
              Serial.println("MODUS: Maus");
            }

            clickHandled = true; // Kein Linksklick auslösen
          }
        }
      }

      // Verzögerter Linksklick: nur wenn kein Doppelklick kam
      // (wird unten nach dem Timeout ausgelöst)
    }

    buttonWasPressed = buttonPressed;

    // Verzögerter Einzelklick (Linksklick) nach Doppelklick-Timeout
    if (clickCount == 1 && !scrollMode &&
        (millis() - lastClickTime) > DOUBLE_CLICK_TIME) {
      bleMouse.click(MOUSE_LEFT);
      Serial.println("LINKSKLICK");
      clickCount = 0;
    }

    // --- Joystick-Offset ---
    int xOffset = xVal - X_NEUTRAL;
    int yOffset = yVal - Y_NEUTRAL;

    if (abs(xOffset) < DEADZONE) xOffset = 0;
    if (abs(yOffset) < DEADZONE) yOffset = 0;

    // --- Modus: Scrollen oder Bewegen ---
    if (scrollMode) {
      // Y-Achse → vertikales Scrollen
      // X-Achse → horizontales Scrollen
      scrollAccumulator += -yOffset * scrollFactor;

      int scrollStep = (int)scrollAccumulator;
      if (scrollStep != 0) {
        int hScroll = (int)(-xOffset * scrollFactor);
        bleMouse.move(0, 0, scrollStep, hScroll);
        scrollAccumulator -= scrollStep;
      }
    } else {
      int xMove = -xOffset * speedFactor;
      int yMove = -yOffset * speedFactor;
      bleMouse.move(xMove, yMove);
    }

    // Debug
    Serial.print("Modus: "); Serial.print(scrollMode ? "SCROLL" : "MAUS");
    Serial.print(" | X: "); Serial.print(xVal);
    Serial.print(" | Y: "); Serial.print(yVal);
    Serial.print(" | Clicks: "); Serial.println(clickCount);
  }

  delay(20);
}
