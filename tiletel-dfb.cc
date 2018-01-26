
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>

#include <++dfb/++dfb.h>

#include "libtsm/src/libtsm.h"

extern "C" {
#include "libtsm/src/libtsm_int.h"
}

#include "external/xkbcommon-keysyms.h"

#include "bdf.h"
#include "config.h"

#include <sys/time.h>

#include "lz77.h"


#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <pty.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>


template <typename T>
struct Tiler {

    inline T map_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        uint32_t _r = (a << 24) | (r << 16) | (g << 8) | b;
        return (T)_r;
    }

    inline T* cursor(void* screen_data, int screen_pitch, unsigned int x, unsigned int y) {
        return (T*)((uint8_t*)screen_data + (y * screen_pitch) + (x * sizeof(T)));
    }

    inline void set_pixel(void* screen_data, int screen_pitch, unsigned int x, unsigned int y, T color) {
        *cursor(screen_data, screen_pitch, x, y) = color;
    }

    inline void fill_rect(void* screen_data, int screen_pitch, unsigned int to_x, unsigned int to_y, 
                          unsigned int to_w, unsigned int to_h, T bgc) {

        for (unsigned int yy = to_y; yy < to_h + to_y; ++yy) {

            T* px = cursor(screen_data, screen_pitch, to_x, yy);

            for (unsigned int xx = 0; xx < to_w; ++xx, ++px) {
                *px = bgc;
            }
        }
    }

    inline void blit_bitmap(void* screen_data, int screen_pitch, unsigned int to_x, unsigned int to_y,
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

            T* px = cursor(screen_data, screen_pitch, to_x + xx, to_y + yy);

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


    inline uint32_t color_mul(const DFBColor& c, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(((r*c.r)/256), ((g*c.g)/256), ((b*c.b)/256), c.a);
    }

    inline uint32_t color_screen(const DFBColor& c, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(256 - (((256-r)*(256-c.r))/256.0), 
                         256 - (((256-g)*(256-c.g))/256.0), 
                         256 - (((256-b)*(256-c.b))/256.0), c.a);
    }

    inline uint32_t color_avg(const DFBColor& c, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(((r+c.r)/2), ((g+c.g)/2), ((b+c.b)/2), c.a);
    }

    template <typename SCREEN>
    void tile(const SCREEN& self, 
              unsigned int x, unsigned int y, 
              uint32_t ti, unsigned int cwidth, bool inverse, bool underline,
              uint8_t fr, uint8_t fg, uint8_t fb,
              uint8_t br, uint8_t bg, uint8_t bb) {

        if (ti == 0xfffd) {
            return;
        }

        if (x >= self.sw || y >= self.sh)
            throw std::runtime_error("Invalid screen offset in tile()");

        unsigned int to_x = x * self.tw;
        unsigned int to_y = y * self.th;
        unsigned int to_w = self.tw * cwidth;
        unsigned int to_h = self.th;

        if (inverse) {
            uint8_t tr = br;
            uint8_t tg = bg;
            uint8_t tb = bb;
            br = fr;
            bg = fg;
            bb = fb;
            fr = tr;
            fg = tg;
            fb = tb;
        }

        T fgc = map_color(fr, fg, fb, 0xFF);
        T bgc = map_color(br, bg, bb, 0xFF);

        fill_rect(self.screen_data, self.screen_pitch, to_x, to_y, to_w, to_h, bgc);

        bool did_tiles = false;

        // Draw pixel tiles.
        if (!self.tiles.empty()) {

            auto tmi = self.tile_mapping.find(ti);

            if (tmi != self.tile_mapping.end()) {

                uint32_t pixi = tmi->second;

                for (unsigned int cwi = 0; cwi < cwidth; ++cwi, ++pixi, to_x += self.tw) {

                    auto tmpi = self.tiles.find(pixi);
                    if (tmpi == self.tiles.end())
                        return;

                    for (const auto& l : tmpi->second.layers) {
                        T fgp = color_mul(l.color, fr, fg, fb);
                        blit_bitmap(self.screen_data, self.screen_pitch, to_x, to_y, fgp, l.bitmap);
                    }
                }

                did_tiles = true;
            }
        }

        // Or else draw font bitmaps.

        if (!did_tiles) {

            auto gi = self.font.glyphs.find(ti);
            
            if (gi == self.font.glyphs.end())
                return;

            const auto& glyph = gi->second;

            blit_bitmap(self.screen_data, self.screen_pitch, to_x, to_y, fgc, glyph);
        }

        if (underline) {

            T* px = cursor(self.screen_data, self.screen_pitch, to_x, to_y + to_h - 1);

            for (unsigned int w = 0; w < to_w; ++w, ++px) {
                *px = fgc;
            }
        }
    }
};



