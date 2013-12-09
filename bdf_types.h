#ifndef __BDF_TYPES_H
#define __BDF_TYPES_H

#include "bitmap_types.h"

struct Font {

    size_t w;
    size_t h;

    std::unordered_map<uint16_t, bitmap> glyphs;

    Font() : w(0), h(0) {}
};

#endif
