#ifndef effectsAurora_h
#define effectsAurora_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_2d.h"

// ============= SIMPLEX NOISE / ОДИНОКИЙ ШУМ ===============
// Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternFlowField.h
// Copyright(c) 2014 Jason Coon
// v1.0 - Updating for GuverLamp v1.7 by Palpalych 14.04.2020

#define e_sns_DITHER (2U) // разрешить дисперсию эффектов (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

void ShowNoiseLayer(byte layer, byte colorrepeat, uint8_t colorshift, const TProgmemRGBPalette16 *curPalette)
{
  for (uint8_t i = 0; i < WIDTH; i++)
  {
    for (uint8_t j = 0; j < HEIGHT; j++)
    {
      uint8_t bri = noise[i][j];
      uint8_t pixel = noise[j][i];
      //bri = dim8_raw(bri);
      drawPixel(i, j, ColorFromPalette(*curPalette, colorrepeat * (pixel + colorshift), bri, LINEARBLEND));
    }
  }
}

bool simplexNoiseRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_sns_DITHER
  if (effect.frame_counter % e_sns_DITHER)
    return false;
#endif
	if (effect.hasChangedEffect())
	{
    noise_setup();
  }
  fillNoise2d(0);
  uint8_t scale1 = ((mod8(Scale, palettes_steps) * palettes_count) >> 2U) + 1U;
  uint8_t pos = Scale / palettes_steps;
  if (Speed > 127U)
  {
    Speed = ~Speed;
    if (!(mod8(eff_offset2++, e_sns_DITHER + 1U)))
      eff_offset++;
  }
  else
  {
    eff_offset = 0;
  }
  Speed <<= 1U;
  ShowNoiseLayer(0, 1, eff_offset, palettes[pos]);

  noise32_x[0] += scale16by8((uint16_t)beatsin8(3, 0, 255U) * (scale1 + 1), Speed);
  noise32_y[0] -= scale16by8((uint16_t)beatsin8(2, 0, 192U) * 2U * (scale1), Speed);
  noise32_z[0] += (uint16_t)map8(Speed, 1, 255U) * 8U;
  scale32_x[0] = (1024U + beatsin8(5, 0, 255U)) * (scale1);
  scale32_y[0] = (1024U + beatsin8(7, 0, 255U)) * (scale1);
  blurAll(3);
  return true;
}

// ============= BOUNCE / ПРЫЖКИ ===============
// Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternFlowField.h
// Copyright(c) 2014 Jason Coon
// v1.0 - Updating for GuverLamp v1.7 by Palpalych 14.04.2020
#define e_bnc_DITHER (2U) // разрешить дисперсию эффектов (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
#define e_bnc_COUNT ((int8_t)WIDTH)
#define e_bnc_SIDEJUMP (true)
PVector gravity = PVector(0, -0.0125);

bool bounceRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_bnc_DITHER
  if (effect.frame_counter % e_bnc_DITHER)
    return false;
#endif
	if (effect.hasChangedEffect())
	{
    unsigned int colorWidth = 256 / e_bnc_COUNT;
    for (int i = 0; i < e_bnc_COUNT; i++)
    {
      Boid boid = Boid(i, HEIGHT / 8);
      boid.velocity.x = 0;
      boid.location.y = HEIGHT_d / 4;
      boid.velocity.y = i * -0.01;
      boid.colorIndex = colorWidth * i;
      boid.maxforce = 10;
      boid.maxspeed = 10;
      boids[i] = boid;
    }
  }
  blurAll(beatsin8(5U, 1U, 5U));
  dimAll(250 - scale8(64U, (Scale % palettes_steps) * palettes_count));
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  for (int i = 0; i < e_bnc_COUNT; i++)
  {
    Boid boid = boids[i];

    boid.applyForce(gravity);
    boid.update();

    CRGB color = ColorFromPalette(*curPalette, boid.colorIndex + eff_offset);
    drawPixel((uint32_t)(boid.location.x) % WIDTH, boid.location.y, color);

    if (boid.location.y <= 0)
    {
      boid.location.y = 0;
      boid.velocity.y *= -1.0;
      boid.velocity.x *= 0.95;
      if (!random8() || boid.velocity.y < 0.01)
      {
#if e_bnc_SIDEJUMP
        boid.applyForce(PVector((float)random(128) / 255 - 0.25, (float)random(192) / 255));
#else
        boid.applyForce(PVector(0, (float)random(255) / 255));
#endif
      }
    }

    boids[i] = boid;
  }
  EVERY_N_MILLIS(100)
  {
    eff_offset++;
  }
  return true;
}

