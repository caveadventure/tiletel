
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>

#include <png.h>

#include "libtsm/src/libtsm.h"

extern "C" {
#include "libtsm/src/libtsm_int.h"
}

#include "external/xkbcommon-keysyms.h"

#include "bdf.h"
#include "config.h"

#include <sys/time.h>

#include "lz77.h"

#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <stdlib.h>
#include <unistd.h>
#include <pty.h>


struct Tiler {

    config::Config& cfg;

    unsigned int tw;
    unsigned int th;
    unsigned int sw;
    unsigned int sh;

    struct pixel_t {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    std::vector<pixel_t> screen;

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
        sw(cfg.screen_width), sh(cfg.screen_height) {

        if (!cfg.tiles.empty()) {

            png_image rtiles;
            ::memset((void*)&rtiles, 0, sizeof(png_image));

            rtiles.version = PNG_IMAGE_VERSION;
            
            if (png_image_begin_read_from_file(&rtiles, cfg.tiles.c_str()) != 0)
                throw std::runtime_error("Could not read PNG header for " + cfg.files);

            rtiles.format = PNG_FORMAT_RGBA_COLORMAP;

            std::vector<uint8_t> tilesb;
            tilesb.resize(PNG_IMAGE_SIZE(rtiles));

            std::vector<uint8_t> colors;
            colors.resize(PNG_IMAGE_COLORMAP_SIZE(rtiles));

            if (png_image_finish_read(&rtiles, NULL, (png_bytep*)tilesb.data(),
                                      0, (png_bytep*)colors.data()) != 0)
                throw std::runtime_error("Could not read PNG data for " + cfg.tiles);

            surface_to_indexed(rtiles.width, rtiles.height, tilesb, colors, tw, th, tiles);

            png_free_image(&rtiles);
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

        screen.resize(tw*sw*th*sh);
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

    static void surface_to_indexed(unsigned int bw, unsigned int bh, const std::vector<uint8_t>& pixels, const std::vector<uint8_t>& colors,
                                   unsigned int tw, unsigned int th, std::unordered_map<uint32_t, indexed_bitmap>& out) {

        if ((bw % tw) != 0 || (bh % th) != 0) {

            throw std::runtime_error("Size of tiles image does not match tile size");
        }

        indexed_bitmap base;

        std::unordered_map<uint32_t, size_t> colormap;

        for (int ci = 0; ci < colors.size(); ci += 4) {

            if (colors[ci+3] == 0x00)
                continue;

            uint32_t colorhash = (colors[ci] << 24) | (colors[ci+1] << 16) | (colors[ci+2] << 8) | (colors[ci+3]);

            if (colormap.count(colorhash) != 0)
                continue;

            colormap[colorhash] = base.layers.size();
            base.layers.resize(base.layers.size() + 1);
            auto& l = base.layers.back();

            l.r = colors[ci];
            l.g = colors[ci+1];
            l.b = colors[ci+2];
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

                        int colorix = pixels[txx*tw + xx, tyy*th + yy];
                        uint32_t colorhash = (colors[colorix] << 24) | (colors[colorix+1] << 16) | 
                                              (colors[colorix+2] << 8) | (colors[colorix+3]);

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

    inline pixel_t map_color(uint8_t r, uint8_t g, uint8_t b) {
        return pixel_t{r, g, b, 0xFF};
    }

    inline pixel_t color_mul(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(((r*cr)/256), ((g*cg)/256), ((b*cb)/256));
    }

    inline pixel_t color_screen(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(256 - (((256-r)*(256-cr))/256.0), 
                         256 - (((256-g)*(256-cg))/256.0), 
                         256 - (((256-b)*(256-cb))/256.0));
    }

    inline pixel_t color_avg(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t r, uint8_t g, uint8_t b) {
        return map_color(((r+cr)/2), ((g+cg)/2), ((b+cb)/2));
    }

    inline pixel_t* cursor(pixel_t* pixels, unsigned int pitch, unsigned int x, unsigned int y) {
        return pixels + (y * pitch) + x;
    }

    inline void set_pixel(pixel_t* pixels, unsigned int pitch, unsigned int x, unsigned int y, pixel_t color) {
        *cursor(pixels, pitch, x, y) = color;
    }

    inline void fill_rect(pixel_t* pixels, unsigned int pitch, 
                          unsigned int to_x, unsigned int to_y, 
                          unsigned int to_w, unsigned int to_h, pixel_t bgc) {

        for (unsigned int yy = to_y; yy < to_h + to_y; ++yy) {

            pixel_t* px = cursor(pixels, pitch, to_x, yy);

            for (unsigned int xx = 0; xx < to_w; ++xx, ++px) {
                *px = bgc;
            }
        }
    }

    inline void blit_bitmap(pixel_t* pixels, unsigned int pitch, 
                            unsigned int to_x, unsigned int to_y,
                            pixel_t fgc, const bdf::bitmap& bitmap) {

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

            pixel_t* px = cursor(pixels, pitch, to_x + xx, to_y + yy);

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

        pixel_t fgc = map_color(fr, fg, fb);
        pixel_t bgc = map_color(br, bg, bb);

        int pitch = tw*sw;
        pixel_t* pixels = screen.data();

        fill_rect(pixels, pitch, to_x, to_y, to_w, to_h, bgc);

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
                        pixel_t fgp = color_mul(l.r, l.g, l.b, fr, fg, fb);
                        blit_bitmap(pixels, pitch, to_x, to_y, fgp, l.bitmap);
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

            blit_bitmap(pixels, pitch, to_x, to_y, fgc, glyph);
        }

        if (underline) {

            pixel_t* px = cursor(pixels, pitch, to_x, to_y + to_h - 1);

            for (unsigned int w = 0; w < to_w; ++w, ++px) {
                *px = fgc;
            }
        }
    }

    void resize(unsigned int w, unsigned int h) {

        sw = w / tw;
        sh = h / th;

        std::vector<pixel_t> tmp;
        tmp.resize(tw*sw*th*sh);
        screen.swap(tmp);
    }

    void render() {

        png_image out;
        ::memset((void*)&out, 0, sizeof(png_image));

        out.version = PNG_IMAGE_VERSION;
        out.format = PNG_FORMAT_RGBA;
        out.width = tiler.tw*tiler.sw;
        out.height = tiler.th*tiler.sh;

        png_image_write_to_file(&out, "out.png", 0,
                                (void*)tiler.screen.data(), sizeof(Tiler::pixel_t)*tiler.tw*tiler.sw,
                                NULL);
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
    
    VTE(SOCKET& s, config::Config& _cfg) : screen(NULL), vte(NULL), socket(s), cfg(_cfg), tiler(cfg), sw(tiler.sw), sh(tiler.sh)
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

        resize(tiler.sw, tiler.sh);
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

        //tsm_age_t age =
        tsm_screen_draw(screen, tsm_drawer_cb, &tiler);
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

        ::memcpy((void*)*serv_addr.sin_addr.s_addr, (void*)server->h_addr, server->h_length);
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
        if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, SETSOCKOPT_TYPE_CAST &is_true, sizeof(is_true)) < 0)
            teardown("could not setsockopt(SO_REUSEADDR) : ");

        if (::setsockopt(fd, SOL_TCP, TCP_NODELAY, SETSOCKOPT_TYPE_CAST &is_true, sizeof(is_true)) < 0)
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

        ssize_t tmp = ::recv(fd, (void*)out.data(), out.size(), 0);

        if (i < 0)
            throw std::runtime_error("Error receiving data");

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
        close(fd);
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

    Protocol_Base(VTE<SOCKET>& _vte) : vte(_vte) {}
};


template <typename SOCKET>
struct Protocol_Telnet : public Protocol_Base<SOCKET> {

    unsigned int polltimeout;
    bool enable_compression;

    using Protocol_Base<SOCKET>::vte;

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
    void resizer(unsigned int sw, unsigned int sh) {

        send_resize(sw, sh);
        vte.resize(sw, sh);
        vte.redraw();
    }


    // The meat of the telnet protocol.
    bool multiplexor() {

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
        vte.redraw();

        buff.clear();
        rewritten.clear();

        if (more) return multiplexor();

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

        bool more;

        buff.resize(16*1024);

        if (!vte.socket.recv(buff, more)) {
            return true;
        }

        vte.feed(buff);
        vte.redraw();

        buff.clear();

        if (more) return multiplexor();

        return false;
    }
};


template <typename SOCKET>
void mainloop(SOCKET& sock, config::Config& cfg) {

    VTE<SOCKET> vte(sock, cfg);

    if (cfg.palette.size() > 0) {
        vte.set_palette(cfg.palette);
    }

    vte.set_cursor(cfg.cursor);

    Protocol_Pty<SOCKET> protocol(vte, cfg.polling_rate, cfg.compression);

    protocol.resizer(vte.tiler.sw, vte.tiler.sh);

    while (1) {
        if (protocol.multiplexor())
            break;
    }
}

void handle_http_command(Socket& sock, const std::string& method, const std::string& url, const std::string& proto) {

    if (method != "GET")
        return;

    if (proto != "HTTP/1.0" && proto != "HTTP/1.1")
        return;

    
}


void read_http_command(Socket& sock) {

    std::string buff;
    buff.resize(1024);
    bool more;

    enum {
        METHOD,
        URL,
        PROTO,
        HEADERS_START,
        HEADERS
    } state = METHOD;
        

    std::string method;
    std::string url;
    std::string proto;
    
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
                if (c == '\r') // nothing
                else if (c == '\n') state = HEADERS_START;
                else proto += c;
                break;

            case HEADERS_START:
                if (c == '\r') // nothing
                else if (c == '\n') handle_http_command(sock, method, url, proto);
                else state = HEADERS;
                break;

            case HEADERS:
                if (c == '\n') state = HEADERS_START;
                break;
            }
        }
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

            Socket client(server.accept());

            read_http_command(client);
        }

        if (cfg.command.size() > 0) {

            Process proc(cfg.command);

            mainloop(proc, cfg);

        } else {

            Socket sock(cfg.host, cfg.port);

            mainloop(sock, cfg);
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