struct Screen {

    IDirectFB dfb;
    IDirectFBSurface screen;
    IDirectFBEventBuffer input;
    void* screen_data;
    int screen_pitch;

    Tiler<uint32_t> tiler;

    bdf::Font font;

    unsigned int tw;
    unsigned int th;

    unsigned int sw;
    unsigned int sh;

    bool done;

    struct indexed_bitmap {
        struct layer {
            DFBColor color;
            bdf::bitmap bitmap;
        };

        std::vector<layer> layers;
    };

    std::unordered_map<uint32_t, indexed_bitmap> tiles;

    const std::unordered_map<uint32_t, uint32_t>& tile_mapping;

    Screen(const config::Config& cfg) :
        screen_data(NULL), screen_pitch(0),
        tw(cfg.tile_width), th(cfg.tile_height), 
        sw(cfg.screen_width), sh(cfg.screen_height), 
        done(false), 
        tile_mapping(cfg.tile_mapping)
    {

        dfb = DirectFB::Create();

        dfb.SetCooperativeLevel(DFSCL_FULLSCREEN);

        DFBSurfaceDescription desc;
        desc.flags = DSDESC_CAPS;
        desc.caps = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY); // | DSCAPS_DOUBLE);

        screen = dfb.CreateSurface(desc);

        input = dfb.CreateInputEventBuffer(DICAPS_KEYS, DFB_FALSE);

        DFBSurfacePixelFormat pixfmt = screen.GetPixelFormat();

        if (pixfmt != DSPF_RGB32 && pixfmt != DSPF_ARGB) {
            throw std::runtime_error("Window surface has unsupported bytes per pixel. (Wanted 4, got "
                                     + std::to_string(pixfmt) + ")");
        }

        int w, h;
        screen.GetSize(&w, &h);
        sw = w / tw;
        sh = h / th;

        if (!cfg.tiles.empty()) {

            IDirectFBImageProvider image_provider = dfb.CreateImageProvider(cfg.tiles.c_str());

            DFBSurfaceDescription tiles_desc;
            image_provider.GetSurfaceDescription(&tiles_desc);

            tiles_desc.pixelformat = DSPF_LUT8;

            IDirectFBSurface rtiles = dfb.CreateSurface(tiles_desc);
            image_provider.RenderTo(&rtiles, NULL);

            surface_to_indexed(rtiles, tw, th, tiles);
        }

        auto fi = cfg.fonts.rbegin();

        if (fi == cfg.fonts.rend())
            throw std::runtime_error("At least one font needs to be specified!");

        bdf::parse_bdf(*fi, font);

        if (font.h != th)
            throw std::runtime_error("Font size does not match tile size: " + *fi);

