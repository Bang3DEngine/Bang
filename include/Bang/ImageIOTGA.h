#ifndef IMAGEIOTGA_H
#define IMAGEIOTGA_H

/**
 * tga_reader.h
 *
 * Copyright (c) 2014 Kenji Sasaki
 * Released under the MIT license.
 * https://github.com/npedotnet/TGAReader/blob/master/LICENSE
 *
 * English document
 * https://github.com/npedotnet/TGAReader/blob/master/README.md
 *
 * Japanese document
 * http://3dtech.jp/wiki/index.php?TGAReader
 *
 */

#include <stdio.h>

#include "Bang/BangDefines.h"

NAMESPACE_BANG_BEGIN

struct TGA_ORDER
{
    int redShift;
    int greenShift;
    int blueShift;
    int alphaShift;
};

constexpr TGA_ORDER TGA_READER_ARGB = {0, 8, 16, 24};
constexpr TGA_ORDER TGA_READER_ABGR = {0, 8, 16, 24};

void *tgaMalloc(size_t size);
void tgaFree(void *memory);

int tgaGetWidth(const unsigned char *buffer);
int tgaGetHeight(const unsigned char *buffer);
int *tgaRead(const unsigned char *buffer, const TGA_ORDER *order);

NAMESPACE_BANG_END

#endif /* __TGA_READER_H__ */

