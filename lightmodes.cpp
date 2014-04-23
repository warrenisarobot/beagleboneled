#include "lightmodes.h"
#include <stdlib.h>
#include <arduino.h>
#include <FastLED.h>


void changeBrightness(CRGB *pixelIn, CRGB *pixelOut, int steps, int currentStep) {
  int scaleMe;
  int difference;
  if (currentStep > 0) {
    scaleMe = pixelIn->red;
    if (pixelIn->green > scaleMe) scaleMe = pixelIn->green;
    if (pixelIn->blue > scaleMe) scaleMe = pixelIn->blue;
    difference = 255-scaleMe;
  } else {
    scaleMe = 255;
    if (pixelIn->red != 0 && pixelIn->red < scaleMe) scaleMe = pixelIn->red;
    if (pixelIn->green != 0 && pixelIn->green < scaleMe) scaleMe = pixelIn->green;
    if (pixelIn->blue != 0 && pixelIn->blue < scaleMe) scaleMe = pixelIn->blue;
    difference = scaleMe;
  }
  //intensity * scale * originalcolor
  //I want biggest to max out out at 255
  //if biggetst is 205 and 10 steps to get to 255 then it is 5 each step
  //(255 - biggest) / 10 = 5
  //205 * x = 205 + 5 = 210
  //scaling factor = next_step / biggest
  float step = difference / steps;
  float intensity_scale = (scaleMe + (step * currentStep)) / scaleMe;
  int temp;
  pixelOut->red = pixelIn->red * intensity_scale;
  pixelOut->green = pixelIn->green * intensity_scale;
  pixelOut->blue = pixelIn->blue * intensity_scale;
}

/*
RGB::RGB() {
  this->red = 0;
  this->green = 0;
  this->blue = 0;
};

RGB::RGB(int r, int g, int b) {
  this->red = r;
  this->green = g;
  this->blue = b;
}
*/

CRGB* LightMode::cycle() {};
int LightMode::delayTime() {};
LightMode::~LightMode() {};


TestMode::TestMode(CRGB *leds, int numberOfLights) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
};

CRGB* TestMode::cycle() {
  for (int i=0;i<this->numberOfLights;i++) {
    this->lights[i].red = rand() % 250;
    this->lights[i].green = rand() % 250;
    this->lights[i].blue = rand() % 250;
  }
  return this->lights;
};

int TestMode::delayTime() {
  return 100;
};

TestMode::~TestMode() {
  //delete this->lights;
};


SolidMode::SolidMode(CRGB *leds, int numberOfLights, int red, int green, int blue) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
  this->color.red = red;
  this->color.green = green;
  this->color.blue = blue;
};

CRGB* SolidMode::cycle() {
  for (int i=0;i<this->numberOfLights;i++) {
    this->lights[i].red = this->color.red;
    this->lights[i].green = this->color.green;
    this->lights[i].blue = this->color.blue;
  }
  return this->lights;
};

int SolidMode::delayTime() {
  return 10;
};

SolidMode::~SolidMode() {
  //delete this->lights;
};

SolidAlternateMode::SolidAlternateMode(CRGB *leds, int numberOfLights, int red, int green, int blue, int red2, int green2, int blue2) : SolidMode(leds, numberOfLights, red, green, blue) {
  this->color2.red = red2;
  this->color2.green = green2;
  this->color2.blue = blue2;
};

CRGB* SolidAlternateMode::cycle() {
  for (int i=0;i<this->numberOfLights;i++) {
    if (i % 2 == 1) {
      this->lights[i].red = this->color.red;
      this->lights[i].green = this->color.green;
      this->lights[i].blue = this->color.blue;
    } else {
      this->lights[i].red = this->color2.red;
      this->lights[i].green = this->color2.green;
      this->lights[i].blue = this->color2.blue;
    }
  }
  return this->lights;
};


GroupMode::GroupMode(CRGB *leds, int numberOfLights, int red, int green, int blue, int on, int off) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
  this->color.red = red;
  this->color.green = green;
  this->color.blue = blue;
  this->on = on;
  this->off = off;
};

CRGB* GroupMode::cycle() {
  int onCount = 1;
  int offCount = 0;
  for (int i=0;i<this->numberOfLights;i++) {
    if (onCount > 0) {
      this->lights[i].red = this->color.red;
      this->lights[i].green = this->color.green;
      this->lights[i].blue = this->color.blue;
      onCount++;
    }
    if (onCount > this->on) {
      onCount = 0;
      offCount = 1;
    }
    if (offCount > 0) {
      offCount += 1;
      if (offCount > this->off) {
	offCount = 0;
	onCount = 1;
      }
    }
  }
  return this->lights;
};

int GroupMode::delayTime() {
  return 100;
};