// ============= FLOWFIELD / КАСКАД ===============
// Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternFlowField.h
// Copyright(c) 2014 Jason Coon
// v1.0 - Updating for GuverLamp v1.7 by Palpalych 14.04.2020

#define _ffld_DITHER (2U) // разрешить дисперсию эффектов (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
#define _ffld_SPEED 1
#define _ffld_SCALE 26
#define _ffld_COUNT ((int8_t)WIDTH)

bool flowfieldRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if _ffld_DITHER
  if (effect.frame_counter % _ffld_DITHER)
    return false;
#endif
  if (effect.hasChangedEffect())
  {
    noise32_x[0] = random16();
    noise32_y[0] = random16();
    noise32_z[0] = random16();
    for (int i = 0; i < _ffld_COUNT; i++)
    {
      boids[i] = Boid(random8(WIDTH), 0);
    }
  }
  dimAll(240 - scale8(32U, (Scale % palettes_steps) * palettes_count));

  // CRGB color = effects.ColorFromCurrentPalette(hue);
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  for (int i = 0; i < _ffld_COUNT; i++)
  {
    Boid *boid = &boids[i];

    int ioffset = _ffld_SCALE * boid->location.x;
    int joffset = _ffld_SCALE * boid->location.y;

    eff_offset2 = inoise8(noise32_x[0] + ioffset, noise32_y[0] + joffset, noise32_z[0]);

    boid->velocity.x = (float)sin8(eff_offset2) * 0.0078125 - 1.0;
    boid->velocity.y = -((float)cos8(eff_offset2) * 0.0078125 - 1.0);
    boid->update();

    drawPixel(boid->location.x, HEIGHT_d - boid->location.y, ColorFromPalette(*curPalette, eff_offset2 + eff_offset)); // color

    if (boid->location.x < 0 || boid->location.x >= WIDTH ||
        boid->location.y < 0 || boid->location.y >= HEIGHT)
    {
      boid->location.x = random8(WIDTH);
      boid->location.y = 0;
    }
  }

  EVERY_N_MILLIS(200)
  {
    eff_offset++;
  }

  noise32_x[0] += _ffld_SPEED;
  noise32_y[0] += _ffld_SPEED;
  noise32_z[0] += _ffld_SPEED;
  return true;
}

// ============= SPIN / СПИН ===============
// Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternSpin.h
// Copyright(c) 2014 Jason Coon
// v1.0 - Updating for GuverLamp v1.7 by Palpalych 14.04.2020
// v1.1 - +dither, +smoothing, +tilt, +moving

#define e_spl_DITHER (2U) // разрешить дисперсию эффектов SPIN/СПИН (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

float degrees = 0;
float radius = 7;

float maxSpeed = 30;
float speedStart = 6;
float velocityStart = 0.6;

float speed = speedStart;
float velocity = velocityStart;

bool spinRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_spl_DITHER
  if (effect.frame_counter % e_spl_DITHER)
    return false;
#endif
  moveScreenY(0.4);
  blurAll(beatsin8(5U, 1U, 10U));
  dimAll(254U - scale8(3U, (Scale % palettes_steps) * palettes_count));
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  CRGB color = ColorFromPalette(*curPalette, speed * 16);
  // start position
  int x;
  int y;

  // target position
  float targetDegrees = degrees + speed;
  float targetRadians = radians(targetDegrees);
  int targetX = (int)(CENTERX + radius * cos(targetRadians));
  int targetY = (int)(CENTERY - radius * sin(targetRadians));

  float tempDegrees = degrees;

  do
  {
    float radians = radians(tempDegrees);
    x = (int)(CENTERX + radius * cos(radians));
    y = (int)(CENTERY - radius * sin(radians));

    drawPixel(x, y / 2 + 8, color);
    drawPixel(y, x / 2 + 8, color);

    tempDegrees += 1;
    if (tempDegrees >= 360)
      tempDegrees = 0;
  } while (x != targetX || y != targetY);

  degrees += speed;

  // add velocity to the particle each pass around the accelerator
  if (degrees >= 360)
  {
    degrees = 0;
    speed += velocity;
    if (speed <= speedStart)
    {
      speed = speedStart;
      velocity *= -1;
    }
    else if (speed > maxSpeed)
    {
      speed = maxSpeed - velocity;
      velocity *= -1;
    }
  }
  return true;
}

