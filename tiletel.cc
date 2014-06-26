
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>

#include <QtGui>
#include <QtNetwork>

#include "libtsm/src/libtsm.h"

extern "C" {
#include "libtsm/src/libtsm_int.h"
}

#include "external/xkbcommon-keysyms.h"

#include "bdf.h"
#include "config.h"

#include <sys/time.h>

#include "lz77.h"


#ifdef HAVE_PTY_SUPPORT

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <pty.h>

#endif

struct Tiler {

    config::Config& cfg;

    unsigned int tw;
    unsigned int th;
    unsigned int sw;
    unsigned int sh;

    QImage screen;

    typedef uint32_t pixel_t;

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

            QImage rtiles(QString(cfg.tiles.c_str()));

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

        screen = QImage(tw*sw, th*sh, QImage::Format_RGB32);
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


    static void surface_to_indexed(const QImage& s, unsigned int tw, unsigned int th,
                                   std::unordered_map<uint32_t, indexed_bitmap>& out) {

        if ((s.width() % tw) != 0 || (s.height() % th) != 0) {

            throw std::runtime_error("Size of tiles image does not match tile size");
        }

        if (s.format() != QImage::Format_Indexed8)
            throw std::runtime_error("Only indexed (i.e., colormap or palette) tiles are supported");

        indexed_bitmap base;

        std::unordered_map<uint32_t, size_t> colormap;

        for (int ci = 0; ci < s.colorCount(); ++ci) {

            QRgb color = s.color(ci);

            if (qAlpha(color) == 0x00)
                continue;

            uint32_t colorhash = (qRed(color) << 24) | (qGreen(color) << 16) | (qBlue(color) << 8) | (qAlpha(color));

            if (colormap.count(colorhash) != 0)
                continue;

            colormap[colorhash] = base.layers.size();
            base.layers.resize(base.layers.size() + 1);
            auto& l = base.layers.back();

            l.r = qRed(color);
            l.g = qGreen(color);
            l.b = qBlue(color);
            l.bitmap.w = tw;
            l.bitmap.make_pitch();
            l.bitmap.bm.resize(l.bitmap.pitch * th);
        }

        unsigned int numtw = s.width() / tw;
        unsigned int numth = s.height() / th;

        for (unsigned int tyy = 0; tyy < numth; ++tyy) {
            for (unsigned int txx = 0; txx < numtw; ++txx) {

                uint32_t which = (tyy * numtw) + txx;

                out[which] = base;
                indexed_bitmap& what = out[which];

                for (unsigned int yy = 0; yy < th; ++yy) {
                    for (unsigned int xx = 0; xx < tw; ++xx) {

                        int colorix = s.pixelIndex(txx*tw + xx, tyy*th + yy);
                        QRgb color = s.color(colorix);
                        uint32_t colorhash = (qRed(color) << 24) | (qGreen(color) << 16) | (qBlue(color) << 8) | (qAlpha(color));

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
        uint32_t _r = 0xFF000000 | (r << 16) | (g << 8) | b;
        return (pixel_t)_r;
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

    inline pixel_t* cursor(unsigned char* pixels, unsigned int pitch, unsigned int x, unsigned int y) {
        return (pixel_t*)((uint8_t*)pixels + (y * pitch) + (x * sizeof(pixel_t)));
    }

    inline void set_pixel(unsigned char* pixels, unsigned int pitch, unsigned int x, unsigned int y, pixel_t color) {
        *cursor(pixels, pitch, x, y) = color;
    }

    inline void fill_rect(unsigned char* pixels, unsigned int pitch, 
                          unsigned int to_x, unsigned int to_y, 
                          unsigned int to_w, unsigned int to_h, pixel_t bgc) {

        for (unsigned int yy = to_y; yy < to_h + to_y; ++yy) {

            pixel_t* px = cursor(pixels, pitch, to_x, yy);

            for (unsigned int xx = 0; xx < to_w; ++xx, ++px) {
                *px = bgc;
            }
        }
    }

    inline void blit_bitmap(unsigned char* pixels, unsigned int pitch, 
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
              uint32_t ti, unsigned int cwidth, bool inverse,
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

        int pitch = screen.bytesPerLine();
        unsigned char* pixels = screen.bits();

        fill_rect(pixels, pitch, to_x, to_y, to_w, to_h, bgc);

        // Draw pixel tiles.
        if (!tiles.empty()) {

            auto tmi = cfg.tile_mapping.find(ti);

            if (tmi != cfg.tile_mapping.end()) {

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

                return;
            }
        }

        // Or else draw font bitmaps.

        auto gi = font.glyphs.find(ti);

        if (gi == font.glyphs.end())
            return;

        const auto& glyph = gi->second;

        blit_bitmap(pixels, pitch, to_x, to_y, fgc, glyph);
    }

    void resize(unsigned int w, unsigned int h) {

        sw = w / tw;
        sh = h / th;
        screen = QImage(w, h, QImage::Format_RGB32);
    }
};


template <typename PROTO>
class RasterWindow : public QWindow {

public:

    PROTO& protocol;
    bool done;

    QBackingStore backingStore;

    config::Config& cfg;
    Tiler tiler;

    explicit RasterWindow(PROTO& p, config::Config& _cfg, QWindow* parent = 0) : 
        QWindow(parent), 
        protocol(p),
        done(false),
        backingStore(this),
        cfg(_cfg), 
        tiler(cfg) {

        create();

        unsigned int ww = tiler.tw*tiler.sw;
        unsigned int hh = tiler.th*tiler.sh;

        resize(ww, hh);
        backingStore.resize(QSize(ww, hh));
    }

    void renderNow() {

        if (!isExposed())
            return;

        QRect rect(0, 0, width(), height());
        backingStore.beginPaint(rect);

        QPaintDevice* device = backingStore.paintDevice();
        QPainter painter(device);

        painter.drawImage(0, 0, tiler.screen);

        backingStore.endPaint();
        backingStore.flush(rect);
    }

    bool event(QEvent* event) {

        if (event->type() == QEvent::UpdateRequest) {
            renderNow();
            return true;

        } else if (event->type() == QEvent::KeyPress) {
            keyEvent((QKeyEvent*)event);
            return true;

        } else if (event->type() == QEvent::Close) {
            done = true;
        }

        return QWindow::event(event);
    }

    void resizeEvent(QResizeEvent* event) {

        const auto& s = event->size();

        tiler.resize(s.width(), s.height());

        backingStore.resize(s);

        if (isExposed())
            renderNow();

        protocol.resizer(tiler.sw, tiler.sh);
    }

    void exposeEvent(QExposeEvent* event) {

        if (isExposed()) {
            renderNow();
        }
    }

    void keyEvent(QKeyEvent* event) {

        if (event->text().size() == 0) {
            protocol.keypressor(event->key(), event->modifiers());

        } else {
            protocol.keypressor(event->text().toUcs4());
        }
    }

    void readyRead() {
        done = protocol.multiplexor();
        renderNow();
    }

};

// Horrible !!HACK!!

static Tiler* GLOBAL_TILER = NULL;


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

        tiler->tile(posx+i, posy, c, cwidth, attr->inverse,
                    attr->fr, attr->fg, attr->fb,
                    attr->br, attr->bg, attr->bb);
    }

    if (len == 0) {
        for (unsigned int j = 0; j < cwidth; ++j) {
            tiler->tile(posx+j, posy, (uint32_t)' ', 1, attr->inverse,
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

    unsigned int sw;
    unsigned int sh;
    
    VTE(SOCKET& s) : screen(NULL), vte(NULL), socket(s), sw(0), sh(0)
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

        if (tsm_screen_resize(screen, sw, sh) < 0)
            throw std::runtime_error("Could not resize screen");
    }

    void feed(const std::string& data) {

        tsm_vte_input(vte, data.data(), data.size());
    }

    void redraw() {

        //tsm_age_t age =
        tsm_screen_draw(screen, tsm_drawer_cb, GLOBAL_TILER);
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

    QTcpSocket socket;

    bool compression;
    std::string compression_leftover;
    lz77::decompress_t decompressor;

    Socket(const std::string& host, unsigned int port) : compression(false) {

        socket.connectToHost(QString(host.c_str()), port);
    }

    bool recv_raw(std::string& out) {

        int i = socket.read((char*)out.data(), out.size());

        if (i < 0)
            throw std::runtime_error("Error receiving data");

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

    void send(const std::string& in) {

        int i = socket.write((char*)in.data(), in.size());

        if (i != (int)in.size())
            throw std::runtime_error("Error sending data");
    }

    void send(const char* data, size_t len) {

        int i = socket.write(data, len);

        if (i != (int)len)
            throw std::runtime_error("Error sending data");
    }

    bool poll(unsigned int wait = 100) {

        if (compression_leftover.size() > 0) {
            return true;
        }

        return socket.waitForReadyRead(wait);
    }
};

#ifdef HAVE_PTY_SUPPORT

struct Process {

    int fd;
    pid_t pid;
    struct pollfd poller;

    Process(const std::vector<std::string>& cmd) {

        if (cmd.size() == 0)
            throw std::runtime_error("Need a command to run.");

        pid = forkpty(&fd, NULL, NULL, NULL);

        if (pid < 0)
            throw std::runtime_error("Could not forkpty()");

        poller.fd = fd;
        poller.events = POLLIN;

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

#endif

template <typename SOCKET>
struct Protocol_Base {

    VTE<SOCKET>& vte;

    Protocol_Base(VTE<SOCKET>& _vte) : vte(_vte) {}

    // Feeds keyboard text input to the terminal emulator.
    template <typename UCS>
    void keypressor(const UCS& ucs) {

        for (uint32_t glyph : ucs) {

            tsm_vte_handle_keyboard(vte.vte, XKB_KEY_NoSymbol, glyph, 0, glyph);
        }
    }

    // Feeds raw keypress data to the terminal emulator.
    void keypressor(int key, Qt::KeyboardModifiers qmods) {

        unsigned char xkey = (key > 127 ? '?' : key);

        if (xkey == 0)
            return;

        unsigned int mods = 0;

        if (qmods & Qt::ShiftModifier) {
            mods |= TSM_SHIFT_MASK;
        }

        if (qmods & Qt::AltModifier) {
            mods |= TSM_ALT_MASK;
        }

        if (qmods & Qt::ControlModifier) {
            mods |= TSM_CONTROL_MASK;
        }

        uint32_t tsmsym = XKB_KEY_NoSymbol;

        if (qmods & Qt::KeypadModifier) {

            switch (key) {
            case Qt::Key_1:
                tsmsym = XKB_KEY_Select;
                break;
            case Qt::Key_2:
                tsmsym = XKB_KEY_KP_Down;
                break;
            case Qt::Key_3:
                tsmsym = XKB_KEY_KP_Page_Down;
                break;
            case Qt::Key_4:
                tsmsym = XKB_KEY_KP_Left;
                break;
            case Qt::Key_5:
                tsmsym = XKB_KEY_period;
                break;
            case Qt::Key_6:
                tsmsym = XKB_KEY_KP_Right;
                break;
            case Qt::Key_7:
                tsmsym = XKB_KEY_Find;
                break;
            case Qt::Key_8:
                tsmsym = XKB_KEY_KP_Up;
                break;
            case Qt::Key_9:
                tsmsym = XKB_KEY_KP_Page_Up;
                break;
            }

        } else {

            switch (key) {
            case Qt::Key_Insert:
                tsmsym = XKB_KEY_Insert;
                break;
            case Qt::Key_Home:
                tsmsym = XKB_KEY_Home;
                break;
            case Qt::Key_PageUp:
                tsmsym = XKB_KEY_Page_Up;
                break;
            case Qt::Key_End:
                tsmsym = XKB_KEY_End;
                break;
            case Qt::Key_PageDown:
                tsmsym = XKB_KEY_Page_Down;
                break;
            case Qt::Key_Right:
                tsmsym = XKB_KEY_Right;
                break;
            case Qt::Key_Left:
                tsmsym = XKB_KEY_Left;
                break;
            case Qt::Key_Down:
                tsmsym = XKB_KEY_Down;
                break;
            case Qt::Key_Up:
                tsmsym = XKB_KEY_Up;
                break;

            case Qt::Key_F1:
                tsmsym = XKB_KEY_F1;
                break;
            case Qt::Key_F2:
                tsmsym = XKB_KEY_F2;
                break;
            case Qt::Key_F3:
                tsmsym = XKB_KEY_F3;
                break;
            case Qt::Key_F4:
                tsmsym = XKB_KEY_F4;
                break;
            case Qt::Key_F5:
                tsmsym = XKB_KEY_F5;
                break;
            case Qt::Key_F6:
                tsmsym = XKB_KEY_F6;
                break;
            case Qt::Key_F7:
                tsmsym = XKB_KEY_F7;
                break;
            case Qt::Key_F8:
                tsmsym = XKB_KEY_F8;
                break;
            case Qt::Key_F9:
                tsmsym = XKB_KEY_F9;
                break;
            case Qt::Key_F10:
                tsmsym = XKB_KEY_F10;
                break;
            case Qt::Key_F11:
                tsmsym = XKB_KEY_F11;
                break;
            case Qt::Key_F12:
                tsmsym = XKB_KEY_F12;
                break;
            default:
                return;
            }
        }

        tsm_vte_handle_keyboard(vte.vte, tsmsym, xkey, mods, xkey);
    }
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

        while (1) {

            buff.resize(16*1024);

            if (!vte.socket.recv(buff)) {

                return true;
            }

            rewritten.reserve(buff.size());

            for (char c : buff) {

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

                    } else if (c == '\x54') {
                        vte.socket.compression = true;
                    }
                    break;

                case SB_IAC:

                    if (c == '\xF0') {
                        telnetstate = STREAM;

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

                    if (c == '\x54' && enable_compression) {
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

#ifdef HAVE_PTY_SUPPORT

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

#endif

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


        QGuiApplication app(argc, argv);

#ifdef HAVE_PTY_SUPPORT

        if (cfg.command.size() > 0) {

            Process proc(cfg.command);

            VTE<Process> vte(proc);

            if (cfg.palette.size() > 0) {
                vte.set_palette(cfg.palette);
            }

            vte.set_cursor(cfg.cursor);

            Protocol_Pty<Process> protocol(vte, cfg.polling_rate, cfg.compression);

            RasterWindow< Protocol_Pty<Process> > screen(protocol, cfg);
            screen.show();
            GLOBAL_TILER = &(screen.tiler);

            protocol.resizer(screen.tiler.sw, screen.tiler.sh);

            QSocketNotifier notify(proc.fd, QSocketNotifier::Read);
            QObject::connect(&notify, &QSocketNotifier::activated, &screen, &RasterWindow< Protocol_Pty<Process> >::readyRead);
        
            app.exec();

            return 0;
        }

#endif

        Socket sock(cfg.host, cfg.port);

        VTE<Socket> vte(sock);

        if (cfg.palette.size() > 0) {
            vte.set_palette(cfg.palette);
        }

        vte.set_cursor(cfg.cursor);

        Protocol_Telnet<Socket> protocol(vte, cfg.polling_rate, cfg.compression);
        
        RasterWindow< Protocol_Telnet<Socket> > screen(protocol, cfg);
        screen.show();
        GLOBAL_TILER = &(screen.tiler);

        protocol.resizer(screen.tiler.sw, screen.tiler.sh);

        QObject::connect(&sock.socket, &QTcpSocket::readyRead, &screen, &RasterWindow< Protocol_Telnet<Socket> >::readyRead);

        app.exec();

    } catch (std::exception& e) {
        std::cout << "Fatal Error: " << e.what() << std::endl;
        return 1;

    } catch (...) {
        std::cout << "Unknown fatal error." << std::endl;
        return 1;
    }

    return 0;
}