        ++fi;
        while (fi != cfg.fonts.rend()) {
            bdf::Font font_tmp;
            bdf::parse_bdf(*fi, font_tmp);

            if (font_tmp.h != th)
                throw std::runtime_error("Font size does not match tile size: " + *fi);

            for (auto& g : font_tmp.glyphs) {
                font.glyphs[g.first].swap(g.second);
            }
            ++fi;
        }
    }

    static void print_bitmap(uint32_t ix, const bdf::bitmap& bitmap) {

        unsigned int x = 0;

        for (uint8_t v : bitmap.bm) {

            for (int bit = 7; bit >= 0; --bit) {

                if (v & (1 << bit)) {
                    std::cout << "*";
                } else {
                    std::cout << "-";
                }

                ++x;

                if (x >= bitmap.w) {
                    x = 0;
                    std::cout << std::endl;
                    break;
                }
            }
        }
    }


    static void surface_to_indexed(IDirectFBSurface& s, unsigned int tw, unsigned int th,
                                   std::unordered_map<uint32_t, indexed_bitmap>& out) {

        int w, h;
        s.GetSize(&w, &h);

        if ((w % tw) != 0 || (h % th) != 0) {

            throw std::runtime_error("Size of tiles image does not match tile size");
        }

        void* s_pixels;
        int s_pitch;
        s.Lock(DSLF_READ, &s_pixels, &s_pitch);

        if (s.GetPixelFormat() != DSPF_LUT8) 
            throw std::runtime_error("Only indexed (i.e., colormap or palette) tiles are supported");

        IDirectFBPalette _palette = s.GetPalette();

        std::vector<DFBColor> palette;
        palette.resize(_palette.GetSize());
        _palette.GetEntries(&(palette[0]), palette.size(), 0);

        indexed_bitmap base;

        std::unordered_map<uint32_t, size_t> colormap;

        for (const DFBColor& color : palette) {

            if (color.a == 0x00)
                continue;

            uint32_t colorhash = (color.r << 24) | (color.g << 16) | (color.b << 8) | (color.a);

            if (colormap.count(colorhash) != 0)
                continue;

            colormap[colorhash] = base.layers.size();
            base.layers.resize(base.layers.size() + 1);
            auto& l = base.layers.back();

            l.color = color;
            l.bitmap.w = tw;
            l.bitmap.make_pitch();
            l.bitmap.bm.resize(l.bitmap.pitch * th);
        }

        unsigned int numtw = w / tw;
        unsigned int numth = h / th;

        for (unsigned int tyy = 0; tyy < numth; ++tyy) {
            for (unsigned int txx = 0; txx < numtw; ++txx) {

                uint32_t which = (tyy * numtw) + txx;

                out[which] = base;
                indexed_bitmap& what = out[which];

                uint8_t* pixels0 = (uint8_t*)s_pixels + (tyy * th) * s_pitch + (txx * tw);

                for (unsigned int yy = 0; yy < th; ++yy) {

                    uint8_t* pixels = pixels0 + (yy * s_pitch);

                    for (unsigned int xx = 0; xx < tw; ++xx, ++pixels) {

                        uint8_t colorix = *pixels;
                        const DFBColor& color = palette[colorix];
                        uint32_t colorhash = (color.r << 24) | (color.g << 16) | (color.b << 8) | (color.a);

                        auto tmp = colormap.find(colorhash);

                        if (tmp == colormap.end())
                            continue;

                        bdf::bitmap& bitmap = what.layers[tmp->second].bitmap;

                        uint8_t& bmbyte = bitmap.bm[(yy * bitmap.pitch) + (xx / 8)];

                        bmbyte |= (1 << (7 - (xx % 8)));

                    }
                }
            }
        }

        s.Unlock();

        auto iz = out.begin();
        while (iz != out.end()) {

            indexed_bitmap& ib = iz->second;
            auto z = ib.layers.begin();

            while (z != ib.layers.end()) {

                bdf::bitmap& bitmap = z->bitmap;
                bool all_empty = true;

                for (uint8_t bmbyte : bitmap.bm) {
                    if (bmbyte != 0) {
                        all_empty = false;
                        break;
                    }
                }

                if (all_empty) {
                    z = ib.layers.erase(z);
                } else {
                    ++z;
                }
            }

            if (ib.layers.empty()) {
                iz = out.erase(iz);
            } else {
                
                /*
                std::cout << "\n\n[][ " << iz->first << " ][]\n" << std::endl;
                for (const auto& zzz : ib.layers) {
                    std::cout << "  color " << (int)zzz.color.r << "," 
                              << (int)zzz.color.g << "," << (int)zzz.color.b << std::endl;
                    print_bitmap(iz->first, zzz.bitmap);
                    std::cout << std::endl;
                }
                */

                ++iz;
            }
        }
    }

    void lock() {
        screen.Lock(DSLF_WRITE, &screen_data, &screen_pitch);
    }

    void unlock() {
        screen.Unlock();
    }

    void tile(unsigned int x, unsigned int y, 
              uint32_t ti, unsigned int cwidth, bool inverse, bool underline,
              uint8_t fr, uint8_t fg, uint8_t fb,
              uint8_t br, uint8_t bg, uint8_t bb) {

        tiler.tile(*this, x, y, ti, cwidth, inverse, underline, fr, fg, fb, br, bg, bb);
    }


    template <typename PROTOCOL>
    void handle_event(const DFBEvent& e, PROTOCOL& proto) {

        if (e.clazz == DFEC_INPUT && e.input.type == DIET_KEYPRESS) {

            proto.keypressor(e.input.key_symbol,
                             e.input.modifiers & DIMM_SHIFT,
                             e.input.modifiers & DIMM_CONTROL,
                             e.input.modifiers & DIMM_ALT);
        }
    }

    template <typename PROTOCOL>
    void mainloop(PROTOCOL& proto) {

        while (!done) {

            done = proto.multiplexor();

            screen.Flip();

            while (input.HasEvent()) {
                DFBEvent e;
                input.GetEvent(&e);
                handle_event(e, proto);
            }
        }
    }
};


