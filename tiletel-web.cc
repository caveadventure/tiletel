
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>
#include <map>

#include <thread>
#include <mutex>

#include <gd.h>

#include "libtsm/src/libtsm.h"

extern "C" {
#include "libtsm/src/libtsm_int.h"
}

#include "external/xkbcommon-keysyms.h"

#include "bdf.h"
#include "config.h"

#include <sys/time.h>

#include "lz77.h"

#include "sha1.h"

#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <ctype.h>



struct Socket;

void write_websocket_frame(Socket&, void*, size_t);


struct Tiler {

    config::Config& cfg;

    unsigned int tw;
    unsigned int th;
    unsigned int sw;
    unsigned int sh;

    gdImagePtr screen;

    struct indexed_bitmap {
        struct layer {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            bdf::bitmap bitmap;
        };

        std::vector<layer> layers;
    };

    std::unordered_map<uint32_t, indexed_bitmap> tiles;

    bdf::Font font;

    Tiler(config::Config& _cfg):
        cfg(_cfg),
        tw(cfg.tile_width), th(cfg.tile_height), 
        sw(cfg.screen_width), sh(cfg.screen_height),
        screen(NULL) {

        if (!cfg.tiles.empty()) {

            FILE* f = ::fopen(cfg.tiles.c_str(), "rb");

            if (f == NULL)
                throw std::runtime_error("Could not open " + cfg.tiles);
            
            gdImagePtr rtiles = NULL;

            try {
            
                rtiles = gdImageCreateFromPng(f);

                if (rtiles == NULL)
                    throw std::runtime_error("Could not read PNG: " + cfg.tiles);

                if (gdImageTrueColor(rtiles))
                    throw std::runtime_error("Only palette indexed PNG files are supported.");

                surface_to_indexed(rtiles, tw, th, tiles);

                gdImageDestroy(rtiles);
                ::fclose(f);
                
            } catch (...) {

                if (rtiles != NULL)
                    gdImageDestroy(rtiles);

                if (f != NULL)
                    ::fclose(f);

                throw;
            }
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

        screen = gdImageCreateTrueColor(tw*sw, th*sh);

        if (screen == NULL)
            throw std::runtime_error("Could not allocate screen buffer");
    }

    ~Tiler() {

        if (screen != NULL)
            gdImageDestroy(screen);
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

    static void surface_to_indexed(gdImagePtr tiles, unsigned int tw, unsigned int th,
                                   std::unordered_map<uint32_t, indexed_bitmap>& out) {

        unsigned int bw = gdImageSX(tiles);
        unsigned int bh = gdImageSY(tiles);
        
        if ((bw % tw) != 0 || (bh % th) != 0) {

            throw std::runtime_error("Size of tiles image does not match tile size");
        }

        indexed_bitmap base;

        std::unordered_map<uint32_t, size_t> colormap;

        unsigned int ncolors = gdImageColorsTotal(tiles);

        for (unsigned int ci = 0; ci < ncolors; ++ci) {

            int _r = gdImageRed(tiles, ci);
            int _g = gdImageGreen(tiles, ci);
            int _b = gdImageBlue(tiles, ci);
            int _a = gdImageAlpha(tiles, ci);

            if (_a == gdAlphaMax)
                continue;

            uint32_t colorhash = gdTrueColorAlpha(_r, _g, _b, _a);

            if (colormap.count(colorhash) != 0)
                continue;

            colormap[colorhash] = base.layers.size();
            base.layers.resize(base.layers.size() + 1);
            auto& l = base.layers.back();

            l.r = _r;
            l.g = _g;
            l.b = _b;
            l.bitmap.w = tw;
            l.bitmap.make_pitch();
            l.bitmap.bm.resize(l.bitmap.pitch * th);
        }

        unsigned int numtw = bw / tw;
        unsigned int numth = bh / th;

        for (unsigned int tyy = 0; tyy < numth; ++tyy) {
            for (unsigned int txx = 0; txx < numtw; ++txx) {

                uint32_t which = (tyy * numtw) + txx;

                out[which] = base;
                indexed_bitmap& what = out[which];

                for (unsigned int yy = 0; yy < th; ++yy) {
                    for (unsigned int xx = 0; xx < tw; ++xx) {

                        int colorix = gdImagePalettePixel(tiles, txx*tw + xx, tyy*th + yy);
                        
                        int _r = gdImageRed(tiles, colorix);
                        int _g = gdImageGreen(tiles, colorix);
                        int _b = gdImageBlue(tiles, colorix);
                        int _a = gdImageAlpha(tiles, colorix);

                        uint32_t colorhash = gdTrueColorAlpha(_r, _g, _b, _a);

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

    inline int map_color(uint8_t r, uint8_t g, uint8_t b) {
        return gdTrueColor(r, g, b);
    }

    inline int color_mul(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(((r*cr)/256), ((g*cg)/256), ((b*cb)/256));
    }

    inline int color_screen(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(256 - (((256-r)*(256-cr))/256.0), 
                         256 - (((256-g)*(256-cg))/256.0), 
                         256 - (((256-b)*(256-cb))/256.0));
    }

    inline int color_avg(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(((r+cr)/2), ((g+cg)/2), ((b+cb)/2));
    }

    inline void set_pixel(gdImagePtr pixels, unsigned int x, unsigned int y, int color) {
        gdImageTrueColorPixel(pixels, x, y) = color;
    }

    inline void fill_rect(gdImagePtr pixels, 
                          unsigned int to_x, unsigned int to_y, 
                          unsigned int to_w, unsigned int to_h, int bgc) {

        for (unsigned int yy = to_y; yy < to_h + to_y; ++yy) {
            for (unsigned int xx = to_x; xx < to_w + to_x; ++xx) {
                set_pixel(pixels, xx, yy, bgc);
            }
        }
    }

    inline void blit_bitmap(gdImagePtr pixels, 
                            unsigned int to_x, unsigned int to_y,
                            int fgc, const bdf::bitmap& bitmap) {

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

            for (int bit = 7; bit >= 0; --bit) {

                if (v & (1 << bit)) 
                    set_pixel(pixels, to_x + xx, to_y + yy, fgc);

                ++xx;

                if (xx >= bitmap.w) {
                    xx = 0;
                    ++yy;
                    break;
                }
            }
        }
    }

    void tile(unsigned int x, unsigned int y, 
              uint32_t ti, unsigned int cwidth, bool inverse, bool underline,
              uint8_t fr, uint8_t fg, uint8_t fb,
              uint8_t br, uint8_t bg, uint8_t bb) {

        if (ti == 0xfffd) {
            return;
        }

        if (x >= sw || y >= sh)
            throw std::runtime_error("Invalid screen offset in tile()");

        unsigned int to_x = x * tw;
        unsigned int to_y = y * th;
        unsigned int to_w = tw * cwidth;
        unsigned int to_h = th;

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

        int fgc = map_color(fr, fg, fb);
        int bgc = map_color(br, bg, bb);

        fill_rect(screen, to_x, to_y, to_w, to_h, bgc);

        bool did_tiles = false;

        // Draw pixel tiles.
        if (!tiles.empty()) {

            auto tmi = cfg.tile_mapping.find(ti);

            if (tmi == cfg.tile_mapping.end()) {

                uint32_t pixi = tmi->second;

                for (unsigned int cwi = 0; cwi < cwidth; ++cwi, ++pixi, to_x += tw) {

                    auto tmpi = tiles.find(pixi);
                    if (tmpi == tiles.end())
                        return;

                    for (const auto& l : tmpi->second.layers) {
                        int fgp = color_mul(l.r, l.g, l.b, fr, fg, fb);
                        blit_bitmap(screen, to_x, to_y, fgp, l.bitmap);
                    }
                }

                did_tiles = true;
            }
        }

        // Or else draw font bitmaps.

        if (!did_tiles) {

            auto gi = font.glyphs.find(ti);

            if (gi == font.glyphs.end())
                return;

            const auto& glyph = gi->second;

            blit_bitmap(screen, to_x, to_y, fgc, glyph);
        }

        if (underline) {

            for (unsigned int w = 0; w < to_w; ++w) {
                set_pixel(screen, to_x + w, to_y + to_h - 1, fgc);
            }
        }
    }

    void resize(unsigned int w, unsigned int h) {

        sw = w;
        sh = h;

        gdImageDestroy(screen);

        screen = gdImageCreateTrueColor(tw*sw, th*sh);
    }

    void render(Socket& browser_sock) {

        int size;
        void* buff = gdImageGifPtr(screen, &size);

        write_websocket_frame(browser_sock, buff, size);

        gdFree(buff);
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

    Tiler* tiler = (Tiler*)data;

    for (unsigned int i = 0; i < len; i += cwidth) {
        uint32_t c = ch[i];

        tiler->tile(posx+i, posy, c, cwidth, attr->inverse, attr->underline,
                    attr->fr, attr->fg, attr->fb,
                    attr->br, attr->bg, attr->bb);
    }

    if (len == 0) {
        for (unsigned int j = 0; j < cwidth; ++j) {
            tiler->tile(posx+j, posy, (uint32_t)' ', 1, attr->inverse, attr->underline,
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

    SOCKET& socket;

    config::Config& cfg;
    Tiler tiler;

    unsigned int sw;
    unsigned int sh;
    
    VTE(SOCKET& s, config::Config& _cfg) : screen(NULL), vte(NULL), socket(s), cfg(_cfg), tiler(cfg),
                                           sw(tiler.sw), sh(tiler.sh)
    {

        try {

            if (tsm_screen_new(&screen, tsm_logger_cb, NULL) < 0)
                throw std::runtime_error("Could not init tsm_screen");

            if (tsm_vte_new(&vte, screen, tsm_term_writer_cb<SOCKET>, &socket, tsm_logger_cb, NULL) < 0)
                throw std::runtime_error("Could not init tsm_vte");

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

        tiler.resize(sw, sh);
        
        if (tsm_screen_resize(screen, sw, sh) < 0)
            throw std::runtime_error("Could not resize screen");
    }

    void feed(const std::string& data) {

        tsm_vte_input(vte, data.data(), data.size());
    }

    void redraw(Socket& browser_sock) {

        //tsm_age_t age =
        tsm_screen_draw(screen, tsm_drawer_cb, &tiler);
        tiler.render(browser_sock);
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
    }

    Socket(bool, unsigned int port) : compression(false) {

        fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) {
            throw std::runtime_error("could not socket()");
        }

        int is_true = 1;
        if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &is_true, sizeof(is_true)) < 0)
            teardown("could not setsockopt(SO_REUSEADDR) : ");

        if (::setsockopt(fd, SOL_TCP, TCP_NODELAY, &is_true, sizeof(is_true)) < 0)
            teardown("could not setsockopt(TCP_NODELAY) : ");

        struct sockaddr_in addr;
        ::memset(&addr, 0, sizeof(addr));

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        addr.sin_addr.s_addr = 0;

        if (::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            teardown("could not bind() : ");

        if (::listen(fd, 1024) < 0)
            teardown("could not listen() : ");

    }

    Socket(int _fd) : fd(_fd), compression(false) {}

    ~Socket() {
        close();
    }

    void close() {

        if (fd >= 0) {
            ::shutdown(fd, SHUT_RDWR);
            ::close(fd);
        }
    }
    
    int accept() {

        int client = ::accept(fd, NULL, NULL);

        if (client < 0) {
            throw std::runtime_error("could not accept()");
        }

        return client;
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

    bool recv(std::string& out, bool& more) {

        bool ret = true;
        more = false;

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

        more = (compression_leftover.size() > 0);

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
};


struct Process {

    int fd;
    pid_t pid;

    Process(const std::vector<std::string>& cmd) {

        if (cmd.size() == 0)
            throw std::runtime_error("Need a command to run.");

        pid = forkpty(&fd, NULL, NULL, NULL);

        if (pid < 0)
            throw std::runtime_error("Could not forkpty()");

        if (pid == 0) {

            setenv("TERM", "xterm", 1);

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
        close();
    }

    void close() {
        if (fd >= 0) {
            ::close(fd);
            kill(pid, SIGTERM);
            int tmp;
            ::wait(&tmp);
            fd = -1;
        }
    }
    
    bool recv(std::string& out, bool& more) {

        more = false;

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
};


template <typename SOCKET>
struct Protocol_Base {

    VTE<SOCKET>& vte;

    bool schedule_resize;
    unsigned int sw;
    unsigned int sh;
    
    Protocol_Base(VTE<SOCKET>& _vte) : vte(_vte), schedule_resize(false), sw(0), sh(0) {}

    virtual void resizer(Socket&, unsigned int, unsigned int);
    
    void check_for_resize(Socket& browser_sock) {

        if (schedule_resize) {
            resizer(browser_sock, sw, sh);
            schedule_resize = false;
        }
    }

    // Input from the websocket.
    void input(const std::string& data) {

        static std::unordered_map<unsigned int, uint32_t> keycodes =
            { {  8, XKB_KEY_NoSymbol },
              {  9, XKB_KEY_NoSymbol },
              { 27, XKB_KEY_NoSymbol, },
              { 37, XKB_KEY_Left },
              { 38, XKB_KEY_Up },
              { 39, XKB_KEY_Right },
              { 40, XKB_KEY_Down },
              { 12, XKB_KEY_period },
              { 33, XKB_KEY_KP_Page_Up },
              { 34, XKB_KEY_KP_Page_Down },
              { 35, XKB_KEY_Select },
              { 36, XKB_KEY_Find },
              { 112, XKB_KEY_F1 },
              { 113, XKB_KEY_F2 },
              { 114, XKB_KEY_F3 },
              { 115, XKB_KEY_F4 },
              { 116, XKB_KEY_F5 },
              { 117, XKB_KEY_F6 },
              { 118, XKB_KEY_F7 },
              { 119, XKB_KEY_F8 },
              { 120, XKB_KEY_F9 },
              { 121, XKB_KEY_F10 },
              { 122, XKB_KEY_F11 },
              { 123, XKB_KEY_F12 } };
        
        if (data.empty())
            return;
        
        unsigned char t = data[0];

        if (t == 'k') {
            // Unicode input
            vte.socket.send(data.substr(1));

        } else if (t == 'c') {
            // Key scancode
            size_t n;
            unsigned int code = std::stoul(data.substr(1), &n);
            bool ctrl = std::stoul(data.substr(n+1));

            auto i = keycodes.find(code);

            if (ctrl || i != keycodes.end()) {

                uint32_t sym = (i == keycodes.end() ? XKB_KEY_NoSymbol : i->second);
                unsigned char chr = (sym == XKB_KEY_NoSymbol ? code : 0);
                unsigned int mods = (ctrl ? TSM_CONTROL_MASK : 0);
                tsm_vte_handle_keyboard(vte.vte, sym, chr, mods, chr);
            }

        } else if (t == 'r') {
            // Resize
            size_t n;
            unsigned int w = std::stoul(data.substr(1), &n);
            unsigned int h = std::stoul(data.substr(n+1));

            w = w / vte.tiler.tw;
            h = h / vte.tiler.th;
            
            schedule_resize = true;
            sw = w;
            sh = h;
        }
    }

};


template <typename SOCKET>
struct Protocol_Telnet : public Protocol_Base<SOCKET> {

    unsigned int polltimeout;
    bool enable_compression;

    using Protocol_Base<SOCKET>::vte;
    using Protocol_Base<SOCKET>::check_for_resize;

    Protocol_Telnet(VTE<SOCKET>& _vte, unsigned int pt, bool ec) : 
        Protocol_Base<SOCKET>(_vte), polltimeout(pt), enable_compression(ec) {}

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
    void resizer(Socket& browser_sock, unsigned int sw, unsigned int sh) {

        send_resize(sw, sh);
        vte.resize(sw, sh);
        vte.redraw(browser_sock);
    }

    // The meat of the telnet protocol.
    bool multiplexor(Socket& browser_sock) {

        static std::string buff;
        static std::string rewritten;

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
        bool more;

        while (1) {

            buff.resize(16*1024);

            check_for_resize(browser_sock);

            if (!vte.socket.recv(buff, more)) {

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
                        send_terminal_type("xterm");

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
        vte.redraw(browser_sock);

        check_for_resize(browser_sock);
        
        buff.clear();
        rewritten.clear();

        if (more) return multiplexor(browser_sock);

        return false;
    }
};


template <typename SOCKET>
struct Protocol_Pty : public Protocol_Base<SOCKET> {

    unsigned int polltimeout;

    using Protocol_Base<SOCKET>::vte;
    using Protocol_Base<SOCKET>::check_for_resize;

    Protocol_Pty(VTE<SOCKET>& _vte, unsigned int pt, bool ec) :
        Protocol_Base<SOCKET>(_vte), polltimeout(pt) {}

    void send_resize(unsigned int sw, unsigned int sh) {

        struct winsize ws;
        ws.ws_col = sw;
        ws.ws_row = sh;
        
        ioctl(vte.socket.fd, TIOCSWINSZ, &ws);
    }

    void resizer(Socket& browser_sock, unsigned int sw, unsigned int sh) {

        send_resize(sw, sh);
        vte.resize(sw, sh);
        vte.redraw(browser_sock);
    }

    bool multiplexor(Socket& browser_sock) {

        static std::string buff;

        bool more;

        buff.resize(16*1024);

        check_for_resize(browser_sock);

        if (!vte.socket.recv(buff, more)) {
            return true;
        }

        vte.feed(buff);
        vte.redraw(browser_sock);

        check_for_resize(browser_sock);

        buff.clear();

        if (more) return multiplexor(browser_sock);

        return false;
    }
};



void write_websocket_frame(Socket& browser_sock, void* buff, size_t len) {

    uint8_t magic[14] = {
        0x82, // Final frame with binary data.
        0x00, // Not masked.
        0x00, 0x00, // Length extension 1
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Length extension 2
    }; 

    size_t header_len;
    
    if (len <= 125) {
        magic[1] |= len & 0xFF;
        header_len = 2;

    } else if (len <= 65535) {
        magic[1] |= 126;
        magic[2] = (len >> 8) & 0xFF;
        magic[3] = len & 0xFF;
        header_len = 4;

    } else {
        magic[1] = 127;
        header_len = 10;

        size_t tmp = len;
        for (size_t i = 9; i >= 2; --i) {
            magic[i] = tmp & 0xFF;
            tmp = tmp >> 8;
        }
    }

    browser_sock.send((const char*)magic, header_len);
    browser_sock.send((const char*)buff, len);
}

template <typename PROTO>
bool process_websocket_frame(PROTO& proto, uint8_t opcode, const std::string& data) {

    if (opcode == 8)
        return false;

    if (opcode == 1 || opcode == 2) {
        proto.input(data);
    }

    return true;
}

template <typename PROTO>
void read_websocket_frames(Socket& browser_sock, PROTO& proto) {

    std::string buff;
    buff.resize(1024);
    bool more;

    enum {
        START,
        MASK_LEN,
        LENEXT,
        MASK,
        DATA
    } state = START;

    uint8_t head1 = 0;
    uint8_t head2 = 0;
    uint64_t len = 0;
    uint64_t run = 0;
    uint8_t mask[4] = { 0, 0, 0, 0 };

    std::string data;

    auto goto_mask_or_data = [&]() {

        if (head2 & 0x80) {
            state = MASK;
            run = 4;
        } else {
            state = DATA;
            run = len;
        }
    };
    
    while (1) {

        if (!browser_sock.recv(buff, more))
            break;

        for (unsigned int c : buff) {
            switch (state) {

            case START:
                head1 = c;
                state = MASK_LEN;
                break;

            case MASK_LEN:
                head2 = c;
                len = c & 0x7F;

                if (len <= 125) {
                    goto_mask_or_data();

                } else if (len == 126) {
                    state = LENEXT;
                    len = 0;
                    run = 2;

                } else if (len == 127) {
                    state = LENEXT;
                    len = 0;
                    run = 8;
                }
                break;

            case LENEXT:
                len = len << 8;
                len = len | c;

                --run;
                if (run == 0) {
                    goto_mask_or_data();
                }
                break;

            case MASK:
                mask[4-run] = c;

                --run;
                if (run == 0) {
                    state = DATA;
                    run = len;
                }
                break;

            case DATA:
                data += c;

                if (head2 & 0x80) {
                    data.back() ^= mask[(len-run) % 4];
                }

                --run;
                if (run == 0) {

                    if (head1 & 0x80) {

                        if (!process_websocket_frame(proto, head1 & 0x0F, data))
                            return;

                        data.clear();
                    }

                    state = START;
                }
            }
        }
    }
}


template <typename PROTO>
void websocket_reader(Socket& browser_sock, PROTO& proto) {

    // Read keyboard input from browser and send it to
    // program.
    
    try {
        read_websocket_frames(browser_sock, proto);

    } catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;

    } catch (...) {
    }

    // If the browser stops communicating, close connection
    // to program too.
    
    try {
        proto.vte.socket.close();
    } catch (...) {
    }
}

template <template <typename> class PROTO, typename SOCKET>
void mainloop_aux(Socket& browser_sock, SOCKET& term_sock, config::Config& cfg) {

    // Create a virtual terminal and connect it to a program
    // outputting VT100 codes.
    
    VTE<SOCKET> vte(term_sock, cfg);

    if (cfg.palette.size() > 0) {
        vte.set_palette(cfg.palette);
    }

    vte.set_cursor(cfg.cursor);

    PROTO<SOCKET> protocol(vte, cfg.polling_rate, cfg.compression);

    protocol.resizer(browser_sock, vte.tiler.sw, vte.tiler.sh);

    // Read keyboard input from browser and proxy it to the virtual terminal.

    std::thread thr(websocket_reader< PROTO<SOCKET> >, std::ref(browser_sock), std::ref(protocol));

    // Read (decode) data from the program and write it to
    // virtual terminal; capture virtual terminal output and send it
    // to browser.
    
    try {

        while (1) {
            if (protocol.multiplexor(browser_sock))
                break;
        }

    } catch (...) {

        browser_sock.close();
        thr.join();

        throw;
    }

    // When program stops communicating close socket to browser.

    browser_sock.close();
    thr.join();
}

void mainloop(Socket& browser_sock, const config::Config& _cfg) {

    config::Config cfg = _cfg;
    
    if (cfg.command.size() > 0) {

        Process proc(cfg.command);

        mainloop_aux<Protocol_Pty>(browser_sock, proc, cfg);

    } else {

        Socket telnet_sock(cfg.host, cfg.port);

        mainloop_aux<Protocol_Telnet>(browser_sock, telnet_sock, cfg);
    }
}


std::string base64_encode(const unsigned char* bytes, size_t len) {
    
    static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string ret;

    const unsigned char* end = bytes + len;
    
    while (bytes != end) {

        unsigned char b0 = 0;
        unsigned char b1 = 0;
        unsigned char b2 = 0;
        unsigned int marks = 0;

        b0 = *bytes;
        ++bytes;

        if (bytes == end) {
            marks = 2;

        } else {
            b1 = *bytes;
            ++bytes;

            if (bytes == end) {
                marks = 1;
            } else {
                b2 = *bytes;
                ++bytes;
            }
        }

        ret += base64_chars[(b0 & 0xFC) >> 2];
        ret += base64_chars[((b0 & 0x03) << 4) | ((b1 & 0xF0) >> 4)];

        if (marks == 2) {
            ret += '=';
        } else {
            ret += base64_chars[((b1 & 0x0F) << 2) | ((b2 & 0xC0) >> 6)];
        }

        if (marks == 1) {
            ret += '=';
        } else {
            ret += base64_chars[b2 & 0x3F];
        }
    }

    return ret;
}

bool handle_http_command(Socket& sock, const config::Config& cfg,
                         const std::string& method, const std::string& url,
                         const std::string& proto, const std::map<std::string,std::string>& headers) {

    bool ok = true;
    
    if (method != "GET")
        ok = false;

    if (proto != "HTTP/1.0" && proto != "HTTP/1.1")
        ok = false;

    if (url != "/run")
        ok = false;

    auto hi = headers.find("sec-websocket-key");

    if (hi == headers.end())
        ok = false;

    if (!ok) {

        static const std::string error = "HTTP/1.1 404 Not found\r\n\r\n";
        sock.send(error);
        return false;
    }

    std::string bullshit = hi->second + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    unsigned char sha[20];
    sha1::calc(bullshit.c_str(), bullshit.size(), sha);

    bullshit = base64_encode(sha, 20);

    static const std::string reply =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: ";

    std::string tmp = reply;
    tmp += bullshit;
    tmp += "\r\n\r\n";

    sock.send(tmp);

    mainloop(sock, cfg);

    return true;
}


void read_http_command_aux(Socket& sock, const config::Config& cfg) {

    std::string buff;
    buff.resize(1024);
    bool more;

    enum {
        METHOD,
        URL,
        PROTO,
        HEADER_START,
        HEADER_KEY,
        HEADER_VAL
    } state = METHOD;
        

    std::string method;
    std::string url;
    std::string proto;
    std::string header_key;
    std::string header_val;
    std::map<std::string,std::string> headers;

    // This parser is non-compliant: it doesn't handle whitespace within
    // header values or duplicate headers correctly.

    auto add_header = [&]() {

        if (!header_key.empty() || !header_val.empty())
            headers[header_key] = header_val;

        header_key.clear();
        header_val.clear();
    };
    
    while (1) {

        if (!sock.recv(buff, more))
            break;

        for (unsigned char c : buff) {
            switch (state) {

            case METHOD:
                if (c == ' ') state = URL;
                else method += c;
                break;

            case URL:
                if (c == ' ') state = PROTO;
                else url += c;
                break;

            case PROTO:
                if (c == '\r') {}
                else if (c == '\n') state = HEADER_START;
                else proto += c;
                break;

            case HEADER_START:
                if (c == '\r') {}
                else if (c == '\n') {

                    add_header();

                    if (!handle_http_command(sock, cfg, method, url, proto, headers)) {
                        return;
                    } else {
                        method.clear();
                        url.clear();
                        proto.clear();
                        headers.clear();
                    }
                }    
                else if (c == ' ' || c == '\t') state = HEADER_VAL;
                else {

                    add_header();
                    header_key += ::tolower(c);
                    state = HEADER_KEY;
                }
                break;

            case HEADER_KEY:
                if (c == ':') state = HEADER_VAL;
                else header_key += ::tolower(c);
                break;

            case HEADER_VAL:
                if (c == '\r') {}
                else if (c == '\n') state = HEADER_START;
                else if (c == ' ' || c == '\t') {}
                else header_val += c; 
            }
        }
    }
}

void read_http_command(int fd, const config::Config& cfg) {

    try {

        Socket sock(fd);
        read_http_command_aux(sock, cfg);

    } catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;

    } catch (...) {
    }
}

void usage(const std::string& argv0) {
    std::cerr << "Usage: " << argv0 << " [--config <cfgfile>] [host] [port]" << std::endl;
}

int main(int argc, char** argv) {

    try {

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

        Socket server(true, 8080);

        while (1) {

            int client = server.accept();
            std::thread thr(read_http_command, client, std::cref(cfg));
            thr.detach();
        }
        
    } catch (std::exception& e) {
        std::cout << "Fatal Error: " << e.what() << std::endl;
        return 1;

    } catch (...) {
        std::cout << "Unknown fatal error." << std::endl;
        return 1;
    }

    return 0;
}
