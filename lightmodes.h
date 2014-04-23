//#include <FastLED.h>


class RGB {
 public:
  int red;
  int green;
  int blue;
  RGB();
  RGB(int r, int g, int b);
};


class LightMode {
 public:
  RGB *lights;
  int numberOfLights;
  virtual RGB* cycle();
  virtual int delayTime();
  virtual ~LightMode();
};


class TestMode: public LightMode {
 public:
  RGB* lights;
  int numberOfLights;
  RGB* cycle();
  int delayTime();
  TestMode(RGB *leds, int numberOfLights);
  ~TestMode();
};


class SolidMode: public LightMode {
 public:
  RGB* lights;
  RGB color;
  int numberOfLights;
  RGB* cycle();
  int delayTime();
  SolidMode(RGB *leds, int numberOfLights, int red, int green, int blue);
  ~SolidMode();
};

class SolidAlternateMode: public SolidMode {
 public:
  RGB color2;
  RGB* cycle();
  SolidAlternateMode(RGB *leds, int numberOfLights, int red, int green, int blue, int red2, int green2, int blue2);
};

class GroupMode: public LightMode {
 public:
  RGB* lights;
  RGB color;
  int numberOfLights;
  int on;
  int off;
  RGB* cycle();
  int delayTime();
  GroupMode(RGB *leds, int numberOfLights, int red, int green, int blue, int on, int off);
  ~GroupMode();
};


class TwinkleMode: public LightMode {
 public:
  RGB* originalLights;
  int* lightState;
  RGB* twinkleLight;
  TwinkleMode(RGB *leds, int numberOfLights);
  //TwinkleMode();
  ~TwinkleMode();
  int delayTime();
  RGB* cycle();
};


class WhiteTwinkleMode: public TwinkleMode {
 public:
  WhiteTwinkleMode(RGB *leds, int numberOfLights);
  ~WhiteTwinkleMode();
};


class ChristmasTwinkleMode: public TwinkleMode {
 public:
  ChristmasTwinkleMode(RGB *leds, int numberOfLights);
  ~ChristmasTwinkleMode();
};


class RainbowMode: public LightMode {
 public:
  RGB* lights;
  RGB firstLight;
  int lastState;
  int maxBrightness;
  int lightIncrease;
  RainbowMode(RGB *leds, int numberOfLights);
  int delayTime();
  RGB* cycle();
  ~RainbowMode();
};


class FlickerMode: public LightMode {
 public:
  RGB* originalLights;
  int* lightState;
  FlickerMode(RGB *leds, int numberOfLights);
  ~FlickerMode();
  RGB* cycle();
  int delayTime();
};


class CandleMode: public FlickerMode {
 public:
  CandleMode(RGB *leds, int numberOfLights);
  ~CandleMode();
};


class DropMode: public LightMode {
 public:
  DropMode(RGB *leds, int numberOfLights);
  ~DropMode();
  RGB* cycle();
  int delayTime();
  int position;
  int dropSpeed;
  int colorState;
  RGB dropColor;
  virtual void changeDropColor();
};

class HalloweenDropMode: public DropMode {
 public:
  HalloweenDropMode(RGB *leds, int numberOfLights);
  void changeDropColor();
};

class PulseMode: public LightMode {
 public:
  PulseMode(RGB *leds, int numberOfLights, int startIntensity, int midIntensity, int stopIntensity, int startDelay, int midDelay, int stopDelay);
  ~PulseMode();
  RGB *lights;
  RGB* cycle();
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
  LightningMode(RGB *leds, int numberOfLights);
  ~LightningMode();
  RGB* cycle();
  int pulsesLeft;
};