void tsm_logger_cb(void* data, const char* file, int line, const char* func, const char* subs,
             unsigned int sev, const char* format, va_list args) {

    fprintf(stderr, "%d: %s: ", sev, subs);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}

template <typename SOCKET>
void tsm_term_writer_cb(struct tsm_vte* vte, const char* u8, size_t len, void* data) {

    SOCKET* socket = (SOCKET*)data;

    socket->send(u8, len);
}

int tsm_drawer_cb(struct tsm_screen* screen, uint32_t id, const uint32_t* ch, size_t len, unsigned int cwidth, 
                  unsigned int posx, unsigned int posy, const struct tsm_screen_attr* attr, tsm_age_t age, 
                  void* data) {

    if (cwidth == 0) {
        return 0;
    }

    Screen* draw = (Screen*)data;

    for (unsigned int i = 0; i < len; i += cwidth) {
        uint32_t c = ch[i];

        draw->tile(posx+i, posy, c, cwidth, attr->inverse, attr->underline,
                   attr->fr, attr->fg, attr->fb,
                   attr->br, attr->bg, attr->bb);
    }

    if (len == 0) {
        for (unsigned int j = 0; j < cwidth; ++j) {
            draw->tile(posx+j, posy, (uint32_t)' ', 1, attr->inverse, attr->underline,
                       attr->fr, attr->fg, attr->fb,
                       attr->br, attr->bg, attr->bb);
        }
    }

    return 0;
}

template <typename SOCKET>
struct VTE {

    tsm_screen* screen;
    tsm_vte* vte;

    Screen& draw;
    SOCKET& socket;

    unsigned int sw;
    unsigned int sh;
    
    VTE(Screen& d, SOCKET& s) : screen(NULL), vte(NULL), draw(d), socket(s), sw(d.sw), sh(d.sh)
    {

        try {

            if (tsm_screen_new(&screen, tsm_logger_cb, NULL) < 0)
                throw std::runtime_error("Could not init tsm_screen");

            if (tsm_vte_new(&vte, screen, tsm_term_writer_cb<SOCKET>, &socket, tsm_logger_cb, NULL) < 0)
                throw std::runtime_error("Could not init tsm_vte");

            if (tsm_screen_resize(screen, sw, sh) < 0)
                throw std::runtime_error("Could not resize screen");

        } catch (...) {

            tsm_vte_unref(vte);
            tsm_screen_unref(screen);
            throw;
        }
    }

    ~VTE() {
        tsm_vte_unref(vte);
        tsm_screen_unref(screen);
    }

    void resize(unsigned int _sw, unsigned int _sh) {

        sw = _sw;
        sh = _sh;

        if (tsm_screen_resize(screen, sw, sh) < 0)
            throw std::runtime_error("Could not resize screen");
    }

    void feed(const std::string& data) {

        tsm_vte_input(vte, data.data(), data.size());
    }

    void redraw() {

        draw.lock();

        //tsm_age_t age =
        tsm_screen_draw(screen, tsm_drawer_cb, &draw);

        draw.unlock();
    }

    void set_palette(const std::string& palette) {
        tsm_vte_set_palette(vte, palette.c_str());
    }

    void set_cursor(bool onoff) {
        unsigned int flags = tsm_screen_get_flags(screen);
        
        if (!onoff) {
            flags |= TSM_SCREEN_HIDE_CURSOR;
        } else {
            flags &= ~(TSM_SCREEN_HIDE_CURSOR);
        }

        tsm_screen_set_flags(screen, flags);
    }
};


struct Socket {

    int fd;
    struct pollfd poller;

    bool compression;
    std::string compression_leftover;
    lz77::decompress_t decompressor;

    void teardown(const std::string& msg) {

        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
        fd = -1;

        throw std::runtime_error(msg);
    }
    
