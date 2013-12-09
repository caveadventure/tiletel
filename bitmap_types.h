#ifndef __BITMAP_TYPES_H
#define __BITMAP_TYPES_H

struct bitmap {
    std::vector<uint8_t> bm;
    size_t w;
    size_t pitch;

    bitmap() : w(0), pitch(0) {}

    void swap(bitmap& a) {
        bm.swap(a.bm);
        w = a.w;
        pitch = a.pitch;
    }

    void make_pitch() {
        if ((w % 8) != 0) {
            pitch = (w / 8) + 1;
        } else {
            pitch = w / 8;
        }
    }

};


#endif