// ============= RADAR / РАДАР ===============
// Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternRadar.h
// Copyright(c) 2014 Jason Coon
// v1.0 - Updating for GuverLamp v1.7 by Palpalych 14.04.2020
// v1.1 - +dither, +smoothing
#define e_rad_DITHER (2U) // разрешить дисперсию эффектов DRIFT/DRIFT2 (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
bool radarRoutine(uint8_t Scale = effect.scale_real)
{
#if e_rad_DITHER
  if (effect.frame_counter % e_rad_DITHER)
    return false;
#endif
  blurAll(beatsin8(5U, 3U, 10U));
  dimAll(254U - scale8(4U, (Scale % palettes_steps) * palettes_count));
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  for (int offset = 0U; offset < SEMIWIDTH_d; offset++)
  {
    drawPixel_fast(mapcos8(eff_theta, offset, (WIDTH_d)-offset),
                   mapsin8(eff_theta, offset, (HEIGHT_d)-offset),
                   ColorFromPalette(*curPalette, 255U - (offset * 16U + eff_offset)));
    EVERY_N_MILLIS(25)
    {
      eff_theta += 2;
      eff_offset += 1;
    }
  }
  return true;
}

// ============= DRIFT / ДРИФТ ===============
// https://github.com/pixelmatix/aurora/blob/master/PatternIncrementalDrift.h
// v1.0 - Updating for GuverLamp v1.7 by SottNick 12.04.2020
// v1.1 - +dither, +phase shifting by PalPalych 12.04.2020
#define e_dri_DITHER (2U) // разрешить дисперсию эффектов DRIFT/DRIFT2 (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool incrementalDriftRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_dri_DITHER
  if (effect.frame_counter % e_dri_DITHER)
    return false;
#endif
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  blurAll(beatsin8(5U, 5U, 15U));
  dimAll(beatsin8(2U, 242U, 252U) - scale8(10U, (Scale % palettes_steps) * palettes_count));
  uint8_t _dri_speed = map8(Speed, 1U, 20U);
  uint8_t _dri_delta = beatsin8(1U);
  EVERY_N_MILLIS(13)
  {
    _dri_phase++;
  }
  for (uint8_t i = 1; i < SEMIWIDTH; i++) // возможно, стоит здесь использовать const MINLENGTH
  {
    int8_t x = beatsin8((CENTER_max - i) * _dri_speed, SEMIWIDTH_d - i, SEMIWIDTH_d + 1U + i, 0, 64U + _dri_phase); // используем константы центра матрицы из эффекта Кометы
    int8_t y = beatsin8((CENTER_max - i) * _dri_speed, SEMIWIDTH_d - i, SEMIWIDTH_d + 1U + i, 0, _dri_phase);       // используем константы центра матрицы из эффекта Кометы
    drawPixel(x, y, ColorFromPalette(*curPalette, (i - 1U) * WIDTH_steps * 2U + _dri_delta));                       // используем массив палитр из других эффектов выше
  }
  return true;
}

// ============= DRIFT 2 / ДРИФТ 2 ===============
// https://github.com/pixelmatix/aurora/blob/master/PatternIncrementalDrift2.h
// v1.0 - Updating for GuverLamp v1.7 by SottNick 12.04.2020
// v1.1 - +dither, +phase shifting by PalPalych 12.04.2020
bool incrementalDrift2Routine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_dri_DITHER
  if (effect.frame_counter % e_dri_DITHER)
    return false;
#endif
  const TProgmemRGBPalette16 *curPalette = palettes[Scale / palettes_steps];
  blurAll(beatsin8(3U, 5U, 10U));
  dimAll(beatsin8(2U, 236U, 252U) - scale8(10U, (Scale % palettes_steps) * palettes_count));
  uint8_t _dri_speed = map8(Speed, 1U, 15U);
  uint8_t _dri_delta = beatsin8(1U);
  EVERY_N_MILLIS(13)
  {
    _dri_phase++;
  }
  for (uint8_t i = 0; i < WIDTH; i++)
  {
    int8_t x = 0;
    int8_t y = 0;
    CRGB color;
    if (i < SEMIWIDTH)
    {
      x = beatsin8((i + 1) * _dri_speed, i + 1U, WIDTH_d - i, 0, 64U + _dri_phase);
      y = beatsin8((i + 1) * _dri_speed, i + 1U, HEIGHT_d - i, 0, _dri_phase);
      color = ColorFromPalette(*curPalette, i * WIDTH_steps * 2U + _dri_delta);
    }
    else
    {
      x = beatsin8((WIDTH - i) * _dri_speed, WIDTH_d - i, i + 1U, 0, _dri_phase);
      y = beatsin8((HEIGHT - i) * _dri_speed, HEIGHT_d - i, i + 1U, 0, 64U + _dri_phase);
      color = ColorFromPalette(*curPalette, ~(i * WIDTH_steps * 2U + _dri_delta));
    }
    drawPixel(x, y, color);
  }
  return true;
}
#endif