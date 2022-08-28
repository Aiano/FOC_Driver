#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "st7735.h"

#ifdef ST7735_IS_128X128

extern const uint16_t test_img_128x128[][128];
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
extern const uint16_t head_portrait_80x80[][80];

#endif

#endif // __IMAGE_H__