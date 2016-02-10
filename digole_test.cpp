#include "application.h"

SYSTEM_THREAD(ENABLED);
STARTUP(WiFi.selectAntenna(ANT_AUTO));

#define _Digole_Serial_UART_
#include "DigoleGeo.h"

#define SIGNAL_BARS 4
#define SIGNAL_THRESHOLD 127 / SIGNAL_BARS

class Widget : public Printable {
  int x;
  int y;
  int width;
  int height;

public:
  void initialize(int _x, int _y, int _w, int _h) {
    x = _x;
    y = _y;
    width = _w;
    height = _h;
  }

  int getX() { return x; }
  int getY() { return y; }
  int getWidth() { return width; }
  int getHeight() { return height; }

  virtual size_t printTo(Print& p) const {
    size_t result = 0;

    result += p.print(String::format("x: %i, y: %i, width: %i, height: %i", x, y, width, height));

    return result;
  }

  virtual void render(DigoleSerialDisp&) = 0;
};

class Layout : public Widget {
public:
  void render(DigoleSerialDisp &display) {
    display.setTrueColor(0, 0, 255);

    display.drawRoundRect(getX(), getY(), getWidth() - 1, 30);
    display.drawRoundRect(getX(), getY() + 35, getWidth() - 1, 150);
  }
};

class RSSIMeter : public Widget {
public:
  void render(DigoleSerialDisp &display) {
    uint8_t strength = abs(WiFi.RSSI());
    char icon = '0';

    if(strength > 0) {
      switch(strength / SIGNAL_THRESHOLD) {
        case 0: icon = '1'; break;
        case 1: icon = '2'; break;
        case 2: icon = '3'; break;
        case 3: icon = '4'; break;
      }
    }

    display.setTrueColor(255, 255, 0);

    display.setFont(201);
    display.setTextPosAbs(getX(), getY());
    display.print(icon);
    delay(20);
  }
};

class SSIDLabel : public Widget {
public:
  void render(DigoleSerialDisp &display) {
    display.setTrueColor(255, 255, 0);

    display.setFont(18);
    display.setTextPosAbs(getX(), getY());
    display.print(WiFi.SSID());
    delay(20);
  }
};

class DigitalClock : public Widget {
public:
  void render(DigoleSerialDisp &display) {
    display.setTrueColor(255, 255, 255);

    display.setFont(200);
    display.setTextPosAbs(getX(), getY());
    display.print(Time.format("%l:%M"));
    delay(20);
  }
};

void render();
/*void play();*/

/*Timer Player(1000, play);*/

DigoleSerialDisp display(&Serial1, 115200);

Layout layout = Layout();
RSSIMeter rssiMeter = RSSIMeter();
SSIDLabel ssidLabel = SSIDLabel();
DigitalClock digitalClock = DigitalClock();

uint16_t melody[] = {1908, 2551, 2551, 2273, 2551, 0, 2024, 1908}; // C4, G3, G3, A3, G3, 0, B3, C4
uint8_t noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

uint16_t width;
uint16_t height;

uint32_t frameTime = 0;

void setup() {
  pinMode(DAC1, OUTPUT);

  Time.zone(-6);

  Serial.begin(115200);

  display.begin();
  display.backLightOn();
  display.disableCursor();

  delay(20);

  display.clearScreen();
  display.setRot90();
  display.setColor(1);

  delay(500);

  // After 90 deg rotation, w & h swap
  width  = display.getNativeDisplayHeight();
  height = display.getNativeDisplayWidth();

  layout.initialize(0, 0, width, height);
  rssiMeter.initialize(4, 27, 22, 31);
  ssidLabel.initialize(45, 26, 0, 0);
  digitalClock.initialize(110, 135, 0, 0);

  layout.render(display);

  /*display.setTrueColor(255, 255, 255);
  display.setPrintPos(0, 16);
  display.print(rssiMeter);*/

  Serial.printlnf("Particle Version: %s", System.version().c_str());

  /*Player.start();*/
}

void loop() {
  render();
  delay(80);
}

void render() {
  uint32_t start = millis();

  rssiMeter.render(display);
  ssidLabel.render(display);
  digitalClock.render(display);

  display.setFont(18);
  display.setTrueColor(0, 255, 0);

  display.setPrintPos(0, 14);
  display.print("Frame Time: ");
  display.println(frameTime);
  display.print("FPS: ");
  display.println(frameTime == 0 ? frameTime : 1000 / frameTime);

  frameTime = millis() - start;
}

/*void play() {
  static uint8_t i = 0;

  uint32_t duration = 1000/noteDurations[i];

  noTone(WKP);
  tone(WKP, melody[i], duration);
  Player.changePeriod(duration * 1.3);

  i = (i + 1) % 8;
  digitalWrite(D7, LOW);
}*/
