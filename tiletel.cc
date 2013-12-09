
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_net.h"

#include "libtsm/src/libtsm.h"
#include "external/xkbcommon-keysyms.h"

#include "bdf.h"
#include "config.h"

#include <sys/time.h>


struct bm {
    struct timeval b;
    std::string msg;
    bm(const std::string& s) : msg(s) {
        gettimeofday(&b, NULL);
    }
    ~bm() {
        struct timeval e;
        gettimeofday(&e, NULL);
        size_t a = (e.tv_sec*1e6 + e.tv_usec);
        size_t q = (b.tv_sec*1e6 + b.tv_usec);
        std::cout << msg << ": " << ((double)a-(double)q)/1e6 << std::endl;
    }
};


struct Screen {

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* screen;

    bdf::Font font;

    unsigned int tw;
    unsigned int th;

    unsigned int sw;
    unsigned int sh;

    unsigned int tiles_png_w;
    unsigned int tiles_png_h;

    bool done;

    // In the unlikely event that someone is reading this:
    // SDL is a massive piece of shit.
    // Why doesn't a sane cross-platform frame-buffer library exist?
    // I don't need your half-assed thin API over OpenGL, I need access to pixels.

    struct indexed_bitmap_t {
        struct layer {
            SDL_Color color;
            std::vector<SDL_Point> points;
        };

        std::vector<layer> layers;
    };

    std::unordered_map<uint32_t, indexed_bitmap_t> tiles;
    

