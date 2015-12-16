#include "lightmodes.h"
#include <stdlib.h>
#include <iostream>
#include <cmath>


void changeBrightness(RGB *pixelIn, RGB *pixelOut, int steps, int currentStep) {
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


//merge the color1 into color2 with the given resolution using
//currentStep and steps.  With a currentStep of 0 this just
//returns color1.  With a currentStep equal to steps this will return
//color2.  
void colorMerge(RGB &color1, RGB &color2, RGB *pixelOut, int steps, int currentStep) {
  float increase = (float) currentStep / (float) steps;
  float colorIncrease = 0;
  pixelOut->red = (int) color1.red + ((color2.red - color1.red) * increase);
  pixelOut->green = (int) color1.green + ((color2.green - color1.green) * increase);
  pixelOut->blue = (int) color1.blue + ((color2.blue - color1.blue) * increase);
}

void copyRGB(RGB *source, RGB *dest) {
  dest->red = source->red;
  dest->green = source->green;
  dest->blue = source->blue;
}


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

void RGB::copy(RGB &source) {
  this->red = source.red;
  this->green = source.green;
  this->blue = source.blue;
}

void RGB::update(int r, int g, int b) {
  this->red = r;
  this->green = g;
  this->blue = b;
}

SpecklePixel::SpecklePixel(int index) {
  this->index = index;
  this->state = 0;
}

Ripple::Ripple(int r, int g, int b, int position) {
  this->done = 0;
  this->baseColor = RGB(r, g, b);
  this->state = 0;
  this->position = position;
}

Ripple::~Ripple() {
  //delete this->baseColor;
}

int Ripple::cycle(RGB *leds, int numberOfLights) {
  int length = 0;
  RGB temp(0, 0, 0);
  this->state += 1;
  length = log((float) this->state + 2);
  if (this->state < 120) {
    for (int i=0; i<length; i++) {
      if (this->state < 60) changeBrightness(&this->baseColor, &temp, 15, 15-i);
      else changeBrightness(&this->baseColor, &temp, 15, 15 - i - ((this->state-60) / 10 ));
      std::cout << "position: " << this->position << ", index: " << i << ", ledsize: " << numberOfLights << "(" << temp.red << "," << temp.green << "," << temp.blue << ")\r\n";
      if ((this->position + i) < numberOfLights ) {
	leds[this->position + i].red = temp.red;
	leds[this->position + i].green = temp.green;
	leds[this->position + i].blue = temp.blue;
      }
      if ((this->position - 1) >= 0) {
	leds[this->position - i].red = temp.red;
	leds[this->position - i].green = temp.green;
	leds[this->position - i].blue = temp.blue;
      }
    }
  } else {
    this->done = 1;
  }
}

RGB* LightMode::cycle() {};
int LightMode::delayTime() {};
LightMode::~LightMode() {};


TestMode::TestMode(RGB *leds, int numberOfLights) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
};

RGB* TestMode::cycle() {
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


SolidMode::SolidMode(RGB *leds, int numberOfLights, int red, int green, int blue) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
  this->color.red = red;
  this->color.green = green;
  this->color.blue = blue;
};