    Socket(const std::string& host, unsigned int port) : compression(false) {

        struct sockaddr_in serv_addr;
        struct hostent* server;

        fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) 
            throw std::runtime_error("Could not socket()");

        server = ::gethostbyname(host.c_str());

        if (server == NULL) 
            teardown("Invalid host: " + host);

        ::memset((void*)&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;

        ::memcpy((void*)&serv_addr.sin_addr.s_addr, (void*)server->h_addr, server->h_length);
        serv_addr.sin_port = htons(port);

        if (::connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            teardown("Could not connect to " + host);

        poller.fd = fd;
        poller.events = POLLIN;
    }

    ~Socket() {
        close();
    }

    void close() {

        if (fd >= 0) {
            ::shutdown(fd, SHUT_RDWR);
            ::close(fd);
        }
    }

    bool recv_raw(std::string& out) {

        ssize_t i = ::recv(fd, (void*)out.data(), out.size(), 0);

        if (i < 0)
            return false;
            //throw std::runtime_error("Error receiving data");

        out.resize(i);

        if (out.empty())
            return false;

        return true;
    }

    bool recv(std::string& out) {

        bool ret = true;

        if (compression_leftover.size() > 0) {
            out.swap(compression_leftover);
            compression_leftover.clear();

        } else {
            ret = recv_raw(out);
        }

        if (!compression)
            return ret;

        bool done = decompressor.feed(out, compression_leftover);

        while (!done) {

            ret = recv_raw(out);

            done = decompressor.feed(out, compression_leftover);
        }

        out.swap(decompressor.result());

        return ret;
    }

    void reread(const std::string& buff) {
        compression_leftover = buff;
    }

    void send(const std::string& in) {

        ssize_t tmp = ::send(fd, (void*)in.data(), in.size(), MSG_NOSIGNAL);

        if (tmp != (ssize_t)in.size())
            throw std::runtime_error("Error sending data");
    }

    void send(const char* data, size_t len) {

        ssize_t tmp = ::send(fd, data, len, MSG_NOSIGNAL);

        if (tmp != (ssize_t)len)
            throw std::runtime_error("Error sending data");
    }

    bool poll(unsigned int wait = 100) {

        if (compression_leftover.size() > 0) {
            return true;
        }

        int r = ::poll(&poller, 1, wait);

        if (r < 0)
            throw std::runtime_error("Error in poll()");

        return (r != 0);
    }
};

struct Process {

    int fd;
    pid_t pid;
    struct pollfd poller;

    Process(const std::vector<std::string>& cmd, const std::string term_type) {

        if (cmd.size() == 0)
            throw std::runtime_error("Need a command to run.");

        pid = forkpty(&fd, NULL, NULL, NULL);

        if (pid < 0)
            throw std::runtime_error("Could not forkpty()");

        poller.fd = fd;
        poller.events = POLLIN;

        if (pid == 0) {

            setenv("TERM", term_type.c_str(), 1);

            char** argv = new char*[cmd.size() + 1];

            for (size_t i = 0; i < cmd.size(); ++i) {
                argv[i] = (char*)cmd[i].c_str();
            }

            argv[cmd.size()] = NULL;

            execvp(argv[0], argv);

            std::string tmp;
            
            for (const std::string& i : cmd) {
                tmp += ' ';
                tmp += i;
            }
            
            throw std::runtime_error("Could not execute:" + tmp);
        }
    }

    ~Process() {
        close(fd);
    }

    bool recv(std::string& out) {

        ssize_t i = read(fd, (char*)out.data(), out.size());

        if (i < 0)
            return false;
            //throw std::runtime_error("Error reading data");

        out.resize(i);

        if (out.empty())
            return false;

        return true;
    }

    void send(const std::string& in) {

        ssize_t i = write(fd, (char*)in.data(), in.size());
    
        if (i != (ssize_t)in.size())
            throw std::runtime_error("Error writing data");
    }

    void send(const char* data, size_t len) {

        ssize_t i = write(fd, data, len);

        if (i != (int)len)
            throw std::runtime_error("Error writing data");
    }

    bool poll(unsigned int wait = 100) {

        int r = ::poll(&poller, 1, wait);

        if (r < 0)
            throw std::runtime_error("Error in poll()");

        return (r != 0);
    }
};

template <typename SOCKET>
struct Protocol_Base {

    VTE<SOCKET>& vte;

    Protocol_Base(VTE<SOCKET>& _vte) : vte(_vte) {}

