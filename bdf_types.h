#ifndef __BDF_TYPES_H
#define __BDF_TYPES_H

struct Font {

    struct glyph {
        std::vector<uint8_t> bitmap;
        size_t w;
        size_t pitch;

        glyph() : w(0), pitch(0) {}

        void swap(glyph& a) {
            bitmap.swap(a.bitmap);
            w = a.w;
            pitch = a.pitch;
        }
    };

    size_t w;
    size_t h;

    std::unordered_map<uint16_t, glyph> glyphs;

    Font() : w(0), h(0) {}
};

#endif
