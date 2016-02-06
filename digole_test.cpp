#include "application.h"

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

SYSTEM_THREAD(ENABLED);

#define _Digole_Serial_UART_
#include "DigoleGeo.h"

DigoleSerialDisp Display(&Serial1, 115200);

uint16_t width;
uint16_t height;
uint8_t y = 0;

void setup() {
  Serial.begin(115200);

  Display.begin();

  Display.disableCursor();
  Display.backLightOn();
  Display.clearScreen();
  Display.setColor(1);
  Display.setRot90();

  // After 90 deg rotation, w & h swap
  width  = Display.getNativeDisplayHeight();
  height = Display.getNativeDisplayWidth();

  Serial.printlnf("Particle Version: %s", System.version().c_str());
}

void loop() {
  Display.resetDrawWindow();
  Display.setDrawWindow(0, 0, width, 25);

  Display.setFont(18);
  Display.setTrueColor(255, 255, 0);

  Display.drawRoundRect(0, 0, width - 1, 24);

  Display.setTrueColor(255, 255, 255);

  for(uint8_t i = 0; i < 5; i++) {
    Display.drawArc(4, 20, i * 4, 0, 90, 2);
  }

  Display.setTextPosAbs(30, 18);
  Display.print(WiFi.SSID());

  delay(250);
}