    // Feeds raw keypress data to the terminal emulator.
    void keypressor(DFBInputDeviceKeySymbol ksym, bool shift, bool ctrl, bool alt) {

        unsigned char key = (ksym > 127 ? '?' : ksym);

        if (key == 0)
            return;

        unsigned int mods = 0;

        if (shift) {
            mods |= TSM_SHIFT_MASK;
        }

        if (alt) {
            mods |= TSM_ALT_MASK;
        }

        if (ctrl) {
            mods |= TSM_CONTROL_MASK;
        }

        uint32_t tsmsym = XKB_KEY_NoSymbol;

        if ((ksym & DIKT_SPECIAL) == DIKT_SPECIAL) {
        
            switch (ksym) {
            case DIKS_INSERT:
                tsmsym = XKB_KEY_Insert;
                break;
            case DIKS_HOME:
                tsmsym = XKB_KEY_Home;
                break;
            case DIKS_PAGE_UP:
                tsmsym = XKB_KEY_Page_Up;
                break;
            case DIKS_END:
                tsmsym = XKB_KEY_End;
                break;
            case DIKS_PAGE_DOWN:
                tsmsym = XKB_KEY_Page_Down;
                break;
            case DIKS_CURSOR_RIGHT:
                tsmsym = XKB_KEY_Right;
                break;
            case DIKS_CURSOR_LEFT:
                tsmsym = XKB_KEY_Left;
                break;
            case DIKS_CURSOR_DOWN:
                tsmsym = XKB_KEY_Down;
                break;
            case DIKS_CURSOR_UP:
                tsmsym = XKB_KEY_Up;
                break;

                /* TODO
            case SDLK_KP_1:
                tsmsym = XKB_KEY_Select;
                break;
            case SDLK_KP_2:
                tsmsym = XKB_KEY_KP_Down;
                break;
            case SDLK_KP_3:
                tsmsym = XKB_KEY_KP_Page_Down;
                break;
            case SDLK_KP_4:
                tsmsym = XKB_KEY_KP_Left;
                break;
            case SDLK_KP_5:
                tsmsym = XKB_KEY_period;
                break;
            case SDLK_KP_6:
                tsmsym = XKB_KEY_KP_Right;
                break;
            case SDLK_KP_7:
                tsmsym = XKB_KEY_Find;
                break;
            case SDLK_KP_8:
                tsmsym = XKB_KEY_KP_Up;
                break;
            case SDLK_KP_9:
                tsmsym = XKB_KEY_KP_Page_Up;
                break;
                */

            case DIKS_F1:
                tsmsym = XKB_KEY_F1;
                break;
            case DIKS_F2:
                tsmsym = XKB_KEY_F2;
                break;
            case DIKS_F3:
                tsmsym = XKB_KEY_F3;
                break;
            case DIKS_F4:
                tsmsym = XKB_KEY_F4;
                break;
            case DIKS_F5:
                tsmsym = XKB_KEY_F5;
                break;
            case DIKS_F6:
                tsmsym = XKB_KEY_F6;
                break;
            case DIKS_F7:
                tsmsym = XKB_KEY_F7;
                break;
            case DIKS_F8:
                tsmsym = XKB_KEY_F8;
                break;
            case DIKS_F9:
                tsmsym = XKB_KEY_F9;
                break;
            case DIKS_F10:
                tsmsym = XKB_KEY_F10;
                break;
            case DIKS_F11:
                tsmsym = XKB_KEY_F11;
                break;
            case DIKS_F12:
                tsmsym = XKB_KEY_F12;
                break;
            default:
                return;
            }
        }

        tsm_vte_handle_keyboard(vte.vte, tsmsym, key, mods, key);
    }
};


template <typename SOCKET>
struct Protocol_Telnet : public Protocol_Base<SOCKET> {

    unsigned int polltimeout;
    bool enable_compression;
    std::string term_type;

    using Protocol_Base<SOCKET>::vte;

    Protocol_Telnet(VTE<SOCKET>& _vte, unsigned int pt, bool ec, const std::string& tt) : 
        Protocol_Base<SOCKET>(_vte), polltimeout(pt), enable_compression(ec), term_type(tt) {}