GroupMode::~GroupMode() {
  //delete this->lights;
};


TwinkleMode::TwinkleMode(CRGB *leds, int numberOfLights) {
  this->numberOfLights = numberOfLights;
  this->lights = leds;
  this->originalLights = new CRGB[numberOfLights];
  //state of 0 means use original
  //1-10 means increasing for twinkle
  //11-20 means decreasing to original
  this->lightState = new int[numberOfLights];
  for (int i=0; i<numberOfLights; i++) {
    this->lightState[i] = 0;
  }
};

TwinkleMode::~TwinkleMode() {
  delete this->lightState;
  //delete this->lights;
  delete this->originalLights;
}

CRGB* TwinkleMode::cycle() {
  for (int i=0; i<numberOfLights; i++) {
    if (this->lightState[i] == 0)
      if (rand() % 300 == 1) this->lightState[i] = 1;
    if (this->lightState[i] == 0) {
      this->lights[i].red = this->originalLights[i].red;
      this->lights[i].green = this->originalLights[i].green;
      this->lights[i].blue = this->originalLights[i].blue;
    }
    else {
      int intensity = this->lightState[i];
      if (intensity > 10) {
	intensity = 20 - intensity;
      }
      if (this->twinkleLight != NULL )
	changeBrightness(this->twinkleLight, &this->lights[i], 10, intensity);
      else
	changeBrightness(&this->originalLights[i], &this->lights[i], 10, intensity);
      if (this->lightState[i] == 20) this->lightState[i] = 0;
      else this->lightState[i]++;
    }
  }
  return this->lights;
};

int TwinkleMode::delayTime() {
  return 10;
}


WhiteTwinkleMode::WhiteTwinkleMode(CRGB *leds, int numberOfLights) : TwinkleMode(leds, numberOfLights) {
  //
  this->numberOfLights = numberOfLights;
  for (int i=0; i<numberOfLights; i++) {
    this->originalLights[i].red = 10;
    this->originalLights[i].green = 10;
    this->originalLights[i].blue = 10;
  }
  this->twinkleLight = new CRGB(10, 10, 10);
};

WhiteTwinkleMode::~WhiteTwinkleMode() {
  delete this->twinkleLight;
};

ChristmasTwinkleMode::ChristmasTwinkleMode(CRGB *leds, int numberOfLights) : TwinkleMode(leds, numberOfLights) {
  //
  this->numberOfLights = numberOfLights;
  for (int i=0; i<numberOfLights; i++) {
    if ((i % 2) == 0) {
      this->originalLights[i].red = 20;
      this->originalLights[i].green = 0;
    } else {
      this->originalLights[i].red = 0;
      this->originalLights[i].green = 20;
    }
    this->originalLights[i].blue = 0;
  }
  //this->twinkleLight = new CRGB(10, 10, 10);
  this->twinkleLight = new CRGB;
  this->twinkleLight->red = 10;
  this->twinkleLight->green = 10;
  this->twinkleLight->blue = 10;
};

ChristmasTwinkleMode::~ChristmasTwinkleMode() {
  delete this->twinkleLight;
}


RainbowMode::RainbowMode(CRGB *leds, int numberOfLights) {
  this->lights = leds;
  this->firstLight.red = 255;
  this->firstLight.green = 0;
  this->firstLight.blue = 0;
  this->lastState = 0;
  this->numberOfLights = numberOfLights;
  this->lightIncrease = 4;
  this->maxBrightness = 255;
};

RainbowMode::~RainbowMode() {
  //delete this->lights;
}

CRGB* RainbowMode::cycle() {
  int state=0;
  CRGB pixel;
  pixel.red = this->firstLight.red;
  pixel.green = this->firstLight.green;
  pixel.blue = this->firstLight.blue;
  state = this->lastState;
  for (int i=0; i< this->numberOfLights; i++) {
    if (state==0) {
      if (pixel.green < maxBrightness - this->lightIncrease) {
	pixel.red -= this->lightIncrease;
	pixel.green += this->lightIncrease;
      } else {
	pixel.green = this->maxBrightness;
	pixel.red = 0;
	pixel.blue = 0;
	state = 1;
      }
    } else if (state ==1) {
      if (pixel.blue < maxBrightness - this->lightIncrease) {
	pixel.green -= this->lightIncrease;
	pixel.blue += this->lightIncrease;
      } else {
	pixel.green = 0;
	pixel.blue = this->maxBrightness;
	pixel.red = 0;
	state = 2;
      }
    } else if (state == 2) {
      if (pixel.red < this->maxBrightness - this->lightIncrease) {
	pixel.blue -= this->lightIncrease;
	pixel.red += this->lightIncrease;
      } else {
	pixel.blue = 0;
	pixel.green = 0;
	pixel.red = this->maxBrightness;
	state = 0;
      }
    }
    if (i==0) {
      this->firstLight.red = pixel.red;
      this->firstLight.green = pixel.green;
      this->firstLight.blue = pixel.blue;
      this->lastState = state;
    }
    this->lights[i].red = pixel.red;
    this->lights[i].green = pixel.green;
    this->lights[i].blue = pixel.blue;
  }
  return this->lights;
};