    Screen(const config::Config& cfg) : 
        window(NULL), renderer(NULL), screen(NULL), 
        tw(cfg.tile_width), th(cfg.tile_height), 
        sw(cfg.screen_width), sh(cfg.screen_height), 
        done(false)
    {

        SDL_Surface* rtiles = NULL;

        try {

            if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
                throw std::runtime_error("Could not init SDL");

            window = SDL_CreateWindow("tiletel",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      tw*sw, th*sh,
                                      SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN|
                                      (cfg.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

            if (window == NULL)
                throw std::runtime_error("Could not create window.");

            screen = SDL_GetWindowSurface(window);

            if (screen == NULL)
                throw std::runtime_error("Could not create window surface");

            renderer = SDL_CreateSoftwareRenderer(screen);

            if (renderer == NULL)
                throw std::runtime_error("Could not create renderer");

            if (IMG_Init(IMG_INIT_PNG) == 0)
                throw std::runtime_error("Could not init SDL_Image");

            rtiles = IMG_Load(cfg.tiles.c_str());

            if (rtiles == NULL)
                throw std::runtime_error("Failed to load tiles bitmap: '" + cfg.tiles + "'");

            if ((rtiles->w % tw) != 0 || 
                (rtiles->h % th) != 0) {

                throw std::runtime_error("Size of tiles image does not match tile size");
            }

            tiles_png_w = rtiles->w / tw;
            tiles_png_h = rtiles->h / th;


            std::cout << "!!! " << (int)rtiles->format->BitsPerPixel << " " << (int)rtiles->format->BytesPerPixel << std::endl;

            SDL_FreeSurface(rtiles);
            rtiles = NULL;

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


        } catch (...) {

            if (rtiles != NULL)
                SDL_FreeSurface(rtiles);

            if (renderer != NULL)
                SDL_DestroyRenderer(renderer);

            if (window != NULL)
                SDL_DestroyWindow(window);

            SDL_Quit();
            throw;
        }
    }

    ~Screen() {

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    static void surface_to_indexed(SDL_Surface* s, unsigned int tw, unsigned int th,
                                   std::unordered_map<uint32_t, indexed_bitmap_t>& out) {

        if (!SDL_ISPIXELFORMAT_INDEXED(s->format->format))
            throw std::runtime_error("Only indexed (i.e., colormap or palette) tiles are supported");

        if (s->format->BitsPerPixel != 8)
            throw std::runtime_error("Sanity error: SDL BitsPerPixel should be 8");

        SDL_Palette* palette = s->format->palette;

        indexed_bitmap_t base;

        base.layers.resize(palette->ncolors);

        for (int ci = 0; ci < palette->ncolors; ++ci) {
            base.layers[ci].color = palette->colors[ci];
        }

        uint8_t* pixels = s->pixels;
        for (unsigned int y = 0; y < s->h; ++y) {

            uint8_t* row = pixels + y * s->pitch;
            for (unsigned int x = 0; x < s->w; ++x) {

                uint8_t pixel = *(row + x);

                uint32_t which = (y / th) * (s->w / tw) + (x / tw);

                if (out.count(which) == 0) {
                    out[which] = base;
                }

                indexed_bitmap_t& what = out[which];

                what.layers[pixel].push_back(SDL_Point{x, y});
            }
        }
    }


    void tile(unsigned int x, unsigned int y, uint32_t ti, unsigned int cwidth, bool inverse,
              uint8_t fr, uint8_t fg, uint8_t fb,
              uint8_t br, uint8_t bg, uint8_t bb) {


        if (x >= sw || y >= sh)
            throw std::runtime_error("Invalid screen offset in tile()");


        SDL_Rect to;
        to.x = x * tw;
        to.y = y * th;
        to.w = tw * cwidth;
        to.h = th;

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

        SDL_SetRenderDrawColor(renderer, br, bg, bb, 0xFF);
        SDL_RenderFillRect(renderer, &to);

#if 0
        if (ti == 0x6728 || ti == 0x3013 || ti == 0x3001) {

            uint32_t ci = (fr << 16) | (fg << 8) | fb;

            SDL_Surface* true_tiles = tiles;

            if (0 && ncolors != 0) {

                palette_t& p = palettes[ci];

                if (p.p == NULL) {

                    p.p = SDL_ConvertSurface(tiles, tiles->format, tiles->flags);
                    if (p.p == NULL) {

                        throw std::runtime_error("Could not copy tiles for palette change");
                    }

                    SDL_Color tmp;
                    tmp.a = 0xFF;

                    tmp.r = fr;
                    tmp.g = fg;
                    tmp.b = fb;
                    //SDL_SetPaletteColors(p.p->format->palette, &tmp, 1, 1);

                    tmp.r /= 2;
                    tmp.g /= 2;
                    tmp.b /= 2;
                    //SDL_SetPaletteColors(p.p->format->palette, &tmp, 2, 1);

                    tmp.r /= 2;
                    tmp.g /= 2;
                    tmp.b /= 2;
                    //SDL_SetPaletteColors(p.p->format->palette, &tmp, 3, 1);
                }

                true_tiles = p.p;
            }

            unsigned int tx = 16;
            unsigned int ty = 5;

            if (ti == 0x3013) {
                tx = 12;
                ty = 1;
            } else if (ti == 0x3001) {
                tx = 2;
                ty = 5;
            }

            if (ty >= tiles_png_h || tx >= tiles_png_w)
                throw std::runtime_error("Invalid tile offset in tile()");

            SDL_Rect from;
            from.x = tx * tw;
            from.y = ty * th;
            from.w = tw * cwidth;
            from.h = th;

            SDL_BlitSurface(true_tiles, &from, screen, &to);
            return;
        }
#endif

        auto gi = font.glyphs.find(ti);

        if (gi == font.glyphs.end())
            return;

        const auto& glyph = gi->second;

        SDL_SetRenderDrawColor(renderer, fr, fg, fb, 0xFF);

        unsigned int xx = 0;
        unsigned int yy = 0;

        for (uint8_t v : glyph.bitmap) {

            if (v == 0) {
                xx += 8;

                if (xx >= glyph.w) {
                    xx = 0;
                    ++yy;
                }
                continue;
            }

            for (int bit = 7; bit >= 0; --bit) {
                if (v & (1 << bit)) 
                    SDL_RenderDrawPoint(renderer, to.x + xx, to.y + yy);

                ++xx;
                if (xx >= glyph.w) {
                    xx = 0;
                    ++yy;
                    break;
                }
            }
        }
    }

    template <typename FUNC_R, typename FUNC_K>
    void handle_event(const SDL_Event& e, FUNC_R resizer, FUNC_K keypress) {

        switch (e.type) {

        case SDL_QUIT:
            done = true;
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {

                sw = e.window.data1/tw;
                sh = e.window.data2/th;

                screen = SDL_GetWindowSurface(window);

                SDL_DestroyRenderer(renderer);
                renderer = SDL_CreateSoftwareRenderer(screen);

                if (renderer == NULL)
                    throw std::runtime_error("Could not create renderer");

                resizer(*this);
            }
            break;

        case SDL_KEYDOWN:
            //std::cout << "KEYPRESS: " << e.key.keysym.sym << std::endl;
            keypress(*this, e.key.keysym);
            break;

            /*
        case SDL_TEXTINPUT:
            keypress(*this, SDL_Keysym(), e.text.text);
            break;
            */

        default:
            break;
        }
    }

    template <typename FUNC, typename FUNC_R, typename FUNC_K>
    void mainloop(FUNC f, FUNC_R resizer, FUNC_K keypress) {

        SDL_Event event;

        while (!done) {

            f(*this);

            if (SDL_UpdateWindowSurface(window) < 0)
                throw std::runtime_error("Failed to update window surface");

            while (SDL_PollEvent(&event)) {
                handle_event(event, resizer, keypress);
            }
        }
    }
};

struct Socket {

    TCPsocket socket;
    SDLNet_SocketSet poller;

    Socket(const std::string& host, unsigned int port) : socket(NULL) {

        try {

            if (SDLNet_Init() < 0)
                throw std::runtime_error("Could not init network");

            IPaddress addr;

            if (SDLNet_ResolveHost(&addr, host.c_str(), port) < 0) 
                throw std::runtime_error("No such host: " + host);

            socket = SDLNet_TCP_Open(&addr);

            if (socket == NULL)
                throw std::runtime_error("Could not open connection to " + host);

            poller = SDLNet_AllocSocketSet(1);

            if (poller == NULL)
                throw std::runtime_error("Could not allocate socket set");

            SDLNet_TCP_AddSocket(poller, socket);

        } catch (...) {

            if (socket != NULL)
                SDLNet_TCP_Close(socket);

            SDLNet_Quit();
            throw;
        }
    }

    ~Socket() {

        SDLNet_TCP_Close(socket);
        SDLNet_Quit();
    }

    void recv(std::string& out) {

        int i = SDLNet_TCP_Recv(socket, (char*)out.data(), out.size());

        if (i < 0)
            throw std::runtime_error("Error receiving data");

        out.resize(i);
    }

    void send(const std::string& in) {

        int i = SDLNet_TCP_Send(socket, (char*)in.data(), in.size());

        if (i != (int)in.size())
            throw std::runtime_error("Error sending data");
    }

    void send(const char* data, size_t len) {

        int i = SDLNet_TCP_Send(socket, data, len);

        if (i != (int)len)
            throw std::runtime_error("Error sending data");
    }

    bool poll(unsigned int wait = 100) {
        return SDLNet_CheckSockets(poller, wait);
    }
};

void tsm_logger_cb(void* data, const char* file, int line, const char* func, const char* subs,
             unsigned int sev, const char* format, va_list args) {

    fprintf(stderr, "%d: %s: ", sev, subs);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}

void tsm_term_writer_cb(struct tsm_vte* vte, const char* u8, size_t len, void* data) {

    Socket* socket = (Socket*)data;

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

        draw->tile(posx+i, posy, c, cwidth, attr->inverse,
                   attr->fr, attr->fg, attr->fb,
                   attr->br, attr->bg, attr->bb);
    }

    if (len == 0) {
        for (unsigned int j = 0; j < cwidth; ++j) {
            draw->tile(posx+j, posy, (uint32_t)' ', 1, attr->inverse,
                       attr->fr, attr->fg, attr->fb,
                       attr->br, attr->bg, attr->bb);
        }
    }

    return 0;
}

struct VTE {

    tsm_screen* screen;
    tsm_vte* vte;

    Screen& draw;
    Socket& socket;
    
    VTE(Screen& d, Socket& s) : screen(NULL), vte(NULL), draw(d), socket(s)
    {

        try {

            if (tsm_screen_new(&screen, tsm_logger_cb, NULL) < 0)
                throw std::runtime_error("Could not init tsm_screen");

            if (tsm_vte_new(&vte, screen, tsm_term_writer_cb, &socket, tsm_logger_cb, NULL) < 0)
                throw std::runtime_error("Could not init tsm_vte");

            if (tsm_screen_resize(screen, draw.sw, draw.sh) < 0)
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

    void resize() {

        if (tsm_screen_resize(screen, draw.sw, draw.sh) < 0)
            throw std::runtime_error("Could not resize screen");
    }

    void feed(const std::string& data) {

        tsm_vte_input(vte, data.data(), data.size());
    }

    void redraw() {

        //tsm_age_t age =
        tsm_screen_draw(screen, tsm_drawer_cb, &draw);
    }

};


void send_resize(Screen& screen, Socket& socket) {

    std::string tmp;
    tmp.reserve(7);

    tmp += '\xFF';
    tmp += '\xFA';
    tmp += '\x1F';
    tmp += (unsigned char)(screen.sw >> 8);
    tmp += (unsigned char)(screen.sw & 0xFF);
    tmp += (unsigned char)(screen.sh >> 8);
    tmp += (unsigned char)(screen.sh & 0xFF);
    tmp += '\xFF';
    tmp += '\xF0';

    socket.send(tmp);
}

void send_terminal_type(Socket& socket, const std::string& type) {

    std::string tmp;
    tmp.reserve(3+type.size());

    tmp += '\xFF';
    tmp += '\xFA';
    tmp += '\x18';
    tmp += '\x00';
    tmp += type;
    tmp += '\xFF';
    tmp += '\xF0';

    socket.send(tmp);
}

void telnet_will(Socket& socket, char c) {
    std::string tmp;
    tmp.reserve(3);

    tmp += '\xFF';
    tmp += '\xFB';
    tmp += c;

    socket.send(tmp);
}

void telnet_wont(Socket& socket, char c) {
    std::string tmp;
    tmp.reserve(3);

    tmp += '\xFF';
    tmp += '\xFC';
    tmp += c;

    socket.send(tmp);
}

void resizer(Screen& screen, Socket& socket, VTE& vte) {

    send_resize(screen, socket);
    vte.resize();
}

unsigned char doshift(unsigned char c) {
    static bool init = false;
    static unsigned char map[128];

    if (c >= 128)
        return c;

    if (!init) {
        for (unsigned char i = 0; i < 128; ++i) {
            map[i] = i;
        }

        for (unsigned char i = 'a'; i <= 'z'; ++i) {
            map[i] = i - 'a' + 'A';
        }

        map['`'] = '~';
        map['1'] = '!';
        map['2'] = '@';
        map['3'] = '#';
        map['4'] = '$';
        map['5'] = '%';
        map['6'] = '^';
        map['7'] = '&';
        map['8'] = '*';
        map['9'] = '(';
        map['0'] = ')';
        map['-'] = '_';
        map['='] = '+';
        map['['] = '{';
        map[']'] = '}';
        map['\\'] = '|';
        map[';'] = ':';
        map['\''] = '"';
        map[','] = '<';
        map['.'] = '>';
        map['/'] = '?';

        init = true;
    }

    return map[c];
}


void keypressor(Screen& screen, const SDL_Keysym& k, VTE& vte) {

    //std::cout << "| " << (int)k.sym << " " << (int)k.mod << std::endl;
    unsigned char key = (k.sym > 127 ? '?' : k.sym);

    if (key == 0)
        return;

    unsigned int mods = 0;

    if (k.mod & KMOD_SHIFT) {
        key = doshift(key);
        mods |= TSM_SHIFT_MASK;
    }

    if (k.mod & KMOD_ALT) {
        mods |= TSM_ALT_MASK;
    }

    if (k.mod & KMOD_CTRL) {
        mods |= TSM_CONTROL_MASK;
    }

    uint32_t tsmsym = XKB_KEY_NoSymbol;

    if (k.sym & SDLK_SCANCODE_MASK) {
        
        switch (k.sym) {
        case SDLK_INSERT:
            tsmsym = XKB_KEY_Insert;
            break;
        case SDLK_HOME:
            tsmsym = XKB_KEY_Home;
            break;
        case SDLK_PAGEUP:
            tsmsym = XKB_KEY_Page_Up;
            break;
        case SDLK_END:
            tsmsym = XKB_KEY_End;
            break;
        case SDLK_PAGEDOWN:
            tsmsym = XKB_KEY_Page_Down;
            break;
        case SDLK_RIGHT:
            tsmsym = XKB_KEY_Right;
            break;
        case SDLK_LEFT:
            tsmsym = XKB_KEY_Left;
            break;
        case SDLK_DOWN:
            tsmsym = XKB_KEY_Down;
            break;
        case SDLK_UP:
            tsmsym = XKB_KEY_Up;
            break;

/*
        case XKB_KEY_KP_Enter:
        case XKB_KEY_KP_Page_Up:
        case XKB_KEY_KP_Page_Down:
        case XKB_KEY_KP_Up:
        case XKB_KEY_KP_Down:
        case XKB_KEY_KP_Right:
        case XKB_KEY_KP_Left:
        case XKB_KEY_KP_Insert:
        case XKB_KEY_KP_0:
        case XKB_KEY_KP_1:
        case XKB_KEY_KP_2:
        case XKB_KEY_KP_3:
        case XKB_KEY_KP_4:
        case XKB_KEY_KP_5:
        case XKB_KEY_KP_6:
        case XKB_KEY_KP_7:
        case XKB_KEY_KP_8:
        case XKB_KEY_KP_9:

        case XKB_KEY_KP_Subtract:
        case XKB_KEY_KP_Separator:
        case XKB_KEY_KP_Delete:
        case XKB_KEY_KP_Decimal:
        case XKB_KEY_KP_Equal:
        case XKB_KEY_KP_Divide:
        case XKB_KEY_KP_Multiply:
        case XKB_KEY_KP_Add:
        case XKB_KEY_KP_Home:
        case XKB_KEY_KP_End:
*/

        case SDLK_F1:
            tsmsym = XKB_KEY_F1;
            break;
        case SDLK_F2:
            tsmsym = XKB_KEY_F2;
            break;
        case SDLK_F3:
            tsmsym = XKB_KEY_F3;
            break;
        case SDLK_F4:
            tsmsym = XKB_KEY_F4;
            break;
        case SDLK_F5:
            tsmsym = XKB_KEY_F5;
            break;
        case SDLK_F6:
            tsmsym = XKB_KEY_F6;
            break;
        case SDLK_F7:
            tsmsym = XKB_KEY_F7;
            break;
        case SDLK_F8:
            tsmsym = XKB_KEY_F8;
            break;
        case SDLK_F9:
            tsmsym = XKB_KEY_F9;
            break;
        case SDLK_F10:
            tsmsym = XKB_KEY_F10;
            break;
        case SDLK_F11:
            tsmsym = XKB_KEY_F11;
            break;
        case SDLK_F12:
            tsmsym = XKB_KEY_F12;
            break;
        default:
            return;
        }
    }

    tsm_vte_handle_keyboard(vte.vte, tsmsym, key, mods, key);
}

void multiplexor(Screen& screen, Socket& socket, VTE& vte) {

    static std::string buff;
    static std::string rewritten;

    if (!socket.poll(50))
        return;

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
        socket.recv(buff);

        if (buff.empty()) {
            screen.done = true;
            return;
        }

        //std::cout << "> " << buff << std::endl;

        rewritten.reserve(buff.size());

        for (char c : buff) {

            switch (telnetstate) {

            case STREAM:

                if (c == '\xFF') {
                    telnetstate = IAC;

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
                    send_terminal_type(socket, "xterm");
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
                telnetstate = STREAM;
                break;

            case DO:

                if (c == '\x1F') {
                    // Window size.
                    telnet_will(socket, c);
                    send_resize(screen, socket);

                } else if (c == '\x18') {
                    // Terminal type
                    telnet_will(socket, c);

                } else {
                    telnet_wont(socket, c);
                }

                telnetstate = STREAM;
                break;
            }
        }

        if (telnetstate == STREAM)
            break;
    }

    bm _x("redraw");

    vte.feed(rewritten);
    vte.redraw();

    buff.clear();
    rewritten.clear();
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

        for (int ai = 1; ai < argc; ++ai) {
            std::string q(argv[ai]);
            
            if (q == "-c" || q == "--config") {

                if (ai+1 >= argc) {
                    usage(argv[0]);
                    return 1;
                }

                configfile = argv[ai+1];
                ++ai;

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

        if (cfg.host.empty()) {
            std::cerr << "No host to connect to." << std::endl;
            usage(argv[0]);
            return 1;
        }

        Screen screen(cfg);

        Socket sock(cfg.host, cfg.port);

        VTE vte(screen, sock);

        screen.mainloop(std::bind(multiplexor, std::placeholders::_1, std::ref(sock), std::ref(vte)),
                        std::bind(resizer, std::placeholders::_1, std::ref(sock), std::ref(vte)),
                        std::bind(keypressor, std::placeholders::_1, std::placeholders::_2, std::ref(vte))
            );

    } catch (std::exception& e) {
        std::cout << "Fatal Error: " << e.what() << std::endl;
        return 1;

    } catch (...) {
        std::cout << "Unknown fatal error." << std::endl;
        return 1;
    }

    return 0;
}