RGB* SolidMode::cycle() {
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

SolidAlternateMode::SolidAlternateMode(RGB *leds, int numberOfLights, int red, int green, int blue, int red2, int green2, int blue2) : SolidMode(leds, numberOfLights, red, green, blue) {
  this->color2.red = red2;
  this->color2.green = green2;
  this->color2.blue = blue2;
};

RGB* SolidAlternateMode::cycle() {
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


GroupMode::GroupMode(RGB *leds, int numberOfLights, int red, int green, int blue, int on, int off) {
  this->lights = leds;
  this->numberOfLights = numberOfLights;
  this->color.red = red;
  this->color.green = green;
  this->color.blue = blue;
  this->on = on;
  this->off = off;
};

RGB* GroupMode::cycle() {
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


TwinkleMode::TwinkleMode(RGB *leds, int numberOfLights) {
  this->numberOfLights = numberOfLights;
  this->lights = leds;
  this->originalLights = new RGB[numberOfLights];
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

RGB* TwinkleMode::cycle() {
  RGB twinkleLight;
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
      this->updateTwinkleLed(i);
      if (this->twinkleLed != NULL) {
	changeBrightness(this->twinkleLed, &this->lights[i], 10, intensity);
      } else {
	changeBrightness(&this->originalLights[i], &this->lights[i], 10, intensity);
      }
      if (this->lightState[i] == 20) this->lightState[i] = 0;
      else this->lightState[i]++;
    }
  }
  return this->lights;
};

int TwinkleMode::delayTime() {
  return 10;
}

void TwinkleMode::updateTwinkleLed(int state) {
}


WhiteTwinkleMode::WhiteTwinkleMode(RGB *leds, int numberOfLights) : TwinkleMode(leds, numberOfLights) {
  //
  this->numberOfLights = numberOfLights;
  for (int i=0; i<numberOfLights; i++) {
    this->originalLights[i].red = 10;
    this->originalLights[i].green = 10;
    this->originalLights[i].blue = 10;
  }
  this->twinkleLed = new RGB(10, 10, 10);
};

WhiteTwinkleMode::~WhiteTwinkleMode() {
  delete this->twinkleLed;
};

void WhiteTwinkleMode::updateTwinkleLed(int state) {
}

ChristmasTwinkleMode::ChristmasTwinkleMode(RGB *leds, int numberOfLights) : TwinkleMode(leds, numberOfLights) {
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
  this->twinkleLed = new RGB(10, 10, 10);  
};

ChristmasTwinkleMode::~ChristmasTwinkleMode() {
  delete this->twinkleLed;
};

void ChristmasTwinkleMode::updateTwinkleLed(int state) {
}

ReverseChristmasTwinkleMode::ReverseChristmasTwinkleMode(RGB *leds, int numberOfLights) : TwinkleMode(leds, numberOfLights) {
  //
  this->numberOfLights = numberOfLights;
  for (int i=0; i<numberOfLights; i++) {
    this->originalLights[i].red = 50;
    this->originalLights[i].green = 50;
    this->originalLights[i].blue = 50;
  }
  this->twinkleLed = new RGB(255, 0, 0);  
};

ReverseChristmasTwinkleMode::~ReverseChristmasTwinkleMode() {
  delete this->twinkleLed;
};

void ReverseChristmasTwinkleMode::updateTwinkleLed(int state) {
  if (state % 2 == 0) {
    this->twinkleLed->red = 255;
    this->twinkleLed->green = 0;
  } else {
    this->twinkleLed->red = 0;
    this->twinkleLed->green = 255;
  }
}

RainbowMode::RainbowMode(RGB *leds, int numberOfLights) {
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

RGB* RainbowMode::cycle() {
  int state=0;
  RGB pixel;
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


FlickerMode::FlickerMode(RGB *leds, int numberOfLights) {
  this->numberOfLights = numberOfLights;
  this->lights = leds;
  this->originalLights = new RGB[numberOfLights];
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

RGB* FlickerMode::cycle() {
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


CandleMode::CandleMode(RGB *leds, int numberOfLights) : FlickerMode(leds, numberOfLights) {
  for (int i=0; i<this->numberOfLights; i++) {
    this->originalLights[i].red = 200;
    this->originalLights[i].green = 100;
    this->originalLights[i].blue = 0;
  }
}

CandleMode::~CandleMode() {
}

WhiteFlickerMode::WhiteFlickerMode(RGB *leds, int numberOfLights) : FlickerMode(leds, numberOfLights) {
  for (int i=0; i<this->numberOfLights; i++) {
    this->originalLights[i].red = 50;
    this->originalLights[i].green = 50;
    this->originalLights[i].blue = 50;
  }
}

WhiteFlickerMode::~WhiteFlickerMode() {
}


DropMode::DropMode(RGB *leds, int numberOfLights) {
  //position starts at 1 and goes to number of lights + the max drop length
  this->numberOfLights = numberOfLights;
  this->lights = leds;
  this->position = 0;
  this->dropSpeed = 0;
  this->dropColor = RGB();
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

RGB *DropMode::cycle() {
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

HalloweenDropMode::HalloweenDropMode(RGB *lights, int numberOfLights) : DropMode(lights, numberOfLights) {
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


PulseMode::PulseMode(RGB *leds, int numberOfLights, int startIntensity, int midIntensity, int stopIntensity, int startDelay, int midDelay, int stopDelay) {
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

RGB* PulseMode::cycle() {
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

LightningMode::LightningMode(RGB *leds, int numberOfLights) : PulseMode(leds, numberOfLights, 1, 0, 1, 15, 50, 2) {
  //PulseMode::PulseMode(*leds, numberOfLights);
  this->state = 0;
  this->counter = 0;
  //this->midIntensity = random(2, 11);
  this->midIntensity = rand() % 9 + 2;
  //this->pulsesLeft = random(3,6);
  this->pulsesLeft = random() % 3 + 3;
}

LightningMode::~LightningMode() {
  //delete this->lights;
}

RGB *LightningMode::cycle() {
  //state 1 = clearing out lights
  //      2 = increasing brightness
  //      3 = decreasing brightness
  if (this->state < 3) {
    PulseMode::cycle();
  }
  if (this->state == 3) {
    if (this->pulsesLeft > 0) {
      this->state = 0;
      //this->midIntensity = random(2,11);
      this->midIntensity = rand() % 9 + 2;
      //this->tempDelay = random(1, 10);
      this->tempDelay = random() % 9 + 1;
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
      //this->counter = random(45, 190);
      this->counter = random() % 145 + 45;
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
      //this->midIntensity = random(2, 11);
      this->midIntensity = random() % 9 + 2;
      //this->pulsesLeft = random(3,6);
      this->pulsesLeft = random() % 3 + 3;
    }
  }
}


MovingColorMode::MovingColorMode(RGB *leds, int numberOfLights, RGB firstColor, int firstWidth, RGB secondColor, int secondWidth, int fadeLength, int delay) {
  this->state = 0;
  this->leds = leds;
  this->numberOfLights = numberOfLights;
  this->firstColor = firstColor;
  this->secondColor = secondColor;
  this->firstWidth = firstWidth;
  this->secondWidth = secondWidth;
  this->delay = delay;
  this->fadeLength = fadeLength;
};

MovingColorMode::~MovingColorMode() {
};

RGB *MovingColorMode::cycle() {
  int startSpot = this->state % (this->firstWidth + this->secondWidth);
  int position = this->state;
  for (int i=0; i < this->numberOfLights; i++) {
    position = (position + 1) % (this->firstWidth + this->secondWidth + (this->fadeLength * 2));
    if (position <= this->firstWidth) {
      this->leds[i].red = this->firstColor.red;
      this->leds[i].green = this->firstColor.green;
      this->leds[i].blue = this->firstColor.blue;
    } else if (position <= this->firstWidth + this->fadeLength) {
      colorMerge(this->firstColor, this->secondColor, &this->leds[i], this->fadeLength + 1, position - this->firstWidth);
    } else if (position <= this->firstWidth + this->fadeLength + this->secondWidth) {
      this->leds[i].red = this->secondColor.red;
      this->leds[i].green = this->secondColor.green;
      this->leds[i].blue = this->secondColor.blue;
    } else {
      colorMerge(this->secondColor, this->firstColor, &this->leds[i], this->fadeLength + 1, position - this->firstWidth - this->secondWidth - this->fadeLength);
    }
  }
  this->state += 1;
  return  this->leds;
};

int MovingColorMode::delayTime() {
  return this->delay;
}

MovingCandycaneMode::MovingCandycaneMode(RGB *leds, int numberOfLights) : MovingColorMode(leds, numberOfLights, RGB(255, 10, 10), 7, RGB(50, 50, 50), 7, 3, 30) {
}

MovingCandycaneMode::~MovingCandycaneMode() {
}

MovingChristmasMode::MovingChristmasMode(RGB *leds, int numberOfLights) : MovingColorMode(leds, numberOfLights, RGB(255, 10, 10), 10, RGB(10, 255, 10), 10, 4, 30) {
}

MovingChristmasMode::~MovingChristmasMode() {
}

RippleMode::RippleMode(RGB *leds, int numberOfLights, int rippleOccurance) {
  this->leds = leds;
  this->numberOfLights = numberOfLights;
  this->rippleOccurance = rippleOccurance;
  this->state = 0;
}

RippleMode::~RippleMode() {
  this->ripples.clear();
}

void RippleMode::backgroundRGB(RGB *led, int index) {
  led->red = 10;
  led->green = 10;
  led->blue = 10;
}

void RippleMode::newRipple() {
  int red, green, blue;
  switch(this->state % 6) {
  case 0: red = 250; green = 0; blue=0; break;
  case 1: red = 150; green = 150; blue=0; break;
  case 2: red = 0; green = 250; blue=0; break;
  case 3: red = 0; green = 150; blue=150; break;
  case 4: red = 0; green = 0; blue=250; break;
  case 5: red = 150; green = 0; blue=150; break;
  }
  this->ripples.push_back(Ripple(red, green, blue, rand() % this->numberOfLights));
}

RGB* RippleMode::cycle() {
  this->state += 1;
  if (rand() % 100 <= this->rippleOccurance) {
    this->newRipple();
  }
  for (int i=0; i< this->numberOfLights; i++) {
    this->backgroundRGB(&this->leds[i], i);
  }
  for (int i=(int) this->ripples.size()-1; i >= 0; i-- ) {
    if (this->ripples[i].done) {
      this->ripples.erase(this->ripples.begin() + i);
      std::cout << "removing ripple " << i << "/" << this->ripples.size() << "\r\n";
    } else {
      std::cout << "cycling ripple " << i << "/" << this->ripples.size() << " at " << this->ripples[i].position << "\r\n";
      this->ripples[i].cycle(this->leds, this->numberOfLights);
    }
  }
  return this->leds;
}

int RippleMode::delayTime() {
  return 10;
}


SpeckleColorMode::SpeckleColorMode(RGB *leds, int numberOfLights,  int fadesPerCycle, int solidCycles) {
  this->leds = leds;
  this->numberOfLights = numberOfLights;
  this->fadesPerCycle = fadesPerCycle;
  this->counter = 0;
  this->colorCount = 0;
  this->solidCycles = solidCycles;
  this->resetState();
  this->cycleColors();
}

SpeckleColorMode::~SpeckleColorMode() {
}

void SpeckleColorMode::cycleColors() {
  this->previousColor.copy(this->nextColor);
  switch (this->colorCount % 3) {
  case 0: this->nextColor.update(250,0,0); break;
  case 1: this->nextColor.update(0,250,0); break;
  case 2: this->nextColor.update(0,0,250); break;
  }
  this->colorCount += 1;
}

void SpeckleColorMode::resetState() {
  this->state = 0;
  this->speckleNotStarted.clear();
  this->speckleStarted.clear();
  for (int i=0; i<this->numberOfLights; i++ ) {
    this->speckleNotStarted.push_back(SpecklePixel(i));
  }
}

RGB* SpeckleColorMode::cycle() {
  if (this->speckleStarted.size() > 0 || this->speckleNotStarted.size() > 0) {
    //now pick the next colors to start fading in
    //std::cout << this->speckleNotStarted.size() << " lights left to fade\r\n";
      for (int i=0; i<std::min((int) this->speckleNotStarted.size(), this->fadesPerCycle); i++) {
	int index = rand() % this->speckleNotStarted.size();
	this->speckleStarted.push_back(this->speckleNotStarted[index]);
	this->speckleNotStarted.erase(this->speckleNotStarted.begin()+index);
      }
      //std::cout << this->speckleStarted.size() << " lights currently fading\r\n";
      for (int i=this->speckleStarted.size()-1; i>=0; i--) {
	if (this->speckleStarted[i].state < 20) {
	  this->speckleStarted[i].state += 1;
	  colorMerge(this->previousColor, this->nextColor, &this->leds[this->speckleStarted[i].index], 20, this->speckleStarted[i].state);
	  if (this->speckleStarted[i].index == 0) {
	    //std::cout << "prev (" << this->previousColor.red << "," << this->previousColor.green << "," << this->previousColor.blue << "   /   next (" << this->nextColor.red << "," << this->nextColor.green << "," << this->nextColor.blue << ")\r\n";
	    //std::cout << "State: " << this->speckleStarted[i].state << ", RGB (" << this->leds[this->speckleStarted[i].index].red << "," << this->leds[this->speckleStarted[i].index].green << "," << this->leds[this->speckleStarted[i].index].blue << ")\r\n";
	  }

	} else {
	  this->speckleStarted.erase(this->speckleStarted.begin() + i);
	}
      }
  } else {
    this->state += 1;
    //std::cout << "No lights to fade, wait cycle " << this->state;
    if (this->state > this->solidCycles) {
      this->cycleColors();
      this->resetState();
    }
  }
}

int SpeckleColorMode::delayTime() {
  return 8;
}


SpeckleMultiColorMode::SpeckleMultiColorMode(RGB *leds, int numberOfLights) : SpeckleColorMode(leds, numberOfLights, 30, 50) {
}

void SpeckleMultiColorMode::cycleColors() {
  this->previousColor.copy(this->nextColor);
  switch (this->colorCount % 6) {
  case 0: this->nextColor.update(150, 150,0); break;
  case 1: this->nextColor.update(0,150,150); break;
  case 2: this->nextColor.update(0,0,200); break;
  case 3: this->nextColor.update(150,0,150); break;
  case 4: this->nextColor.update(200,0,0); break;
  case 5: this->nextColor.update(0,200,0); break;
  }
  this->colorCount += 1;
}