int RainbowMode::delayTime() {
  return 20;
};


FlickerMode::FlickerMode(CRGB *leds, int numberOfLights) {
  this->numberOfLights = numberOfLights;
  this->lights = leds;
  this->originalLights = new CRGB[numberOfLights];
  //state of 0 means use original
  //1-10 means increasing for twinkle
  //11-20 means decreasing to original
  this->lightState = new int[numberOfLights];
  for (int i=0; i<numberOfLights; i++) {
    this->lightState[i] = 0;
  }
}

FlickerMode::~FlickerMode() {
  //delete this->lights;
  delete this->originalLights;
  delete this->lightState;
}

CRGB* FlickerMode::cycle() {
  for (int i=0;i<this->numberOfLights;i++) {
    if (this->lightState[i] == 0) {
      if (rand() % 40 == 0) {
	this->lightState[i] = (rand() % 10) + 5;
      }
      int step = 0 - (rand() % 3);
      if (step != 0) 
	changeBrightness(&this->originalLights[i], &this->lights[i], 10, step);
      else {
	this->lights[i].red = this->originalLights[i].red;
	this->lights[i].green = this->originalLights[i].green;
	this->lights[i].blue = this->originalLights[i].blue;
      }
    } else {
      this->lightState[i]--;
      int step = -7 + (rand() % 6);
      changeBrightness(&this->originalLights[i], &this->lights[i], 10, step);
    }
  }
  return this->lights;
}

int FlickerMode::delayTime() {
  return 8;
}


CandleMode::CandleMode(CRGB *leds, int numberOfLights) : FlickerMode(leds, numberOfLights) {
  for (int i=0; i<this->numberOfLights; i++) {
    this->originalLights[i].red = 200;
    this->originalLights[i].green = 100;
    this->originalLights[i].blue = 0;
  }
}

CandleMode::~CandleMode() {
}


DropMode::DropMode(CRGB *leds, int numberOfLights) {
  //position starts at 1 and goes to number of lights + the max drop length
  this->numberOfLights = numberOfLights;
  this->lights = leds;
  this->position = 0;
  this->dropSpeed = 0;
  this->dropColor = CRGB();
  this->colorState = 0;
  for (int i=0;i<numberOfLights;i++) {
    this->lights[i].red = 0;
    this->lights[i].green = 0;
    this->lights[i].blue = 0;
  }
}

DropMode::~DropMode() {
  //delete this->lights;
}

CRGB *DropMode::cycle() {
  if (this->position == 0) {
    this->position = 1;
    this->changeDropColor();
  } else if (this->position > 0) {
    int dropLength = 6;
    for (int i=0; i<numberOfLights; i++) {
      if (i < this->position && i > (this->position - dropLength -1)) {
	int spot = this->position - 1 - i;
	if (spot == 0) {
	  this->lights[i].red = this->dropColor.red;
	  this->lights[i].green = this->dropColor.green;
	  this->lights[i].blue = this->dropColor.blue;
	} else {
	  changeBrightness(&this->dropColor, &this->lights[i], dropLength-1, spot * -1);
	}
      } else {
	this->lights[i].red = 0;
	this->lights[i].green = 0;
	this->lights[i].blue = 0;
      }
    }
    this->position += 1;
    if (this->position > this->numberOfLights + dropLength) this->position = 0;
  }
  return this->lights;
}

int DropMode::delayTime() {
  return 30;
}

void DropMode::changeDropColor() {
  switch (this->colorState) {
  case 0: 
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 0; break;
  case 1:
    this->dropColor.red = 120;
    this->dropColor.green = 120;
    this->dropColor.blue = 120; break;
  case 2:
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 0; break;
  case 3:
    this->dropColor.red = 120;
    this->dropColor.green = 120;
    this->dropColor.blue = 120; break;
  case 4:
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 0; break;
  case 5:
    this->dropColor.red = 0;
    this->dropColor.green = 200;
    this->dropColor.blue = 0; break;
  case 6:
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 0; break;
  case 7:
    this->dropColor.red = 0;
    this->dropColor.green = 200;
    this->dropColor.blue = 0; break;
  }
  this->colorState += 1;
  if (this->colorState > 7) this->colorState = 0;
}

HalloweenDropMode::HalloweenDropMode(CRGB *lights, int numberOfLights) : DropMode(lights, numberOfLights) {
}

