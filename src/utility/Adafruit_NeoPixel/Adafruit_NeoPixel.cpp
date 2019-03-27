/*-------------------------------------------------------------------------
  Arduino library to control a wide variety of WS2811- and WS2812-based RGB
  LED devices such as Adafruit FLORA RGB Smart Pixels and NeoPixel strips.
  Currently handles 400 and 800 KHz bitstreams on 8, 12 and 16 MHz ATmega
  MCUs, with LEDs wired for various color orders.  Handles most output pins
  (possible exception with upper PORT registers on the Arduino Mega).

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
  contributions by PJRC, Michael Miller and other members of the open
  source community.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  -------------------------------------------------------------------------
  This file is part of the Adafruit NeoPixel library.

  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "Adafruit_NeoPixel.h"
#include "../../config/config.h"

// Constructor when length, pin and type are known at compile-time:
Adafruit_NeoPixel::Adafruit_NeoPixel(uint16_t n, uint8_t p, neoPixelType t) :
  begun(false), brightness(0), pixels(nullptr), endTime(0)
{
  updateType(t);
  updateLength(n);
  setPin(p);
}

// via Michael Vogt/neophob: empty constructor is used when strand length
// isn't known at compile-time; situations where program config might be
// read from internal flash memory or an SD card, or arrive via serial
// command.  If using this constructor, MUST follow up with updateType(),
// updateLength(), etc. to establish the strand type, length and pin number!
Adafruit_NeoPixel::Adafruit_NeoPixel() :
#ifdef NEO_KHZ400
  is800KHz(true),
#endif
  begun(false), numLEDs(0), numBytes(0), pin(-1), brightness(0), pixels(nullptr),
  rOffset(1), gOffset(0), bOffset(2), wOffset(1), endTime(0)
{
}

Adafruit_NeoPixel::~Adafruit_NeoPixel() {
  if(pixels)   gb_free(pixels);
#if !NO_ARDUINO
  if(pin >= 0) pinMode(pin, INPUT);
#endif
}

void Adafruit_NeoPixel::begin(void) {
  if(pin >= 0) {
#if NO_ARDUINO
    PORT->Group[0].DIR.reg |= (1ul << 13);
    PORT->Group[0].OUT.reg &= ~(1ul << 13);
#else
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
#endif
  }
  begun = true;
}

void Adafruit_NeoPixel::updateLength(uint16_t n) {
  if(pixels) gb_free(pixels); // Free existing data (if any)

  // Allocate new data -- note: ALL PIXELS ARE CLEARED
  numBytes = n * ((wOffset == rOffset) ? 3 : 4);
  if((pixels = (uint8_t *)gb_malloc(numBytes))) {
    memset(pixels, 0, numBytes);
    numLEDs = n;
  } else {
    numLEDs = numBytes = 0;
  }
}

void Adafruit_NeoPixel::updateType(neoPixelType t) {
  bool oldThreeBytesPerPixel = (wOffset == rOffset); // false if RGBW

  wOffset = (t >> 6) & 0b11; // See notes in header file
  rOffset = (t >> 4) & 0b11; // regarding R/G/B/W offsets
  gOffset = (t >> 2) & 0b11;
  bOffset =  t       & 0b11;
#ifdef NEO_KHZ400
  is800KHz = (t < 256);      // 400 KHz flag is 1<<8
#endif

  // If bytes-per-pixel has changed (and pixel data was previously
  // allocated), re-allocate to new size.  Will clear any data.
  if(pixels) {
    bool newThreeBytesPerPixel = (wOffset == rOffset);
    if(newThreeBytesPerPixel != oldThreeBytesPerPixel) updateLength(numLEDs);
  }
}

#ifdef ESP8266
// ESP8266 show() is external to enforce ICACHE_RAM_ATTR execution
extern "C" void ICACHE_RAM_ATTR espShow(
  uint8_t pin, uint8_t *pixels, uint32_t numBytes, uint8_t type);
#endif // ESP8266

#if NO_ARDUINO
#pragma GCC push_options
#pragma GCC optimize ("Os")
#endif

void Adafruit_NeoPixel::show(void) {
  if(!pixels) return;

  // Data latch = 50+ microsecond pause in the output stream.  Rather than
  // put a delay at the end of the function, the ending time is noted and
  // the function will simply hold off (if needed) on issuing the
  // subsequent round of data until the latch time has elapsed.  This
  // allows the mainline code to start generating the next frame of data
  // rather than stalling for the latch.
  while(!canShow());
  // endTime is a private member (rather than global var) so that mutliple
  // instances on different pins can be quickly issued in succession (each
  // instance doesn't delay the next).

  // In order to make this code runtime-configurable to work with any pin,
  // SBI/CBI instructions are eschewed in favor of full PORT writes via the
  // OUT or ST instructions.  It relies on two facts: that peripheral
  // functions (such as PWM) take precedence on output pins, so our PORT-
  // wide writes won't interfere, and that interrupts are globally disabled
  // while data is being issued to the LEDs, so no other code will be
  // accessing the PORT.  The code takes an initial 'snapshot' of the PORT
  // state, computes 'pin high' and 'pin low' values, and writes these back
  // to the PORT register as needed.

#if NO_ARDUINO
  __disable_irq(); // Need 100% focus on instruction timing
#else
  noInterrupts();
#endif
#if NEOPIXELS_ALTERNATIVE_METHOD
  NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_DETERMINISTIC_Val;
#endif
  // Tried this with a timer/counter, couldn't quite get adequate
  // resolution.  So yay, you get a load of goofball NOPs...

  uint8_t  *ptr, *end, p, bitMask, portNum;
  uint32_t  pinMask;

#if NO_ARDUINO
  portNum =  0;
  pinMask =  1ul << 13;
#else
  portNum =  g_APinDescription[pin].ulPort;
  pinMask =  1ul << g_APinDescription[pin].ulPin;
#endif
  ptr     =  pixels;
  end     =  ptr + numBytes;
  p       = *ptr++;
  bitMask = 0x80;

  volatile uint32_t *set = &(PORT->Group[portNum].OUTSET.reg),
                    *clr = &(PORT->Group[portNum].OUTCLR.reg);

  for(;;) {
    *set = pinMask;
#if NEOPIXELS_ALTERNATIVE_METHOD
    asm("nop; nop;");
#else
    asm("nop; nop; nop; nop; nop; nop; nop; nop;");
#endif
    if(p & bitMask) {
#if NEOPIXELS_ALTERNATIVE_METHOD
      asm("nop; nop; nop; nop; nop; nop; nop;");
#else
      asm("nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop;");
#endif
      *clr = pinMask;
    } else {
      *clr = pinMask;
#if NEOPIXELS_ALTERNATIVE_METHOD
      asm("nop; nop;");
#else
      asm("nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop;");
#endif
    }
    if(bitMask >>= 1) {
#if NEOPIXELS_ALTERNATIVE_METHOD
      asm("nop; nop; nop; nop; nop;");
#else
      asm("nop; nop; nop; nop; nop; nop; nop; nop; nop;");
#endif
    } else {
#if NO_ARDUINO
      __enable_irq();
      __disable_irq();
#else
      interrupts();
      noInterrupts();
#endif
      if(ptr >= end) break;
      p       = *ptr++;
      bitMask = 0x80;
    }
  }

#if NEOPIXELS_ALTERNATIVE_METHOD
  NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_NO_MISS_PENALTY_Val;
#endif
#if NO_ARDUINO
  __enable_irq();
#else
  interrupts();
#endif
  endTime = micros(); // Save EOD time for latch on next call
}
#if NO_ARDUINO
#pragma GCC pop_options
#endif

// Set the output pin number
void Adafruit_NeoPixel::setPin(uint8_t p) {
#if !NO_ARDUINO
  if(begun && (pin >= 0)) pinMode(pin, INPUT);
#endif
    pin = p;
    if(begun) {
#if NO_ARDUINO
      PORT->Group[0].DIR.reg |= (1ul << 13);
      PORT->Group[0].OUT.reg &= ~(1ul << 13);
#else
      pinMode(p, OUTPUT);
      digitalWrite(p, LOW);
#endif
    }
#ifdef __AVR__
    port    = portOutputRegister(digitalPinToPort(p));
    pinMask = digitalPinToBitMask(p);
#endif
}

// Set pixel color from separate R,G,B components:
void Adafruit_NeoPixel::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < numLEDs) {
    if(brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    uint8_t *p;
    if(wOffset == rOffset) { // Is an RGB-type strip
      p = &pixels[n * 3];    // 3 bytes per pixel
    } else {                 // Is a WRGB-type strip
      p = &pixels[n * 4];    // 4 bytes per pixel
      p[wOffset] = 0;        // But only R,G,B passed -- set W to 0
    }
    p[rOffset] = r;          // R,G,B always stored
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

void Adafruit_NeoPixel::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  if(n < numLEDs) {
    if(brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
      w = (w * brightness) >> 8;
    }
    uint8_t *p;
    if(wOffset == rOffset) { // Is an RGB-type strip
      p = &pixels[n * 3];    // 3 bytes per pixel (ignore W)
    } else {                 // Is a WRGB-type strip
      p = &pixels[n * 4];    // 4 bytes per pixel
      p[wOffset] = w;        // Store W
    }
    p[rOffset] = r;          // Store R,G,B
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

// Set pixel color from 'packed' 32-bit RGB color:
void Adafruit_NeoPixel::setPixelColor(uint16_t n, uint32_t c) {
  if(n < numLEDs) {
    uint8_t *p,
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
    if(brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    if(wOffset == rOffset) {
      p = &pixels[n * 3];
    } else {
      p = &pixels[n * 4];
      uint8_t w = (uint8_t)(c >> 24);
      p[wOffset] = brightness ? ((w * brightness) >> 8) : w;
    }
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t Adafruit_NeoPixel::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

// Convert separate R,G,B,W into packed 32-bit WRGB color.
// Packed format is always WRGB, regardless of LED strand color order.
uint32_t Adafruit_NeoPixel::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t Adafruit_NeoPixel::getPixelColor(uint16_t n) const {
  if(n >= numLEDs) return 0; // Out of bounds, return no color.

  uint8_t *p;

  if(wOffset == rOffset) { // Is RGB-type device
    p = &pixels[n * 3];
    if(brightness) {
      // Stored color was decimated by setBrightness().  Returned value
      // attempts to scale back to an approximation of the original 24-bit
      // value used when setting the pixel color, but there will always be
      // some error -- those bits are simply gone.  Issue is most
      // pronounced at low brightness levels.
      return (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
             (((uint32_t)(p[gOffset] << 8) / brightness) <<  8) |
             ( (uint32_t)(p[bOffset] << 8) / brightness       );
    } else {
      // No brightness adjustment has been made -- return 'raw' color
      return ((uint32_t)p[rOffset] << 16) |
             ((uint32_t)p[gOffset] <<  8) |
              (uint32_t)p[bOffset];
    }
  } else {                 // Is RGBW-type device
    p = &pixels[n * 4];
    if(brightness) { // Return scaled color
      return (((uint32_t)(p[wOffset] << 8) / brightness) << 24) |
             (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
             (((uint32_t)(p[gOffset] << 8) / brightness) <<  8) |
             ( (uint32_t)(p[bOffset] << 8) / brightness       );
    } else { // Return raw color
      return ((uint32_t)p[wOffset] << 24) |
             ((uint32_t)p[rOffset] << 16) |
             ((uint32_t)p[gOffset] <<  8) |
              (uint32_t)p[bOffset];
    }
  }
}

// Returns pointer to pixels[] array.  Pixel data is stored in device-
// native format and is not translated here.  Application will need to be
// aware of specific pixel data format and handle colors appropriately.
uint8_t *Adafruit_NeoPixel::getPixels(void) const {
  return pixels;
}

uint16_t Adafruit_NeoPixel::numPixels(void) const {
  return numLEDs;
}

// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.  C'est la vie.
void Adafruit_NeoPixel::setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  if (!pixels) {
    return;
  }
  uint8_t newBrightness = b + 1;
  if(newBrightness != brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    uint8_t  c,
            *ptr           = pixels,
             oldBrightness = brightness - 1; // De-wrap old brightness value
    uint16_t scale;
    if(oldBrightness == 0) scale = 0; // Avoid /0
    else if(b == 255) scale = 65535 / oldBrightness;
    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
    for(uint16_t i=0; i<numBytes; i++) {
      c      = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    brightness = newBrightness;
  }
}

//Return the brightness value
uint8_t Adafruit_NeoPixel::getBrightness(void) const {
  return brightness - 1;
}

void Adafruit_NeoPixel::clear() {
  if (pixels) {
    memset(pixels, 0, numBytes);
  }
}
