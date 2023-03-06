#include <M5Core2.h>

ButtonColors stop_on_clrs = {RED, WHITE, WHITE};
ButtonColors start_on_clrs = {GREEN, WHITE, WHITE};
ButtonColors off_clrs = {BLACK, WHITE, WHITE};

Button start_btn(10, 150, 120, 80, false, "START", off_clrs, start_on_clrs,
                 CC_DATUM);
Button stop_btn(190, 150, 120, 80, false, "STOP", off_clrs, stop_on_clrs,
                CC_DATUM);

bool running = false;
bool stoppedBefore = false;

bool isVibrating = false;
unsigned long vibrateEnd;

int startTime = 0;
int elapsed = 0;

String currentTime;

void vibrate(int milliseconds) {
  M5.Axp.SetLDOEnable(3, true);
  vibrateEnd = milliseconds + millis();
  isVibrating = true;
}

String addLeadingZero(uint8_t time) {
  if (time < 10) {
    return "0" + String(time);
  } else {
    return String(time);
  }
}

void drawTime(int x, int y) {
  if (running) {
    elapsed = millis() - startTime;
  }

  int hours = elapsed / 3600000;
  int minutes = (elapsed / 60000) - (hours * 60);
  int seconds = (elapsed / 1000) % 60;
  int milliseconds = (elapsed % 1000) / 10;

  currentTime = addLeadingZero(hours) + ":" + addLeadingZero(minutes) + ":" +
                addLeadingZero(seconds) + "." + addLeadingZero(milliseconds);

  M5.Lcd.drawString(currentTime, x, y, 7);
}

void startPressed(Event &e) {
  vibrate(250);
  stop_btn.setLabel("STOP");
  M5.Buttons.draw();

  if (!running) {
    running = true;
    stoppedBefore = false;
    startTime = millis() - elapsed;
  }
}

void stopPressed(Event &e) {
  vibrate(250);

  static unsigned long lastStopTime = 0;
  if (millis() - lastStopTime > 500) {
    running = false;
    if (stoppedBefore) {
      stop_btn.setLabel("STOP");
      elapsed = 0;
      startTime = 0;
    } else {
      stoppedBefore = true;
      elapsed = millis() - startTime;
      stop_btn.setLabel("RESET");
    }

    lastStopTime = millis();
  }
}

void setup() {
  M5.begin();
  M5.Buttons.draw();
  start_btn.addHandler(startPressed);
  stop_btn.addHandler(stopPressed);
}

void loop() {
  M5.update();

  // Only redraw the timer every 100 milliseconds to avoid flicker
  static unsigned long lastUpdated = 0;
  if ((millis() - lastUpdated) >= 100) {
    lastUpdated = millis();
    drawTime(160, 80);
  }

  // Check if we need to disable the vibration motor
  if ((millis() >= vibrateEnd) && isVibrating) {
    M5.Axp.SetLDOEnable(3, false);
  }
}