    // Send a resize event to the telnet server.
    void send_resize(unsigned int sw, unsigned int sh) {

        std::string tmp;
        tmp.reserve(9);

        tmp += '\xFF';
        tmp += '\xFA';
        tmp += '\x1F';
        tmp += (unsigned char)(sw >> 8);
        tmp += (unsigned char)(sw & 0xFF);
        tmp += (unsigned char)(sh >> 8);
        tmp += (unsigned char)(sh & 0xFF);
        tmp += '\xFF';
        tmp += '\xF0';

        vte.socket.send(tmp);
    }

    // Cruft for the telnet server.
    void send_terminal_type(const std::string& type) {

        std::string tmp;
        tmp.reserve(6+type.size());

        tmp += '\xFF';
        tmp += '\xFA';
        tmp += '\x18';
        tmp += '\x00';
        tmp += type;
        tmp += '\xFF';
        tmp += '\xF0';

        vte.socket.send(tmp);
    }

    void telnet_will(char c) {
        std::string tmp;
        tmp.reserve(3);

        tmp += '\xFF';
        tmp += '\xFB';
        tmp += c;

        vte.socket.send(tmp);
    }

    void telnet_wont(char c) {
        std::string tmp;
        tmp.reserve(3);

        tmp += '\xFF';
        tmp += '\xFC';
        tmp += c;

        vte.socket.send(tmp);
    }

    void telnet_do(char c) {
        std::string tmp;
        tmp.reserve(3);

        tmp += '\xFF';
        tmp += '\xFD';
        tmp += c;

        vte.socket.send(tmp);
    }

    // Handle a window resize event from the GUI.
    void resizer(unsigned int sw, unsigned int sh) {

        send_resize(sw, sh);
        vte.resize(sw, sh);
        vte.redraw();
    }


    // The meat of the telnet protocol.
    bool multiplexor() {

        static std::string buff;
        static std::string rewritten;

        if (!vte.socket.poll(polltimeout))
            return false;

        enum {
            STREAM,
            IAC,
            DO,
            DONT,
            WONT,
            WILL,
            SB,
            SB_IAC
        } telnetstate = STREAM;

        bool did_enable_compression = false;
        bool do_restart = false;

        while (1) {

            buff.resize(16*1024);

            if (!vte.socket.recv(buff)) {

                return true;
            }

            rewritten.reserve(buff.size());

            for (auto ci = buff.begin(); ci != buff.end() && !do_restart; ++ci) {
                char c = *ci;

                switch (telnetstate) {

                case STREAM:

                    if (c == '\xFF') {
                        telnetstate = IAC;

                    } else if (c == '\0') {
                        // Nothing, skip random null bytes that telnetd likes to send for no reason.

                    } else {
                        rewritten += c;
                    }
                    break;

                case IAC:

                    if (c == '\xFF') {
                        rewritten += c;
                        telnetstate = STREAM;

                    } else if (c == '\xFA') {
                        telnetstate = SB;
                        did_enable_compression = false;

                    } else if (c == '\xFE') {
                        telnetstate = DONT;

                    } else if (c == '\xFD') {
                        telnetstate = DO;

                    } else if (c == '\xFC') {
                        telnetstate = WONT;

                    } else if (c == '\xFB') {
                        telnetstate = WILL;

                    } else {
                        telnetstate = STREAM;
                    }
                    break;

                case SB:

                    if (c == '\xFF') {
                        telnetstate = SB_IAC;

                    } else if (c == '\x18') {
                        send_terminal_type(term_type);

                    } else if (c == '\x55') {
                        did_enable_compression = true;
                    }
                    break;

                case SB_IAC:

                    if (c == '\xF0') {

                        telnetstate = STREAM;

                        if (did_enable_compression) {
                            vte.socket.compression = true;
                            vte.socket.reread(std::string(ci+1, buff.end()));
                            did_enable_compression = false;
                            do_restart = true;
                        }

                    } else {
                        telnetstate = SB;
                    }
                    break;
            
                case DONT:
                    telnetstate = STREAM;
                    break;

                case WONT:
                    telnetstate = STREAM;
                    break;

                case WILL:

                    if (c == '\x55' && enable_compression) {
                        // Compression.
                        telnet_do(c);
                    }

                    telnetstate = STREAM;
                    break;

                case DO:

                    if (c == '\x1F') {
                        // Window size.
                        telnet_will(c);
                        send_resize(vte.sw, vte.sh);

                    } else if (c == '\x18') {
                        // Terminal type
                        telnet_will(c);

                    } else {
                        telnet_wont(c);
                    }

                    telnetstate = STREAM;
                    break;
                }
            }

            if (do_restart) {
                do_restart = false;
                continue;
            }

            if (telnetstate == STREAM)
                break;
        }

        //

        vte.feed(rewritten);
        vte.redraw();

        buff.clear();
        rewritten.clear();

        return false;
    }
};

template <typename SOCKET>
struct Protocol_Pty : public Protocol_Base<SOCKET> {

