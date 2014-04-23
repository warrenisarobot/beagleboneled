#include <FastLED.h>

/*
class RGB {
 public:
  int red;
  int green;
  int blue;
  RGB();
  RGB(int r, int g, int b);
};
*/

class LightMode {
 public:
  CRGB *lights;
  int numberOfLights;
  virtual CRGB* cycle();
  virtual int delayTime();
  virtual ~LightMode();
};


class TestMode: public LightMode {
 public:
  CRGB* lights;
  int numberOfLights;
  CRGB* cycle();
  int delayTime();
  TestMode(CRGB *leds, int numberOfLights);
  ~TestMode();
};


class SolidMode: public LightMode {
 public:
  CRGB* lights;
  CRGB color;
  int numberOfLights;
  CRGB* cycle();
  int delayTime();
  SolidMode(CRGB *leds, int numberOfLights, int red, int green, int blue);
  ~SolidMode();
};

class SolidAlternateMode: public SolidMode {
 public:
  CRGB color2;
  CRGB* cycle();
  SolidAlternateMode(CRGB *leds, int numberOfLights, int red, int green, int blue, int red2, int green2, int blue2);
};

class GroupMode: public LightMode {
 public:
  CRGB* lights;
  CRGB color;
  int numberOfLights;
  int on;
  int off;
  CRGB* cycle();
  int delayTime();
  GroupMode(CRGB *leds, int numberOfLights, int red, int green, int blue, int on, int off);
  ~GroupMode();
};


class TwinkleMode: public LightMode {
 public:
  CRGB* originalLights;
  int* lightState;
  CRGB* twinkleLight;
  TwinkleMode(CRGB *leds, int numberOfLights);
  //TwinkleMode();
  ~TwinkleMode();
  int delayTime();
  CRGB* cycle();
};


class WhiteTwinkleMode: public TwinkleMode {
 public:
  WhiteTwinkleMode(CRGB *leds, int numberOfLights);
  ~WhiteTwinkleMode();
};


class ChristmasTwinkleMode: public TwinkleMode {
 public:
  ChristmasTwinkleMode(CRGB *leds, int numberOfLights);
  ~ChristmasTwinkleMode();
};


class RainbowMode: public LightMode {
 public:
  CRGB* lights;
  CRGB firstLight;
  int lastState;
  int maxBrightness;
  int lightIncrease;
  RainbowMode(CRGB *leds, int numberOfLights);
  int delayTime();
  CRGB* cycle();
  ~RainbowMode();
};


class FlickerMode: public LightMode {
 public:
  CRGB* originalLights;
  int* lightState;
  FlickerMode(CRGB *leds, int numberOfLights);
  ~FlickerMode();
  CRGB* cycle();
  int delayTime();
};


class CandleMode: public FlickerMode {
 public:
  CandleMode(CRGB *leds, int numberOfLights);
  ~CandleMode();
};


class DropMode: public LightMode {
 public:
  DropMode(CRGB *leds, int numberOfLights);
  ~DropMode();
  CRGB* cycle();
  int delayTime();
  int position;
  int dropSpeed;
  int colorState;
  CRGB dropColor;
  virtual void changeDropColor();
};

class HalloweenDropMode: public DropMode {
 public:
  HalloweenDropMode(CRGB *leds, int numberOfLights);
  void changeDropColor();
};

class PulseMode: public LightMode {
 public:
  PulseMode(CRGB *leds, int numberOfLights, int startIntensity, int midIntensity, int stopIntensity, int startDelay, int midDelay, int stopDelay);
  ~PulseMode();
  CRGB *lights;
  CRGB* cycle();
  int delayTime();
  int state;
  int counter;
  int startIntensity;
  int midIntensity;
  int stopIntensity;
  int startDelay;
  int midDelay;
  int stopDelay;
  int remainingDelay;
  int tempDelay;
};

class LightningMode: public PulseMode {
 public:
  LightningMode(CRGB *leds, int numberOfLights);
  ~LightningMode();
  CRGB* cycle();
  int pulsesLeft;
};