void HalloweenDropMode::changeDropColor() {
  switch (this->colorState) {
  case 0: 
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 0; break;
  case 1:
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 0; break;
  case 2:
    this->dropColor.red = 0;
    this->dropColor.green = 200;
    this->dropColor.blue = 0; break;
  case 3:
    this->dropColor.red = 200;
    this->dropColor.green = 0;
    this->dropColor.blue = 200; break;
  }
  this->colorState += 1;
  if (this->colorState > 3) this->colorState = 0;
}


PulseMode::PulseMode(CRGB *leds, int numberOfLights, int startIntensity, int midIntensity, int stopIntensity, int startDelay, int midDelay, int stopDelay) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
  this->startIntensity = startIntensity;
  this->midIntensity = midIntensity;
  this->stopIntensity = stopIntensity;
  this->startDelay = startDelay;
  this->midDelay = midDelay;
  this->stopDelay = stopDelay;
  this->remainingDelay = 0;
  this->state = 0;
  this->counter = 0;
  this->tempDelay = 0;
}

PulseMode::~PulseMode() {
}

CRGB* PulseMode::cycle() {
  int colorStep = 255 / 10;
  if (this->state == 0) {
    if (this->counter <= this->midIntensity) {
      for (int i=0;i < numberOfLights; i++) {
	this->lights[i].red = colorStep * this->counter;
	this->lights[i].green = colorStep * this->counter;
	this->lights[i].blue = colorStep * this->counter;
	this->counter += 1;
	this->tempDelay = this->startDelay;
      }
    } else {
	this->state = 1;
	this->counter = 0;
    }
  }
  if (this->state == 1) {
    if (this->counter == 0) {
      this->tempDelay = this->midDelay;
      this->counter = 1;
    } else {
      this->state = 2;
      this->counter = this->midIntensity;
    }
  }
  if (this->state == 2) {
    if (this->counter >= this->stopIntensity)  {
      for (int i=0;i < numberOfLights; i++) {
	this->lights[i].red = colorStep * this->counter;
	this->lights[i].green = colorStep * this->counter;
	this->lights[i].blue = colorStep * this->counter;
      }
      this->counter--;
      this->tempDelay = this->stopDelay;
    } else {
      this->state = 3;
      this->counter = 0;
    }
  }
  return this->lights;
}

int PulseMode::delayTime() {
  return this->tempDelay;
}

LightningMode::LightningMode(CRGB *leds, int numberOfLights) : PulseMode(leds, numberOfLights, 1, 0, 1, 15, 50, 2) {
  //PulseMode::PulseMode(*leds, numberOfLights);
  this->state = 0;
  this->counter = 0;
  this->midIntensity = random(2, 11);
  this->pulsesLeft = random(3,6);
}

LightningMode::~LightningMode() {
  //delete this->lights;
}

CRGB *LightningMode::cycle() {
  //state 1 = clearing out lights
  //      2 = increasing brightness
  //      3 = decreasing brightness
  if (this->state < 3) {
    PulseMode::cycle();
  }
  if (this->state == 3) {
    if (this->pulsesLeft > 0) {
      this->state = 0;
      this->midIntensity = random(2,11);
      this->tempDelay = random(1, 10);
      this->pulsesLeft--;
      //pulse(100, 1, peak, 1, 15, 50, 2);
      //delay(random(1,10));
    } else if (this->counter == 0) {
      for (int i = 0; i < this->numberOfLights; i++) {
	this->lights[i].red = 0;
	this->lights[i].green = 0;
	this->lights[i].blue = 0;
      }
      this->counter = 1;
      this->tempDelay = 10;
    } else {
      this->counter = random(45, 190);
      this->state = 4;
    }
  }
  if (this->state ==  4) {
    if (this->counter > 0) {
      for (int i = 0; i < this->numberOfLights; i++) {
	this->lights[i].red = 0;
	this->lights[i].green = 0;
	this->lights[i].blue = 0;
      }
      this->tempDelay = 10;
      this->counter--;
    } else {
      this->state = 0;
      this->counter = 0;
      this->midIntensity = random(2, 11);
      this->pulsesLeft = random(3,6);
    }
  }
}

/*
void lightning(int numberOfLights) {
  int lightningPulses = random(3,6);
  for (int i=0; i < numberOfLights; i++) {
    SPI.transfer(0);
    SPI.transfer(0);
    SPI.transfer(0);
  }
  delay(1);
  for (int i=0; i < lightningPulses; i++) {
    int peak = random(2,11);
    pulse(100, 1, peak, 1, 15, 50, 2);
    delay(random(1,10));
  }
  for (int i=0; i < numberOfLights; i++) {
    SPI.transfer(0);
    SPI.transfer(0);
    SPI.transfer(0);
  }
  delay(random(900,3000));
}
*/