    unsigned int polltimeout;

    using Protocol_Base<SOCKET>::vte;

    Protocol_Pty(VTE<SOCKET>& _vte, unsigned int pt, bool ec) :
        Protocol_Base<SOCKET>(_vte), polltimeout(pt) {}

    void send_resize(unsigned int sw, unsigned int sh) {
        
        struct winsize ws;
        ws.ws_col = sw;
        ws.ws_row = sh;
        
        ioctl(vte.socket.fd, TIOCSWINSZ, &ws);
    }

    void resizer(unsigned int sw, unsigned int sh) {

        send_resize(sw, sh);
        vte.resize(sw, sh);
        vte.redraw();
    }

    bool multiplexor() {

        static std::string buff;

        if (!vte.socket.poll(polltimeout)) {
            return false;
        }

        buff.resize(16*1024);

        if (!vte.socket.recv(buff)) {
            return true;
        }

        vte.feed(buff);
        vte.redraw();

        buff.clear();

        return false;
    }
};

void usage(const std::string& argv0) {
    std::cerr << "Usage: " << argv0 << " [--config <cfgfile>] [host] [port]" << std::endl;
}

int main(int argc, char** argv) {

    try {

        DirectFB::Init(&argc, &argv);

        std::string configfile("default.cfg");

        bool did_host = false;
        bool did_port = false;
        std::string host;
        unsigned int port = 0;

        bool do_command = false;
        std::vector<std::string> command;

        bool notiles = false;

        for (int ai = 1; ai < argc; ++ai) {
            std::string q(argv[ai]);
            
            if (q == "-c" || q == "--config") {

                if (ai+1 >= argc) {
                    usage(argv[0]);
                    return 1;
                }

                configfile = argv[ai+1];
                ++ai;

            } else if (q == "-n" || q == "--notiles") {

                notiles = true;

            } else if (q == "-e" || q == "--exec") {
                do_command = true;

            } else if (do_command) {
                command.push_back(q);

            } else if (!did_host) {
                host = q;
                did_host = true;

            } else if (!did_port) {
                port = ::atoi(q.c_str());
                did_port = true;

            } else {
                usage(argv[0]);
                return 1;
            }
        }
            

        config::Config cfg;
        config::parse_config(configfile, cfg);

        if (did_host) {
            cfg.host = host;
        }

        if (did_port) {
            cfg.port = port;
        }

        if (do_command) {
            cfg.command = command;
        }

        if (cfg.host.empty() && cfg.command.empty()) {
            std::cerr << "No host to connect to." << std::endl;
            usage(argv[0]);
            return 1;
        }

        if (notiles) {
            cfg.tiles.clear();
        }


        if (cfg.command.size() > 0) {

            Process proc(cfg.command, cfg.term_type);

            Screen screen(cfg);

            VTE<Process> vte(screen, proc);

            if (cfg.palette.size() > 0) {
                vte.set_palette(cfg.palette);
            }

            vte.set_cursor(cfg.cursor);

            Protocol_Pty<Process> protocol(vte, cfg.polling_rate, cfg.compression);

            protocol.resizer(screen.sw, screen.sh);
        
            screen.mainloop(protocol);
            
            return 0;
        }

        Socket sock(cfg.host, cfg.port);

        Screen screen(cfg);

        VTE<Socket> vte(screen, sock);

        if (cfg.palette.size() > 0) {
            vte.set_palette(cfg.palette);
        }

        vte.set_cursor(cfg.cursor);

        Protocol_Telnet<Socket> protocol(vte, cfg.polling_rate, cfg.compression, cfg.term_type);
        
        screen.mainloop(protocol);


    } catch (std::exception& e) {
        std::cout << "Fatal Error: " << e.what() << std::endl;
        return 1;

    } catch (...) {
        std::cout << "Unknown fatal error." << std::endl;
        return 1;
    }

    return 0;
}
