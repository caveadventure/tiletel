
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>

#include "SDL2/SDL.h"

#include "bdf.h"


template <typename T>
struct Tiler {

    inline T map_color(SDL_Surface* screen, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        uint32_t _r = SDL_MapRGBA(screen->format, r, g, b, a);
        return (T)_r;
    }

    inline T* cursor(SDL_Surface* screen, unsigned int x, unsigned int y) {
        return (T*)((uint8_t*)screen->pixels + (y * screen->pitch) + (x * sizeof(T)));
    }

    inline void set_pixel(SDL_Surface* screen, unsigned int x, unsigned int y, T color) {
        *cursor(screen, x, y) = color;
    }

    inline void fill_rect(SDL_Surface* screen, unsigned int to_x, unsigned int to_y, 
                          unsigned int to_w, unsigned int to_h, T bgc) {

        for (unsigned int yy = to_y; yy < to_h + to_y; ++yy) {

            T* px = cursor(screen, to_x, yy);

            for (unsigned int xx = 0; xx < to_w; ++xx, ++px) {
                *px = bgc;
            }
        }
    }

    inline void blit_bitmap(SDL_Surface* screen, unsigned int to_x, unsigned int to_y,
                            T fgc, const bdf::bitmap& bitmap) {

        unsigned int xx = 0;
        unsigned int yy = 0;

        for (uint8_t v : bitmap.bm) {

            if (v == 0) {
                xx += 8;

                if (xx >= bitmap.w) {
                    xx = 0;
                    ++yy;
                }
                continue;
            }

            T* px = cursor(screen, to_x + xx, to_y + yy);

            for (int bit = 7; bit >= 0; --bit) {

                if (v & (1 << bit)) 
                    *px = fgc;

                ++xx;
                ++px;

                if (xx >= bitmap.w) {
                    xx = 0;
                    ++yy;
                    break;
                }
            }
        }
    }


    inline uint32_t color_mul(SDL_Surface* screen, const SDL_Color& c, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(screen, ((r*c.r)/256), ((g*c.g)/256), ((b*c.b)/256), c.a);
    }

    inline uint32_t color_screen(SDL_Surface* screen, const SDL_Color& c, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(screen, 
                         256 - (((256-r)*(256-c.r))/256.0), 
                         256 - (((256-g)*(256-c.g))/256.0), 
                         256 - (((256-b)*(256-c.b))/256.0), c.a);
    }

    inline uint32_t color_avg(SDL_Surface* screen, const SDL_Color& c, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(screen, ((r+c.r)/2), ((g+c.g)/2), ((b+c.b)/2), c.a);
    }

    void tile(SDL_Surface* screen,
              unsigned int to_x, unsigned int to_y, unsigned int to_w, unsigned int to_h,
              const bdf::bitmap& glyph, bool underline,
              uint8_t fr, uint8_t fg, uint8_t fb, uint8_t br, uint8_t bg, uint8_t bb) {

        T fgc = map_color(screen, fr, fg, fb, 0xFF);
        T bgc = map_color(screen, br, bg, bb, 0xFF);

        fill_rect(screen, to_x, to_y, to_w, to_h, bgc);

        blit_bitmap(screen, to_x, to_y, fgc, glyph);

        if (underline) {

            T* px = cursor(screen, to_x, to_y + to_h - 1);

            for (unsigned int w = 0; w < to_w; ++w, ++px) {
                *px = fgc;
            }
        }
    }
};

void make_bmp(const std::string& out_half, const std::string& out_full, std::vector<std::string>& fonts) {

    bdf::Font font;
    bool first = true;
    size_t th = 0;
    
    for (const std::string& f : fonts) {

        if (first) {
            bdf::parse_bdf(f, font);
            th = font.h;
            first = false;

        } else {
            bdf::Font ftmp;
            bdf::parse_bdf(f, ftmp);

            if (ftmp.h != th)
                throw std::runtime_error("Font height mismatch.");

            for (auto& g : ftmp.glyphs) {
                font.glyphs[g.first].swap(g.second);
            }
        }
    }

    uint16_t gmax = 0;
    size_t minw = 0xFFFF;
    size_t maxw = 0;
    for (auto& g : font.glyphs) {
        gmax = std::max(gmax, g.first);
        minw = std::min(minw, g.second.w);
        maxw = std::max(maxw, g.second.w);
    }

    if (maxw != minw*2) {
        throw std::runtime_error("Font is not a proper monospace font 1.");
    }
    
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
        throw std::runtime_error("Could not init SDL");

    size_t w1 = minw * 256;
    size_t w2 = maxw * 256;
    size_t h = (((gmax+1) >> 8) + ((gmax+1) & 0xFF ? 1 : 0)) * font.h;

    SDL_Surface* surf_half = SDL_CreateRGBSurface(0, w1, h, 32, 0xFF, 0xFF00, 0xFF0000, 0);

    if (surf_half == NULL)
        throw std::runtime_error(SDL_GetError());

    SDL_Surface* surf_full = SDL_CreateRGBSurface(0, w2, h, 32, 0xFF, 0xFF00, 0xFF0000, 0);

    if (surf_full == NULL)
        throw std::runtime_error(SDL_GetError());

    if (surf_half->format->BytesPerPixel != 4 ||
        surf_full->format->BytesPerPixel != 4) {
        throw std::runtime_error("Surface has unsupported bytes per pixel. (Wanted 4)");
    }

    Tiler<uint32_t> tiler;

    for (const auto& g : font.glyphs) {

        if (g.second.w != minw && g.second.w != maxw)
            throw std::runtime_error("Font is not a proper monospace font 2.");

        if (g.second.w == minw) {

            tiler.tile(surf_half, (g.first % 256) * minw, (g.first / 256) * font.h, minw, font.h, g.second, false,
                       0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

        } else if (g.second.w == maxw) {

            tiler.tile(surf_full, (g.first % 256) * maxw, (g.first / 256) * font.h, maxw, font.h, g.second, false,
                       0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
        }
    }
    
    if (SDL_SaveBMP(surf_half, out_half.c_str()) != 0)
        throw std::runtime_error(SDL_GetError());

    if (SDL_SaveBMP(surf_full, out_full.c_str()) != 0)
        throw std::runtime_error(SDL_GetError());

    SDL_Quit();
}

void usage(const std::string& argv0) {
    std::cerr << "Usage: " << argv0 << " {output_half.bmp} {output_full.bmp} {bdf file...}" << std::endl;
}

int main(int argc, char** argv) {

    try {

        if (argc <= 3) {
            usage(argv[0]);
            return 1;
        }

        std::string output_half = argv[1];
        std::string output_full = argv[2];
        std::vector<std::string> fonts;

        for (int ai = 3; ai < argc; ++ai) {
            fonts.push_back(argv[ai]);
        }

        make_bmp(output_half, output_full, fonts);

    } catch (std::exception& e) {
        std::cout << "Fatal Error: " << e.what() << std::endl;
        return 1;

    } catch (...) {
        std::cout << "Unknown fatal error." << std::endl;
        return 1;
    }

    return 0;
}
