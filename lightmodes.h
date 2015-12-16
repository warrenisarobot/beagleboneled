//#include <FastLED.h>
#include <vector>


class RGB {
 public:
  int red;
  int green;
  int blue;
  RGB();
  RGB(int r, int g, int b);
  void copy(RGB &source);
  void update(int r, int g, int b);
};

class SpecklePixel {
 public:
  int index;
  int state;
  SpecklePixel(int index);
};

class Ripple {
 public:
  Ripple(int r, int g, int b, int position);
  ~Ripple();
  RGB baseColor;
  int state;
  int done;
  int position;
  int cycle(RGB *leds, int numberOfLights);
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
  virtual void updateTwinkleLed(int state);
  RGB* twinkleLed;
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
  void updateTwinkleLed(int state);
};


class ChristmasTwinkleMode: public TwinkleMode {
 public:
  ChristmasTwinkleMode(RGB *leds, int numberOfLights);
  ~ChristmasTwinkleMode();
  void updateTwinkleLed(int state);
};

class ReverseChristmasTwinkleMode: public TwinkleMode {
 public:
  ReverseChristmasTwinkleMode(RGB *leds, int numberOfLights);
  ~ReverseChristmasTwinkleMode();
  void updateTwinkleLed(int state);
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

class WhiteFlickerMode: public FlickerMode {
 public:
  WhiteFlickerMode(RGB *leds, int numberOfLights);
  ~WhiteFlickerMode();
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

class MovingColorMode: public LightMode {
 public:
  MovingColorMode(RGB *leds, int numberOfLights, RGB firstColor, int firstWidth, RGB secondColor, int secondWidth, int fadeLength, int delay);
  ~MovingColorMode();
  RGB* cycle();
  int delayTime();
  int state;
  RGB* leds;
  RGB firstColor;
  RGB secondColor;
  int firstWidth;
  int secondWidth;
  int fadeLength;
  int delay;
};

class MovingCandycaneMode: public MovingColorMode {
 public:
  MovingCandycaneMode(RGB *leds, int numberOfLights);
  ~MovingCandycaneMode();
};

class MovingChristmasMode: public MovingColorMode {
 public:
  MovingChristmasMode(RGB *leds, int numberOfLights);
  ~MovingChristmasMode();
};

class RippleMode: public LightMode {
 public:
  RippleMode(RGB *leds, int numberOfLights, int rippleOccurance);
  ~RippleMode();
  int state;
  RGB *leds;
  int numberOfLights;
  int rippleOccurance;
  int position;
  virtual void backgroundRGB(RGB *led, int index);
  virtual void newRipple();
  RGB* cycle();
  int delayTime();
  std::vector<Ripple> ripples;
};

class SpeckleColorMode: public LightMode {
 public:
  SpeckleColorMode(RGB *leds, int numberOfLights,  int fadesPerCycle, int solidCycles);
  ~SpeckleColorMode();
  int state;
  int counter;
  int colorCount;
  RGB *leds;
  int numberOfLights;
  int fadesPerCycle;
  int solidCycles;
  RGB previousColor;
  RGB nextColor;
  std::vector<SpecklePixel> speckleNotStarted;
  std::vector<SpecklePixel> speckleStarted;
  virtual void cycleColors();
  RGB* cycle();
  virtual int delayTime();
  void resetState();
};

class SpeckleMultiColorMode: public SpeckleColorMode {
 public:
  SpeckleMultiColorMode(RGB *leds, int numberOfLights);
  void cycleColors();
};
