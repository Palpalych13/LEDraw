#ifndef effects_ppMod_h
#define effects_ppMod_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_2d.h"

extern const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {
    0x000000, 0x060707, 0x101110, 0x151717,
    0x1C1D22, 0x242A28, 0x363B3A, 0x313634,
    0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1,
    0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9};

// ***** FALLS / ПАДЕНИЕ *****
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020

// ============= Waterfall 3D/Firefall 3D/Liguid Crystall 3D/Fluff 3D ===============
// =========== Водопад 3D/Огненая стена 3D/Жидкий кристалл 3D/Мишура 3D =============
// Effects Mod for GyverLamp (FastLED) by Сотнег & PalPalych
const uint8_t _cooling = 448 / HEIGHT + 16;

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKINGNEW 80U // 50 // 30 // 120 // 90 // 60

bool waterfall4in1Routine(uint8_t Scale = effect.scale_real)
{ // <- поменять на имя заменяемогоо эффекта
  //void waterfall4in1Routine() { // <- ******* для оригинальной прошивки Gunner47 *******
  //uint8_t scale = effects[EFF_WHITTE_FIRE].Scale;
  //uint8_t scale = eff_scale;
  uint8_t COOLINGNEW = (Scale < 50) ? constrain((uint16_t)(Scale % 16) * 32 / HEIGHT + 16, 1, 255) : _cooling;
  // Array of temperature readings at each simulation cell
  static byte heat[WIDTH][HEIGHT];

  for (uint8_t x = 0; x < WIDTH; x++)
  {
    // Step 1.  Cool down every cell a little
    for (uint8_t i = 0; i < HEIGHT; i++)
    {
      //heat[x][i] = qsub8(heat[x][i], random8(0, ((COOLINGNEW * 10) / HEIGHT) + 2));
      heat[x][i] = qsub8(heat[x][i], random8(0, COOLINGNEW));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (uint8_t k = HEIGHT_d; k >= 2; k--)
    {
      heat[x][k] = (heat[x][k - 1] + heat[x][k - 2] + heat[x][k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKINGNEW)
    {
      uint8_t y = random8(2);
      heat[x][y] = qadd8(heat[x][y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (uint8_t j = 0; j < HEIGHT; j++)
    {
      byte colorindex = scale8(heat[x][j], 240);
      if (Scale < 16)
      { // Waterfall 3D
        leds[getPixelNumber(x, (HEIGHT_d)-j)] = ColorFromPalette(WaterfallColors_p, colorindex);
      }
      else if (Scale < 32)
      { // Firefall 3D
        leds[getPixelNumber(x, (HEIGHT_d)-j)] = ColorFromPalette(HeatColors_p, colorindex);
      }
      else if (Scale < 50)
      { // Rainbowfall 3D
        leds[getPixelNumber(x, (HEIGHT_d)-j)] = ColorFromPalette(RainbowColors_p, colorindex);
      }
      else
      { // Fluff 3D
        //uint8_t scl = constrain((effects[EFF_WHITTE_FIRE].Scale - 50) * 5.1, 0 , 255);
        uint8_t scl = constrain((Scale - 50) * 5.1, 0, 255);
        leds[getPixelNumber(x, (HEIGHT_d)-j)] = ColorFromPalette(CRGBPalette16(CRGB::Black, CHSV(scl, 255U, 255U), CHSV(scl, 128U, 255U), CRGB::White), colorindex);
      }
    }
  }
  return true;
}

// ***** NEW FIRE / НОВЫЙ ОГОНЬ ***** by PalPalych
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020

#define FIRE_WHITEVALUE (100U) // значение регулятора "Масштаб" при котором отображается белое пламя #100U
#define FIRE_OFFSET (4U)       // сдвиг маски огня относительно края. для того, чтобы разместить очаг по центру = FIRE_FLAMEWIDTH/2 #4U

#define FIRE_SMOOTH (3U)        /* сглаживание за счет увеличения промежуточных кадров (0...255, 0 - выключено), #3U \
                                   зависимые параметры: FIRE_CORESTRENGTH, FIRE_CORERESIST, FIRE_FLAMESTRENGTH,FIRE_CORERESIST,                           \
                                   FIRE_BREATHSPEED, FIRE_SPARKDELAY                                                                                                        \
*/
#define FIRE_CORESHIFT (8U)     // допустимый сдвиг очага в пикселях относительно его центральной позиции #4U
#define FIRE_CORESTRENGTH (36U) // устойчивость очага от сдвига по строкам (1...255 , 0 = отключить колыхание) #12U
#define FIRE_CORERESIST (16U)   // устойчивость очага от сдвига по кадрам (1...255) #2U
#define FIRE_COREAMPLITUDE (1U) // амплитуда сдвига очага (1...FIRE_COREWIDTH/2) #1U
#define FIRE_CORESEED (16U)     // величина зерна огня #16U

#define FIRE_FLAMESTRENGTH (8U)  // устойчивость пламени от сдвига по строкам (1...255 , 0 = отключить колыхание) #2U
#define FIRE_FLAMERESIST (3U)    // устойчивость пламени от сдвига по кадрам (1...255) #1U
#define FIRE_FLAMEAMPLITUDE (1U) // амплитуда сдвига пламени (1...FIRE_FLAMEWIDTH/4) #1U
#define FIRE_FLAMESEED (4U)      // величина зерна оттенка соседних пикселей огня #8U

#define FIRE_COREEDGE (24U) // порог срабатывания от e_nfir_hueMask с градиентом +FIRE_FLAMESEED для ообозначения сердцевины огня (1...255 , 0 = отключить) #1U
#define FIRE_COREDEEP (64U) // глубина белизны сердцевины огня #64U

#define FIRE_FLAMECOEFF (4U)      // делитель фактов поворота зеркала для некоторых режимов FIRE_FLAMEMIRROR
#define FIRE_FLAMEMIRROR (201U)   /* отзеркаливание маски оттенка очага #201U                                                       \
                                     0...99 - столб пламени, без смещения оттенка по строкам,                               \
                                       0 - выключение отзеркливания / столб пламени                                             \
                                       1...49 - шаговая диффузия с амплитудой FIRE_FLAMEAMPLITUDE,                                       \
                                       50...99 - случайная диффузия с амплитудой FIRE_FLAMEAMPLITUDE,                                  \
                                     101...199 - колышащее пламя, с учетом смещения оттенка по строкам,               \
                                       100 - выключение отзеркливания / колышащее пламя (базовое отключение) \
                                       101...149 - диффузия со смещением FIRE_FLAMEAMPLITUDE,                                                   \
                                       150...199 - случайная диффузия со смещением FIRE_FLAMEAMPLITUDE,                                \
                                     200...255 - колышащее пламя для каждого очага                                                    \
                                       200 - зеркальное для каждого второго очага                                                  \
                                       201 - колышащее пламя для каждого очага                                                        \
                                       202...254 - случайное отзеркаливание со скоростью переключения,               \
*/
#define FIRE_PLUMEHEIGHTMIN (3U)  // высота языков пламени минимальная, влияет на визуальную скорость подъема пламени (0...255) #3U
#define FIRE_PLUMEHEIGHTMAX (9U)  // высота языков пламени максимальная (FIRE_PLUMEHEIGHTMIN...255) #9U
#define FIRE_PLUMEWIDTHMIN (1U)   // ширина языков пламени минимальная (0...255) #31U
#define FIRE_PLUMEWIDTHMAX (4U)   // ширина языков пламени максимальная (FIRE_PLUMEWIDTHMIN...255) #31U
#define FIRE_PLUMELIGHTMIN (192U) // минимальная яркость темной части языка пламени (0...255) #160U
#define FIRE_PLUMELIGHTMAX (224U) // максимальная яркость темной части языка пламени (0...255) #192U

#define FIRE_BREATHDEEP (64U) // дыхание пламени (0...255) 0 - выключено  #64U
#define FIRE_BREATHSPEED (2U) // скорость дыхание пламени за 1 кадр (0...255) #4U

#define FIRE_SPARKLES (true)     // вылетающие искры вкл/выкл
#define FIRE_SPARKSBRIGHT (128U) // первоначальная яркость искр (255...0) #200U
#define FIRE_EVERYNSPARK (48U)   // только каждая единичная из указанных искр будет выводится (0...255) #64U
#define FIRE_SPARKSTRENGTH (3U)  // стойкость искр относительно напора ветра (1... , 0 = отключить колыхание) #3U
#define FIRE_SPARKBORNDEEP (3U)  // глубина от края пламени, на которой зарождаются искры #3U
#define FIRE_SPARKDELAY (4U)     // замедлитель скорости искр #2U

//#define FIRE_WINDCASE             (2U)      // случайные порывы ветра каждый 256 кадр (1...255 , 0 = отключить)  #2U

// ----- неперемещаемые/неизменные оперделения -----
#define FIRE_COREWIDTH (8U)  // ширина очага (маски)
#define FIRE_FLAMEWIDTH (8U) // ширина пламени (маски)
#define FIRE_HEIGHT (9U)     // высота очага (маски)

//these values are substracetd from the generated values to give a shape to the animation
#if ((FIRE_COREWIDTH == 16U) && (FIRE_HEIGHT == 8U))
static const uint8_t e_nfir_valueMask[FIRE_HEIGHT][FIRE_COREWIDTH] PROGMEM = {
    {32, 0, 0, 16, 16, 0, 0, 32, 32, 0, 0, 16, 16, 0, 0, 32},
    {64, 0, 0, 8, 8, 0, 0, 64, 64, 0, 0, 8, 8, 0, 0, 64},
    {96, 32, 0, 0, 0, 0, 32, 96, 96, 32, 0, 0, 0, 0, 32, 96},
    {128, 64, 32, 0, 0, 32, 64, 128, 128, 64, 32, 0, 0, 32, 64, 128},
    {160, 96, 64, 32, 32, 64, 96, 160, 160, 96, 64, 32, 32, 64, 96, 160},
    {192, 128, 96, 64, 64, 96, 128, 192, 192, 128, 96, 64, 64, 96, 128, 192},
    {255, 160, 128, 96, 96, 128, 160, 255, 255, 160, 128, 96, 96, 128, 160, 255},
    {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}};
#elif ((FIRE_COREWIDTH == 8U) && (FIRE_HEIGHT == 8U))
static const uint8_t e_nfir_valueMask[FIRE_HEIGHT][FIRE_COREWIDTH] PROGMEM = {
    {32, 0, 0, 16, 16, 0, 0, 32},
    {64, 8, 0, 8, 8, 0, 8, 64},
    {96, 32, 0, 0, 0, 0, 32, 96},
    {128, 64, 32, 0, 0, 32, 64, 128},
    {160, 96, 64, 32, 32, 64, 96, 160},
    {192, 128, 96, 64, 64, 96, 128, 192},
    {255, 160, 128, 96, 96, 128, 160, 255},
    {255, 192, 160, 128, 128, 160, 192, 255}};
#elif ((FIRE_COREWIDTH == 8U) && (FIRE_HEIGHT == 9U)) //PalPalych's Optimal Mod for 16x16
static const uint8_t e_nfir_valueMask[FIRE_HEIGHT][FIRE_COREWIDTH] PROGMEM = {
    {16, 0, 8, 16, 16, 8, 0, 16},
    {32, 0, 0, 16, 16, 0, 0, 32},
    {64, 8, 0, 8, 8, 0, 8, 64},
    {96, 32, 0, 0, 0, 0, 32, 96},
    {128, 64, 32, 0, 0, 32, 64, 128},
    {160, 96, 64, 32, 32, 64, 96, 160},
    {192, 128, 96, 64, 64, 96, 128, 192},
    {255, 160, 128, 96, 96, 128, 160, 255},
    {255, 240, 192, 160, 160, 192, 240, 255}};
#elif ((FIRE_COREWIDTH == 8U) && (FIRE_HEIGHT == 10U))
static const uint8_t e_nfir_valueMask[FIRE_HEIGHT][FIRE_COREWIDTH] PROGMEM = {
    {24, 0, 8, 16, 16, 8, 0, 24},
    {32, 4, 0, 12, 12, 0, 4, 32},
    {64, 8, 0, 8, 8, 0, 8, 64},
    {96, 32, 0, 0, 0, 0, 32, 96},
    {128, 64, 32, 0, 0, 32, 64, 128},
    {160, 96, 64, 32, 32, 64, 96, 160},
    {192, 128, 96, 64, 64, 96, 128, 192},
    {255, 160, 128, 96, 96, 128, 160, 255},
    {255, 192, 160, 128, 128, 160, 192, 255},
    {255, 240, 192, 160, 160, 192, 240, 255}};
#else
#pragma message "Wrong value mask parameters!"
#endif

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
#if ((FIRE_FLAMEWIDTH == 16U) && (FIRE_HEIGHT == 8U))
static const uint8_t e_nfir_hueMask[FIRE_HEIGHT][FIRE_FLAMEWIDTH] PROGMEM = {
    {1, 11, 19, 25, 25, 22, 11, 1, 1, 11, 19, 25, 25, 22, 11, 1},
    {1, 8, 13, 19, 25, 19, 8, 1, 1, 8, 13, 19, 25, 19, 8, 1},
    {1, 8, 13, 16, 19, 16, 8, 1, 1, 8, 13, 16, 19, 16, 8, 1},
    {1, 5, 11, 13, 13, 13, 5, 1, 1, 5, 11, 13, 13, 13, 5, 1},
    {1, 5, 11, 11, 11, 11, 5, 1, 1, 5, 11, 11, 11, 11, 5, 1},
    {0, 1, 5, 8, 8, 5, 1, 0, 0, 1, 5, 8, 8, 5, 1, 0},
    {0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}};
#elif ((FIRE_FLAMEWIDTH == 8U) && (FIRE_HEIGHT == 8U))
static const uint8_t e_nfir_hueMask[FIRE_HEIGHT][FIRE_FLAMEWIDTH] PROGMEM = {
    {1, 11, 19, 25, 25, 22, 11, 1},
    {1, 8, 13, 19, 25, 19, 8, 1},
    {1, 8, 13, 16, 19, 16, 8, 1},
    {1, 5, 11, 13, 13, 13, 5, 1},
    {1, 5, 11, 11, 11, 11, 5, 1},
    {0, 1, 5, 8, 8, 5, 1, 0},
    {0, 0, 1, 5, 5, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}};
#elif ((FIRE_FLAMEWIDTH == 8U) && (FIRE_HEIGHT == 9U)) //PalPalych's Optimal Mod for 16x16
static const uint8_t e_nfir_hueMask[FIRE_HEIGHT][FIRE_FLAMEWIDTH] PROGMEM = {
    {7, 21, 25, 26, 26, 25, 21, 7},
    {3, 17, 24, 26, 26, 24, 17, 3},
    {2, 13, 21, 25, 25, 21, 13, 2},
    {1, 11, 18, 23, 23, 18, 11, 1},
    {1, 9, 14, 20, 20, 14, 9, 1},
    {1, 5, 10, 16, 16, 10, 5, 1},
    {0, 2, 7, 11, 11, 7, 2, 0},
    {0, 1, 3, 6, 6, 3, 1, 0},
    {0, 0, 1, 2, 2, 1, 0, 0}};
#elif ((FIRE_FLAMEWIDTH == 8U) && (FIRE_HEIGHT == 10U))
static const uint8_t e_nfir_hueMask[FIRE_HEIGHT][FIRE_FLAMEWIDTH] PROGMEM = {
    {5, 16, 25, 26, 26, 25, 16, 5},
    {1, 11, 22, 26, 26, 22, 11, 1},
    {1, 8, 19, 25, 25, 19, 8, 1},
    {1, 8, 13, 19, 19, 16, 8, 1},
    {1, 5, 11, 13, 13, 13, 5, 1},
    {1, 5, 11, 11, 11, 11, 5, 1},
    {0, 1, 5, 8, 8, 5, 1, 0},
    {0, 0, 3, 6, 6, 3, 0, 0},
    {0, 0, 1, 5, 5, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}};
#else
#pragma message "Wrong hue mask parameters!"
#endif

const uint8_t _coreshiftmin = FIRE_FLAMEWIDTH - FIRE_CORESHIFT;
const uint8_t _coreshiftmax = FIRE_FLAMEWIDTH + FIRE_CORESHIFT;
const uint8_t _flameshiftmin = FIRE_FLAMEWIDTH - FIRE_FLAMEAMPLITUDE;
const uint8_t _flameshiftmax = FIRE_FLAMEWIDTH + FIRE_FLAMEAMPLITUDE;
const uint8_t _corestrength = FIRE_CORESTRENGTH + 1U;
const uint8_t _coreresist = FIRE_CORERESIST + 1U;
const uint8_t _coreamplitude = FIRE_COREAMPLITUDE + 1U;
const uint8_t _flamestrength = FIRE_FLAMESTRENGTH + 1U;
const uint8_t _flameresist = FIRE_FLAMERESIST + 1U;
const uint8_t _flameamplitude = FIRE_FLAMEAMPLITUDE + 1U;
const uint8_t _sparkstrength = FIRE_SPARKSTRENGTH + 1U;
const uint8_t _flameseed = FIRE_FLAMESEED;
const uint8_t _coreoffset = FIRE_COREWIDTH + FIRE_OFFSET;
const uint8_t _nofmasks = WIDTH / FIRE_COREWIDTH + (WIDTH % FIRE_COREWIDTH) ? 2U : 1U;
const uint8_t _iseven = (FIRE_HEIGHT & 2U) ? 0U : 1U;
const uint8_t _sparkdelay = FIRE_SPARKDELAY - 1U;
const uint8_t _plumestep = FIRE_HEIGHT + 1U;

uint8_t shiftValue[FIRE_HEIGHT]; // массив дороожки горизонтального смещения пламени (hueValue)

#if FIRE_FLAMEMIRROR       // ***** FLAMEMIRROR Инициализация *****
#if FIRE_FLAMEMIRROR < 50U // FLAMEMIRROR = 1...49
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
const uint8_t _flamecount = FIRE_FLAMEMIRROR;
#elif FIRE_FLAMEMIRROR < 100U // FLAMEMIRROR = 50...99
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
const uint8_t _flamemirror = (FIRE_FLAMEMIRROR - 50U) * 2U;
const uint8_t _flamecount = (_flamemirror / FIRE_FLAMECOEFF) + 1U;
#elif FIRE_FLAMEMIRROR < 101U // FLAMEMIRROR = 100
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
uint8_t shiftHue[FIRE_HEIGHT] = {_shiftHue};
#elif FIRE_FLAMEMIRROR < 150U // FLAMEMIRROR = 101...149
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
const uint8_t _flamecount = FIRE_FLAMEMIRROR - 100U;
uint8_t shiftHue[FIRE_HEIGHT] = {_shiftHue}; // массив дороожки горизонтального смещения пламени (e_nfir_hueMask)
#elif FIRE_FLAMEMIRROR < 200U // FLAMEMIRROR = 150...199
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
const uint8_t _flamemirror = (FIRE_FLAMEMIRROR - 150U) * 2U;
const uint8_t _flamecount = (_flamemirror / FIRE_FLAMECOEFF) + 1U;
uint8_t shiftHue[FIRE_HEIGHT] = {_shiftHue}; // массив дороожки горизонтального смещения пламени (e_nfir_hueMask)
#elif FIRE_FLAMEMIRROR < 201U // FLAMEMIRROR = 200
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
uint8_t shiftHue[FIRE_HEIGHT] = {_shiftHue}; // массив дороожки горизонтального смещения пламени (e_nfir_hueMask)
#elif FIRE_FLAMEMIRROR < 202U // FLAMEMIRROR = 201
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
uint8_t shiftHue[FIRE_HEIGHT][_nofmasks]; // массив дороожки горизонтального смещения пламени (e_nfir_hueMask)
#else
uint8_t _shiftHue = FIRE_FLAMEWIDTH;
const uint8_t _flamemirror = (FIRE_FLAMEMIRROR - 201U) * 2U;
const uint8_t _flamecount = (_flamemirror / FIRE_FLAMECOEFF) + 1U;
uint8_t shiftHue[FIRE_HEIGHT][_nofmasks]; // массив дороожки горизонтального смещения пламени (e_nfir_hueMask)
#endif
#endif

uint8_t e_nfir_matrixValue[FIRE_HEIGHT + 2U][WIDTH];
uint16_t _rowcounter;
uint16_t _framecounter;
bool _mirror;
uint8_t _mirrorstate[_nofmasks];
#if FIRE_SMOOTH
uint8_t _smooth = FIRE_SMOOTH;
uint8_t _smoothstep = 256.0 / FIRE_SMOOTH;
#endif
uint8_t _seedstep = 256.0 / _flameseed;
uint8_t _sdelay = 128U;
uint8_t _darkness;
uint8_t _value;
uint8_t _windspeed = WIDTH;

#if FIRE_FLAMEMIRROR < 201U // FLAMEMIRROR = 201
uint8_t _briefValue;
uint8_t _briefNewValue;
#else
uint8_t _briefValue[_nofmasks];
uint8_t _briefNewValue[_nofmasks];
#endif

const uint8_t _plumeinc_min = (255.0 - FIRE_PLUMELIGHTMAX) / (float)FIRE_PLUMEHEIGHTMIN;
const uint8_t _plumeinc_max = (255.0 - FIRE_PLUMELIGHTMIN) / (float)FIRE_PLUMEHEIGHTMAX;
const uint8_t _plumedec_min = (255.0 - FIRE_PLUMELIGHTMAX) / 2.0;
const uint8_t _plumedec_max = (255.0 - FIRE_PLUMELIGHTMIN) / 2.0;

void fire_shiftUp(); //прототип

bool newFireRoutine(uint8_t Scale = effect.scale_real)
{ // прорисовка нового кадра
  uint8_t baseHue = qsub8((float)Scale * 2.57, _flameseed);
  uint8_t baseSat = (Scale == FIRE_WHITEVALUE) ? 0U : 255U; // отключение насыщенности при определенном положении колеса

#if FIRE_SMOOTH
  uint8_t _framemod = _framecounter % FIRE_SMOOTH;
  if (!_framemod)
    fire_shiftUp();
#else
  fire_shiftUp();
#endif

#if FIRE_BREATHSPEED
  for (uint8_t i = 0; i < _nofmasks; i++)
  {
    if (_briefValue[i] == _briefNewValue[i])
    {
      _briefNewValue[i] = random(FIRE_BREATHDEEP);
    }
    else
    {
      _briefValue[i] = (_briefValue[i] < _briefNewValue[i])
                           ? min(qadd8(_briefValue[i], FIRE_BREATHSPEED), _briefNewValue[i])
                           : max(qsub8(_briefValue[i], FIRE_BREATHSPEED), _briefNewValue[i]);
    }
  }
#else
  if (_briefValue == _briefNewValue)
  {
    _briefNewValue = random(FIRE_BREATHDEEP);
  }
  else
  {
    _briefValue = (_briefValue < _briefNewValue)
                      ? min(qadd8(_briefValue, FIRE_BREATHSPEED), _briefNewValue)
                      : max(qsub8(_briefValue, FIRE_BREATHSPEED), _briefNewValue);
  }
#endif

#if FIRE_FLAMEMIRROR       // ***** HUE_mirror *****
#if FIRE_FLAMEMIRROR < 50U // _mirror = 1...49
  if (!(_rowcounter % _flamecount) && (_rowcounter % FIRE_HEIGHT))
    _mirror = !_mirror;
#elif FIRE_FLAMEMIRROR < 100U // _mirror = 50...99
  if (!random8(_flamemirror) && !(_rowcounter % _flamecount))
  {
    _mirror = !_mirror;
  }
#elif FIRE_FLAMEMIRROR < 101U
  _shiftHue = shiftHue[0];
#elif FIRE_FLAMEMIRROR < 150U // _mirror = 100...149
  if (!(_rowcounter % _flamecount) && (_rowcounter % FIRE_HEIGHT))
    _mirror = !_mirror;
  _shiftHue = shiftHue[0];
#elif FIRE_FLAMEMIRROR < 200U // _mirror = 150...199
  if (!random8(_flamemirror) && !(_rowcounter % _flamecount))
  {
    _mirror = !_mirror;
  }
  _shiftHue = shiftHue[0];
#elif FIRE_FLAMEMIRROR < 201U // _mirror = 200
  _shiftHue = shiftHue[0];
#elif FIRE_FLAMEMIRROR < 202U // _mirror = 201

#else // _mirror = 202...255
  if (!random8(_flamemirror) && !(_rowcounter % _flamecount))
  {
    _mirror = !_mirror;
  }
#endif
#endif

  //each row interpolates with the one before it
  for (uint8_t y = FIRE_HEIGHT - 1U; y < FIRE_HEIGHT; y--)
  {                        // Work остальных строк
#if FIRE_FLAMEMIRROR       // ***** HUE_mirror *****
#if FIRE_FLAMEMIRROR < 50U // _mirror = 1...49
    if (!(_rowcounter % _flamecount))
      _mirror = !_mirror;
#elif FIRE_FLAMEMIRROR < 100U // _mirror = 50...99
    if (!random8(_flamemirror) && !(_rowcounter % _flamecount))
      _mirror = !_mirror;
#elif FIRE_FLAMEMIRROR < 101U // _mirror = 100
    _shiftHue = shiftHue[y];
#elif FIRE_FLAMEMIRROR < 150U // _mirror = 101...149
    if (!(_rowcounter % FIRE_FLAMEMIRROR))
      _mirror = !_mirror;
    _shiftHue = shiftHue[y];
#elif FIRE_FLAMEMIRROR < 151U // _mirror = 150
    _shiftHue = shiftHue[y];
#elif FIRE_FLAMEMIRROR < 200U // _mirror = 151...199
    if (!random8(_flamemirror) && (_rowcounter % _flamecount))
      _mirror = !_mirror;
    _shiftHue = shiftHue[y];
#elif FIRE_FLAMEMIRROR < 201U // _mirror = 200
    _shiftHue = shiftHue[y];
#elif FIRE_FLAMEMIRROR < 202U // _mirror = 201
#else
    if (!random8(_flamemirror) && !(_rowcounter % _flamecount))
    {
      _mirror = !_mirror;
    }
#endif
#endif
    for (uint8_t x = 0U; x < WIDTH; x++)
    {                      // прорисовка пламени
#if FIRE_FLAMEMIRROR == 0U // ***** HUE_mirror *****
      uint8_t _offset = x + _coreoffset;
      uint8_t z = _offset % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 50U  // _mirror = 1...49
      uint8_t _offset = x + _coreoffset;
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 100U // _mirror = 50...99
      uint8_t _offset = x + _coreoffset;
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 101U // _mirror = 100
      //_shiftHue = shiftHue[y];
      uint8_t _offset = x + _coreoffset - _shiftHue;
      uint8_t z = _offset % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 150U // _mirror = 101...149
      //_shiftHue = shiftHue[y];
      uint8_t _offset = x + _coreoffset + shiftValue[y];
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 151U // _mirror = 150
      //_shiftHue = shiftHue[y];
      uint8_t _offset = x + _coreoffset + shiftValue[y];
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 200U // _mirror = 151...199
      uint8_t _offset = x + _coreoffset + shiftValue[y];
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 201U // _mirror = 200
      uint8_t _offset = x + _coreoffset + shiftValue[y];
      _mirror = (x / FIRE_FLAMEWIDTH) % 2U;
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#elif FIRE_FLAMEMIRROR < 202U // _mirror = 201
      uint8_t _offset = x + _coreoffset + shiftValue[y];
      _shiftHue = shiftHue[y][x / FIRE_FLAMEWIDTH];
      uint8_t z = qadd8(_offset, _shiftHue) % FIRE_FLAMEWIDTH;
#else                         // _mirror = 202...255
      uint8_t _offset = x + _coreoffset + shiftValue[y];
      _shiftHue = shiftHue[y][x / FIRE_FLAMEWIDTH];
      uint8_t z = (_mirror ? qsub8(_offset, _shiftHue) : qadd8(_offset, _shiftHue)) % FIRE_FLAMEWIDTH;
#endif

#if FIRE_BREATHSPEED
      _darkness = qadd8(_briefValue[x / FIRE_FLAMEWIDTH], random8(FIRE_CORESEED));
#else
      _darkness = qadd8(_briefValue, random8(FIRE_CORESEED));
#endif

#if FIRE_SMOOTH
      uint8_t _scale = qmul8(_smoothstep, _framemod);
      _value = qadd8(scale8(e_nfir_matrixValue[y + 1U][x], qsub8(255, _scale)), scale8(e_nfir_matrixValue[y][x], _scale));
#elif FIRE_SMOOTH == 1
      _value = avg8(e_nfir_matrixValue[y][x], e_nfir_matrixValue[y + 1U][x]);
#else
      _value = e_nfir_matrixValue[y + 1U][x];
#endif
      uint8_t _hue = qadd8(random8(_flameseed), pgm_read_byte(&e_nfir_hueMask[y][z]));
#if FIRE_COREEDGE
      uint8_t _white = qsub8(_hue, FIRE_COREEDGE);
      uint8_t _whiteness = qsub8(baseSat, scale8(FIRE_COREDEEP, qsub8(255, qmul8(_seedstep, _white))));
      uint8_t _saturation = _white ? _whiteness : baseSat;
#else
      uint8_t _saturation = baseSat;
#endif
      drawPixel(x, y, CHSV(                                                                                                  // определение цвета пикселя
                          qadd8(_hue, baseHue),                                                                              // H - смещение всполохов
                          _saturation,                                                                                       // S - когда колесо масштаба =100 - белый огонь (экономим на 1 эффекте)
                          qsub8(_value, qadd8(pgm_read_byte(&e_nfir_valueMask[y][_offset % FIRE_FLAMEWIDTH]), _darkness)))); // V
    }
    if (y)
    {
      shiftValue[y] = shiftValue[y - 1]; // подготавлеваем значение смешения для следующего кадра основываясь на предыдущем

#if FIRE_FLAMEMIRROR // ***** HUE_mirror *****
#if FIRE_FLAMEMIRROR < 50U
#elif FIRE_FLAMEMIRROR < 100U // _mirror = 51...99
#elif FIRE_FLAMEMIRROR < 200U // _mirror = 151...199
      shiftHue[y] = shiftHue[y - 1]; // подготавлеваем значение смешения для следующего кадра основываясь на предыдущем
#elif FIRE_FLAMEMIRROR < 201U // _mirror = 200
      shiftHue[y] = shiftHue[y - 1];
#elif FIRE_FLAMEMIRROR < 202U // _mirror = 201
      for (uint8_t i = 0; i < _nofmasks; i++)
      {
        shiftHue[y][i] = shiftHue[y - 1][i];
      }
#else
      for (uint8_t i = 0; i < _nofmasks; i++)
      {
        shiftHue[y][i] = shiftHue[y - 1][i];
      }
#endif
#endif
    }
    _rowcounter++;
  }
#if FIRE_CORESTRENGTH // смещение очага
  if (!random8(_corestrength) && !(_rowcounter % _coreresist))
  {
    shiftValue[0] = constrain(qsub8(qadd8(shiftValue[0], random8(_coreamplitude)), random8(_coreamplitude)), _coreshiftmin, _coreshiftmax);
  }
#endif

#if FIRE_FLAMEMIRROR // ***** HUE_mirror *****
#if FIRE_FLAMEMIRROR < 50U
#elif FIRE_FLAMEMIRROR < 100U // _mirror = 51...99
#elif FIRE_FLAMEMIRROR < 200U // _mirror = 151...199
  if (!random8(_flamestrength) && !(_rowcounter % _flameresist))
  {
    shiftHue[0] = constrain(qsub8(qadd8(shiftHue[0], random8(_flameamplitude)), random8(_flameamplitude)), _flameshiftmin, _flameshiftmax);
  }
#elif FIRE_FLAMEMIRROR < 201U // _mirror = 200
  if (!random8(_flamestrength) && !(_rowcounter % _flameresist))
  {
    shiftHue[0] = constrain(qsub8(qadd8(shiftHue[0], random8(_flameamplitude)), random8(_flameamplitude)), _flameshiftmin, _flameshiftmax);
  }
#elif FIRE_FLAMEMIRROR < 202U // _mirror = 201
  for (uint8_t i = 0; i < _nofmasks; i++)
  {
    if (!random8(_flamestrength) && !((_rowcounter + i) % _flameresist))
    {
      shiftHue[0][i] = constrain(qsub8(qadd8(shiftHue[0][i], random8(_flameamplitude)), random8(_flameamplitude)), _flameshiftmin, _flameshiftmax);
    }
  }
#else                         // _mirror = 202...255
  for (uint8_t i = 0; i < _nofmasks; i++)
  {
    if (!random8(_flamestrength) && !((_rowcounter + i) % _flameresist))
    {
      shiftHue[0][i] = constrain(qsub8(qadd8(shiftHue[0][i], random8(_flameamplitude)), random8(_flameamplitude)), _flameshiftmin, _flameshiftmax);
    }
  }
#endif
#endif

#if FIRE_SPARKLES // если это самая нижняя строка искр - формитуем искорку из пламени
  if (!(_framecounter % _sparkdelay))
  {
    for (uint8_t y = HEIGHT - 1U; y > FIRE_HEIGHT; y--)
    {
      for (uint8_t x = 0U; x < WIDTH; x++)
      { // стираем строчку с искрами (очень не оптимально)
        drawPixel(x, y, 0U);
      }
      for (uint8_t x = 0U; x < WIDTH; x++)
      {

        /*#if FIRE_WINDCASE  // TODO
                if  (!random8(_sparkstrength) && ((_framecounter >> 8) % FIRE_WINDCASE )) {
                  _windspeed = constrain(_windspeed + random8(2U) - random8(2U), WIDTH, WIDTH + 1U) % WIDTH;
                }
                _windspeed = 15U;
          #endif*/
#if FIRE_SPARKSTRENGTH
        uint8_t newX = (random8(_sparkstrength)) ? x : (x + WIDTH + random8(2U) - random8(2U)) % WIDTH; // с вероятностью 1/3 смещаем искорку влево или вправо
#else
        uint8_t newX = x;
#endif
        if (getPixelValue(x, y - 1U) > 0U)
          drawPixel(newX, y, getPixelColor(x, y - 1U)); // рисуем искорку на новой строчке
      }
    }
    for (uint8_t x = 0U; x < WIDTH; x++)
    { // если это не самая нижняя строка искр - перемещаем искорку выше
      if (random8(FIRE_EVERYNSPARK) == 0 && getPixelValue(x, FIRE_HEIGHT - 1U) != 0U)
      {
        uint16_t px = getPixelNumber(x, FIRE_HEIGHT);
        leds[px] = getPixelColor(x, FIRE_HEIGHT - FIRE_SPARKBORNDEEP);
        leds[px] %= FIRE_SPARKSBRIGHT;
      }
      else
        drawPixel(x, FIRE_HEIGHT, 0U);
    }
  }
#endif
  if (_rowcounter > 65521U)
  {
    _rowcounter = _rowcounter % 65521 + _iseven;
  }
  _framecounter++;
  return true;
}

void fire_shiftUp()
{
  for (uint8_t y = FIRE_HEIGHT; y > 0U; y--)
  {
    for (uint8_t x = 0U; x < WIDTH; x++)
    {
      e_nfir_matrixValue[y][x] = e_nfir_matrixValue[y - 1U][x];
    }
  }

  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    if (e_nfir_matrixValue[_plumestep][x])
    {
      e_nfir_matrixValue[0U][x] = (e_nfir_matrixValue[_plumestep][x] == 1U)
                                      ? qsub8(e_nfir_matrixValue[0U][x], random8(_plumedec_min, _plumedec_max))
                                      : qadd8(e_nfir_matrixValue[0U][x], random8(_plumeinc_min, _plumeinc_max));
      e_nfir_matrixValue[_plumestep][x]--;
    }
    else
    {
      uint8_t x1max = random8(FIRE_PLUMEWIDTHMIN, FIRE_PLUMEWIDTHMAX);
      uint8_t _plumeheight = random8(FIRE_PLUMEHEIGHTMIN, FIRE_PLUMEHEIGHTMAX);
      uint8_t mValue = random8(FIRE_PLUMELIGHTMIN, FIRE_PLUMELIGHTMAX);

      for (uint8_t x1 = 0U; x1 < x1max; x1++)
      {
        uint8_t x2 = (x + x1) % WIDTH;
        e_nfir_matrixValue[0U][x2] = ((x1 == 0) || (x1 == x1max)) ? qsub8(mValue, 24U) : mValue;
        e_nfir_matrixValue[_plumestep][x2] = _plumeheight;
      }
      x += x1max;
    }
  }
}

// ***** NOISE STREAMING / ШУМНЫЙ ПОТОК *****
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020
// --------------------------- NoiseSmearing(by StefanPetrick) Effect mod for GyverLamp by PalPalych  --------------------

//CRGB ledsbuff[NUM_LEDS]; // <--- раскоментить если не инициализирована ранее

bool multipleStreamRoutine()
{                 // 2 comets
  dimAll(192); // < -- затухание эффекта для последующего кадрв

  // gelb im Kreis
  byte xx = 2 + sin8(millis() / 10) / 22;
  byte yy = 2 + cos8(millis() / 10) / 22;
  leds[getPixelNumber(xx, yy)] = 0xFFFF00;

  // rot in einer Acht
  xx = 4 + sin8(millis() / 46) / 32;
  yy = 4 + cos8(millis() / 15) / 32;
  leds[getPixelNumber(xx, yy)] = 0xFF0000;

  // Noise
  noise32_x[0] += 3000;
  noise32_y[0] += 3000;
  noise32_z[0] += 3000;
  scale32_x[0] = 8000;
  scale32_y[0] = 8000;
  fillNoise3d(0);
  MoveFractionalNoiseX(3, 0.33);
  MoveFractionalNoiseY(3);
  return true;
}

bool multipleStreamRoutine2()
{                 // 3 comets
  dimAll(160); // < -- затухание эффекта для последующего кадрв
  byte xx = 2 + sin8(millis() / 10) / 22;
  byte yy = 2 + cos8(millis() / 9) / 22;
  leds[getPixelNumber(xx, yy)] += 0x0000FF;

  xx = 4 + sin8(millis() / 10) / 32;
  yy = 4 + cos8(millis() / 7) / 32;
  leds[getPixelNumber(xx, yy)] += 0xFF0000;
  leds[getPixelNumber(CENTERX_d, CENTERY_d)] += 0xFFFF00;

  noise32_x[0] += 3000;
  noise32_y[0] += 3000;
  noise32_z[0] += 3000;
  scale32_x[0] = 8000;
  scale32_y[0] = 8000;
  fillNoise3d(0);
  MoveFractionalNoiseX(2);
  MoveFractionalNoiseY(2, 0.33);
  return true;
}

bool multipleStreamRoutine3()
{                 // Fireline
  dimAll(160); // < -- затухание эффекта для последующего кадрв
  for (uint8_t i = 1; i < WIDTH; i += 3)
  {
    leds[getPixelNumber(i, CENTERY_d)] += CHSV(i * 2, 255, 255);
  }
  // Noise
  noise32_x[0] += 3000;
  noise32_y[0] += 3000;
  noise32_z[0] += 3000;
  scale32_x[0] = 8000;
  scale32_y[0] = 8000;
  fillNoise3d(0);
  MoveFractionalNoiseY(3);
  MoveFractionalNoiseX(3);
  return true;
}

bool multipleStreamRoutine4()
{                 // Comet
  dimAll(184); // < -- затухание эффекта для последующего кадрв
  CRGB _eNs_color = CHSV(millis(), 255, 255);
  leds[getPixelNumber(CENTERX_d, CENTERY_d)] += _eNs_color;
  // Noise
  noise32_x[0] += 2000;
  noise32_y[0] += 2000;
  noise32_z[0] += 2000;
  scale32_x[0] = 4000;
  scale32_y[0] = 4000;
  fillNoise3d(0);
  MoveFractionalNoiseX(6);
  MoveFractionalNoiseY(5, -0.5);
  return true;
}

bool multipleStreamRoutine5()
{                 // Fractorial Fire
  dimAll(140); // < -- затухание эффекта для последующего кадрв
  for (uint8_t i = 1; i < WIDTH; i += 2)
  {
    leds[getPixelNumber(i, WIDTH_d)] += CHSV(i * 2, 255, 128);
  }
  // Noise
  noise32_x[0] += 3000;
  noise32_y[0] += 3000;
  noise32_z[0] += 3000;
  scale32_x[0] = 8000;
  scale32_y[0] = 8000;
  fillNoise3d(0);
  //MoveX(1);
  //MoveY(1);
  MoveFractionalNoiseY(3, 1);
  MoveFractionalNoiseX(2);
  return true;
}

bool multipleStreamRoutine8()
{                // Windows ))
  dimAll(96); // < -- затухание эффекта для последующего кадрв на 96/255*100=37%
  for (uint8_t y = 2; y < HEIGHT; y += 5)
  {
    for (uint8_t x = 2; x < WIDTH; x += 5)
    {
      leds[getPixelNumber(x, y)] += CHSV(x * y, 255, 255);
      leds[getPixelNumber(x + 1, y)] += CHSV((x + 4) * y, 255, 255);
      leds[getPixelNumber(x, y + 1)] += CHSV(x * (y + 4), 255, 255);
      leds[getPixelNumber(x + 1, y + 1)] += CHSV((x + 4) * (y + 4), 255, 255);
    }
  }
  // Noise
  noise32_x[0] += 3000;
  noise32_y[0] += 3000;
  noise32_z[0] += 3000;
  scale32_x[0] = 8000;
  scale32_y[0] = 8000;
  fillNoise3d(0);
  MoveFractionalNoiseX(3);
  MoveFractionalNoiseY(3);
  return true;
}

bool noiseStreamingRoutine(uint8_t Scale = effect.scale_real)
{
  if (!eNs_isSetupped)
    noise_setup();
  uint8_t _selector = constrain(Scale / 16, 0, 5);
  //uint8_t _scale = constrain((uint8_t)scale % 17, 0, 15);

  if (_selector == 0)
    return multipleStreamRoutine();
  if (_selector == 1)
    return multipleStreamRoutine2();
  if (_selector == 2)
    return multipleStreamRoutine3();
  if (_selector == 3)
    return multipleStreamRoutine4(); // Rainbow comet
  if (_selector == 4)
    return multipleStreamRoutine5();

  return multipleStreamRoutine8();
}

// ***** RAINBOW COMET / РАДУЖНАЯ КОМЕТА *****
// v1.5.0 - Follow the Rainbow Comet / first release by PalPalych 29.02.2020
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020
// v1.1 - Scale & Speed correction, dither & blur 23.03.2020
// v1.2 - 0 Scale for Random 3d color 24.03.2020
// v1.3 - center spot is ellipce now 17.04.2020

/*
  Speed = tail dispersing
  Scale = 0 - Random 3d color
          1...127 - time depending color
          128...254 - selected color
          255 - white
*/
#define e_com_TAILSPEED (500)   // скорость смещения хвоста
#define e_com_BLUR (24U)        // размытие хвоста
#define e_com_DITHER (2U)       // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново) Влияет на скорость!
#define e_com_3DCOLORSPEED (5U) // скорость случайного изменения цвета (0й - режим)

bool rainbowCometRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if (e_com_DITHER)
  if (effect.frame_counter % e_com_DITHER)
    return false;
#endif
  blurAll(e_com_BLUR); // < -- размытие хвоста
  dimAll(254);         // < -- затухание эффекта для последующего кадра
  CRGB _eNs_color = CRGB::White;
  if (Scale < 1)
  {
    _eNs_color = CHSV(noise3d[0][0][4] * e_com_3DCOLORSPEED, 255, 255);
  }
  else if (Scale < 128)
  {
    _eNs_color = CHSV(millis() / (Scale + 1U) * 4, 255, 255);
  }
  else if (Scale < 255)
  {
    _eNs_color = CHSV((Scale - 128) * 2, 255, 255);
  }
  drawOval2_fast(CENTERX_d - 1, CENTERX_d + 2, CENTERY_d - 1, CENTERY_d + 2, _eNs_color);
  // Noise
  uint16_t sc = Speed * 64 + 1000;
  noise32_x[0] += e_com_TAILSPEED;
  noise32_y[0] += e_com_TAILSPEED;
  noise32_z[0] += e_com_TAILSPEED;
  scale32_x[0] = sc; // 8000;
  scale32_y[0] = sc; // 8000;
  fillNoise3d(0);
  MoveFractionalNoiseX(CENTERX_d);
  MoveFractionalNoiseY(CENTERY_d);
  return true;
}

// ***** SINUSOID 3 / СИНУСОИД 3 *****
/*  Sinusoid3 by Stefan Petrick (mod by Palpalych for GyverLamp 27/02/2020)
    read more about the concept: https://www.youtube.com/watch?v=mubH-w_gwdA */
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020
// v1.1 - trig16 logic by kDn
// v1.2 - Full integer logic, + inverse rings, + dithering by PalPalych 12.04.2020
// v1.3 - math correction & inverse by Scale -- PalPalych 13.04.2020

#define e_s3_BEATSINLOGIC (false) // логика beatsin8 = true / логика реального времени = false
#define e_s3_DITHER (2U)          // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново) Влияет на скорость!
const uint8_t _s3_xz = CENTERX * WIDTH_steps;
const uint8_t _s3_yz = CENTERY * HEIGHT_steps;
int16_t zx;
int16_t zy;
int16_t cx;
int16_t cy;
uint8_t _s3_scale;
uint32_t _s3_time;

bool sinusoid3Routine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if (e_s3_DITHER)
  if (effect.frame_counter % e_s3_DITHER)
    return false;
#endif
  bool _s3_inverse = Scale < 128U;
  if (!_s3_inverse)
    Scale = ~Scale;
  Scale *= 2U;

  _s3_scale = map8(Scale, HEIGHT_steps, HEIGHT_steps << 2U); // speed of the movement along the Lissajous curves
  Speed = map8(Speed, 31U, 255U);
#if e_s3_BEATSINLOGIC
  _s3_size = Speed >> 1U; // amplitude of the curves
#endif
  fillAll(CRGB::Black);
  for (int16_t y = 0; y < HEIGHT; y++)
  {
    for (int16_t x = 0; x < WIDTH; x++)
    {
      CRGB color = CRGB::Black;
      zx = (int16_t)HEIGHT_d * _s3_scale / 2U - y * _s3_scale - 128U; // (rt version)
      zy = (int16_t)WIDTH_d * _s3_scale / 2U - x * _s3_scale - 128U;  // (rt version)

#if e_s3_BEATSINLOGIC
      cx = zx + beatsin8(4, _s3_size, ~_s3_size);         // (beatsin version)
      cy = zy + beatsin8(7, _s3_size, ~_s3_size, 0, 64U); // (beatsin version)
#else
      uint32_t _s3_time = millis();              // (rt version)
      cx = zx + rangesin8(_s3_time / 23, Speed); // (rt version)
      cy = zy + rangecos8(_s3_time / 29, Speed); // (rt version)
#endif

      if (_s3_inverse)
        color.r = dim8_raw(~sin8(sqrt16((cx * cx) + (cy * cy))));
      else
        color.r = brighten8_raw(sin8(sqrt16((cx * cx) + (cy * cy))));

#if e_s3_BEATSINLOGIC
      cx = zx + beatsin8(5, _s3_size, ~_s3_size);         // (beatsin version)
      cy = zy + beatsin8(8, _s3_size, ~_s3_size, 0, 64U); // (beatsin version)
#else
      cx = zx + rangesin8(_s3_time / 27, Speed); // (rt version)
      cy = zy + rangecos8(_s3_time / 31, Speed); // (rt version)
#endif
      if (_s3_inverse)
        color.g = dim8_raw(~sin8(sqrt16((cx * cx) + (cy * cy))));
      else
        color.g = brighten8_raw(sin8(sqrt16((cx * cx) + (cy * cy))));

#if e_s3_BEATSINLOGIC
      cx = zx + beatsin8(2, _s3_size, ~_s3_size);         // (beatsin version)
      cy = zy + beatsin8(3, _s3_size, ~_s3_size, 0, 64U); // (beatsin version)
#else
      cx = zx + rangesin8(_s3_time / 19, Speed); // (rt version)
      cy = zy + rangecos8(_s3_time / 15, Speed); // (rt version)
#endif
      if (_s3_inverse)
        color.b = dim8_raw(~sin8(sqrt16((cx * cx) + (cy * cy))));
      else
        color.b = brighten8_raw(sin8(sqrt16((cx * cx) + (cy * cy))));

      drawPixel(x, y, color);
    }
  }
  return true;
}

// ***** METABALLS / МЕТАШАРИКИ *****
/*  Metaballs proof of concept by Stefan Petrick (mod by Palpalych for GyverLamp 27/02/2020)
    ...very rough 8bit math here...
    read more about the concept of isosurfaces and metaballs:
    https://www.gamedev.net/articles/programming/graphics/exploring-metaballs-and-isosurfaces-in-2d-r2556 */
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020
// v1.1 - Palleting & Speeding by PalPalych 03.05.2020

#define e_mbl_DITHER (2U) // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново) Влияет на скорость!

bool metaBallsRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if (e_mbl_DITHER)
  if (effect.frame_counter % e_mbl_DITHER)
    return false;
#endif
  uint32_t timebase = millis();
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  // get some 2 random moving points
  effect.i2u = inoise8(timebase, 25355, 3685) / WIDTH;
  effect.j2u = inoise8(timebase, 15355, 11685) / HEIGHT;

  effect.i3u = inoise8(timebase, 55355, 6685) / WIDTH;
  effect.j3u = inoise8(timebase, 25355, 22685) / HEIGHT;

  // and one Lissajou function
  effect.i1u = beatsin8((Speed >> 1U) + 13U, 0, WIDTH_d);
  effect.j1u = beatsin8((Speed >> 2U) + 29U, 0, HEIGHT_d);

  for (effect.y16 = 0; effect.y16 < HEIGHT; effect.y16++)
  {
    for (effect.x16 = 0; effect.x16 < WIDTH; effect.x16++)
    {
      // calculate distances of the 3 points from actual pixel
      // and add them together with weightening
      effect.i16u = abs(effect.x16 - effect.i1u);
      effect.j16u = abs(effect.y16 - effect.j1u);
      effect.k16u = 2 * sqrt16((effect.i16u * effect.i16u) + (effect.j16u * effect.j16u));

      effect.i16u = abs(effect.x16 - effect.i2u);
      effect.j16u = abs(effect.y16 - effect.j2u);
      effect.k16u += sqrt16((effect.i16u * effect.i16u) + (effect.j16u * effect.j16u));

      effect.i16u = abs(effect.x16 - effect.i3u);
      effect.j16u = abs(effect.y16 - effect.j3u);
      effect.k16u += sqrt16((effect.i16u * effect.i16u) + (effect.j16u * effect.j16u));

      uint8_t col = (uint16_t)1000U / effect.k16u;
      // map color between thresholds
      drawPixel(effect.x16, effect.y16, ColorFromPalette(*curPalette, (col < 60) ? col * 9 : CRGB::Black, 255, TBlendType::LINEARBLEND));
      // show the 3 points, too
      drawPixel(effect.i1u, effect.j1u, CRGB::White);
      drawPixel(effect.i2u, effect.j2u, CRGB::White);
      drawPixel(effect.i3u, effect.j3u, CRGB::White);
    }
  }
  return true;
}

/*

  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 55, suggested range 20-100
  #define COOLING  55 // 100 // 55 // 86

  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  // Default 120, suggested range 50-200.
  #define SPARKING 50 // 30 // 120 // 90 // 60*/

//  ----------------------- FIRE 2013 EFFECT ---------------------
/*void fire2013WithPalette(char *param) /// как водопад только огонь синий
  {
  // Array of temperature readings at each simulation cell
  static byte heat[WIDTH][HEIGHT];

  for (uint8_t x = 0; x < WIDTH; x++) {
    // Step 1.  Cool down every cell a little
    for (int i = 0; i < HEIGHT; i++) {
      heat[x][i] = qsub8(heat[x][i], random8(0, ((COOLING * 10) / HEIGHT) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = HEIGHT_d; k >= 2; k--) {
      heat[x][k] = (heat[x][k - 1] + heat[x][k - 2] + heat[x][k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING) {
      int y = random8(2);
      heat[x][y] = qadd8(heat[x][y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < HEIGHT; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8(heat[x][j], 240);
      ledstripe[XY(x, (HEIGHT_d) - j)] = ColorFromPalette(CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White), colorindex);
    }
  }

  //return 15;
  }*/

// ***** PULSE / ПУЛЬС *****
// v1.0 - Updating for GuverLamp v1 by PalPalych 12.03.2020
// v1.1 - LEDraw engine - more smooth & speed! by PalPalych 06.04.2020
// v1.2 - Amount of pulses, Dithering & Frame divider by PalPalych 07.04.2020

#define e_pu_DITHER (4U)   // дизеринг
#define e_pu_DIVIDER (2U)  // делитель кадров
#define e_pu_NUMBER (4U)   // количество пузырей
#define e_pu_DIMMER (250U) // затемнение экрана

CRGB _pu_color[e_pu_NUMBER];
uint8_t _pu_step[e_pu_NUMBER];
int8_t _pu_currR[e_pu_NUMBER];
int8_t _pu_cX[e_pu_NUMBER];
int8_t _pu_cY[e_pu_NUMBER];
uint8_t _pu_hue[e_pu_NUMBER];
uint8_t _pu_hueall[e_pu_NUMBER];
uint8_t _pu_delta[e_pu_NUMBER];
uint8_t _pu_huestat[e_pu_NUMBER];
uint8_t _pu_divider = e_pu_DIVIDER - 1U;

bool pulseRoutine(uint8_t Scale = effect.scale_real)
{
#if e_pu_DITHER
  if (effect.frame_counter % e_pu_DITHER)
    return false;
#endif

  dimAll(e_pu_DIMMER);

#if e_pu_DIVIDER
  if (_pu_divider)
  {
    _pu_divider--;
    return true;
  }
  else
  {
    _pu_divider = e_pu_DIVIDER - 1U;
  }
#endif

  if (effect.hasChangedEffect())
  {
    for (uint8_t n = 0; n < e_pu_NUMBER; n++)
    {
      _pu_currR[n] = 4;
      _pu_cX[n] = random8(WIDTH - 5U) + 3U;
      _pu_cY[n] = random8(HEIGHT - 5U) + 3U;
    }
  }

  for (uint8_t n = 0; n < e_pu_NUMBER; n++)
  {
    {
      if (_pu_step[n] <= _pu_currR[n])
      {
        for (int8_t i = _pu_step[n]; i > 0; i--)
        {
          uint8_t _dark = qmul8(2U, cos8(128U / _pu_currR[n] * i));
          if (Scale == 1) // 1 - случайные диски
          {
            _pu_hue[n] = _pu_huestat[n];
            _pu_color[n] = CHSV(_pu_hue[n], 255U, _dark);
          }
          else if (Scale <= 17) // 2...17 - перелив цвета дисков
          {
            _pu_delta[n] = (17U - Scale);
            _pu_color[n] = CHSV(_pu_hueall[n], 255U, _dark);
          }
          else if (Scale <= 33) // 18...33 - выбор цвета дисков
          {
            _pu_hue[n] = (Scale - 18U) * 16U;
            _pu_color[n] = CHSV(_pu_hue[n], 255U, _dark);
          }
          else if (Scale <= 50) // 34...50 - дискоцветы
          {
            _pu_hue[n] += (Scale - 33U) * 5U;
            _pu_color[n] = CHSV(_pu_hue[n], 255U, _dark);
          }
          else if (Scale <= 67) // 51...67 - пузыри цветы
          {
            uint8_t _sat = qsub8(255U, cos8(128U / (_pu_currR[n] + 1U) * (i + 1U)));
            _pu_hue[n] += (68U - Scale) * 7U;
            _pu_color[n] = CHSV(_pu_hue[n], _sat, _dark);
          }
          else if (Scale < 83) // 68...83 - выбор цвета пузырей
          {
            uint8_t _sat = qsub8(255U, cos8(128U / (_pu_currR[n] + 1U) * (i + 1U)));
            _pu_hue[n] = (Scale - 68U) * 16U;
            _pu_color[n] = CHSV(_pu_hue[n], _sat, _dark);
          }
          else if (Scale < 100) // 84...99 - перелив цвета пузырей
          {
            uint8_t _sat = qsub8(255U, cos8(128U / (_pu_currR[n] + 1U) * (i + 1U)));
            _pu_delta[n] = (Scale - 85U);
            _pu_color[n] = CHSV(_pu_hueall[n], _sat, _dark);
          }
          else // 100 - случайные пузыри
          {
            uint8_t _sat = qsub8(255U, cos8(128U / (_pu_currR[n] + 1U) * (i + 1U)));
            _pu_hue[n] = _pu_huestat[n];
            _pu_color[n] = CHSV(_pu_hue[n], _sat, _dark);
          }
          drawOval2(_pu_cX[n] - i, _pu_cX[n] + i, _pu_cY[n] - i, _pu_cY[n] + i, _pu_color[n]);
          //drawCircle(_pu_cX[n], _pu_cY[n], i, _pu_color[n]  );
        }
        _pu_step[n]++;
      }
      else
      {
        _pu_cX[n] = random8(WIDTH - 5U) + 3U;
        _pu_cY[n] = random8(HEIGHT - 5U) + 3U;
        _pu_hueall[n] += _pu_delta[n];
        _pu_huestat[n] = random8();
        _pu_currR[n] = random8(2U, 6U);
        _pu_step[n] = 0;
      }
    }
  }
  return true;
}
#endif
