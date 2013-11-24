
#include <string>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <unordered_map>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_net.h"
#include "SDL2/SDL_ttf.h"

#include "libtsm/src/libtsm.h"

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

    SDL_Surface* tiles;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* screen;
    TTF_Font* font;

    unsigned int tw;
    unsigned int th;

    unsigned int sw;
    unsigned int sh;

    unsigned int tiles_png_w;
    unsigned int tiles_png_h;

    bool done;

    std::unordered_map<uint32_t, SDL_Surface*> glyphs;


    Screen(unsigned int tilew, unsigned int tileh, 
           const std::string& fontfile, 
           unsigned int screenw, unsigned int screenh) : 
        tiles(NULL), window(NULL), renderer(NULL), screen(NULL), font(NULL),
        tw(tilew), th(tileh), 
        sw(screenw), sh(screenh), 
        done(false)
    {

        try {

            if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
                throw std::runtime_error("Could not init SDL");

            window = SDL_CreateWindow("tiletel",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      tw*sw, th*sh,
                                      SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN);

            if (window == NULL)
                throw std::runtime_error("Could not create window.");

            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

            if (renderer == NULL)
                throw std::runtime_error("Could not create renderer");

            screen = SDL_GetWindowSurface(window);

            if (screen == NULL)
                throw std::runtime_error("Could not create window surface");

            if (IMG_Init(IMG_INIT_PNG) == 0)
                throw std::runtime_error("Could not init SDL_Image");

            tiles = IMG_Load("tiles.png");

            if (tiles == NULL)
                throw std::runtime_error("Failed to load tiles.png");

            if ((tiles->w % tw) != 0 || 
                (tiles->h % th) != 0) {

                //throw std::runtime_error("Size of tiles.png does not match tile size");
            }

            tiles_png_w = tiles->w / tw;
            tiles_png_h = tiles->h / th;

            if (TTF_Init() < 0)
                throw std::runtime_error("Could not init TTF");

            font = TTF_OpenFont(fontfile.c_str(), th);

            if (font == NULL)
                throw std::runtime_error("Could not open font: " + fontfile);

        } catch(...) {

            if (tiles != NULL)
                SDL_FreeSurface(tiles);

            if (font != NULL)
                TTF_CloseFont(font);

            if (renderer != NULL)
                SDL_DestroyRenderer(renderer);

            if (window != NULL)
                SDL_DestroyWindow(window);

            TTF_Quit();
            SDL_Quit();
            throw;
        }
    }

    ~Screen() {

        for (auto& g : glyphs) {
            SDL_FreeSurface(g.second);
        }

        TTF_CloseFont(font);
        SDL_FreeSurface(tiles);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

    void tile(unsigned int x, unsigned int y, uint32_t ti,
              uint8_t fr, uint8_t fg, uint8_t fb,
              uint8_t br, uint8_t bg, uint8_t bb) {

#if 0
        if (x >= sw || y >= sh)
            throw std::runtime_error("Invalid screen offset in tile()");

        SDL_Rect from;
        from.x = ti % tiles_png_w;
        from.y = ti / tiles_png_w;
        from.w = tw;
        from.h = th;

        if (from.y >= (int)tiles_png_h)
            throw std::runtime_error("Invalid tile offset in tile()");

        from.x *= tw;
        from.y *= th;
#endif

        SDL_Surface*& g = glyphs[ti];

        static unsigned int font_w = 0;
        static unsigned int font_h = 0;

        if (g == NULL) {
            std::cout << "!!!" << std::endl;
            SDL_Color fgc = { 0xFF, 0xFF, 0xFF };
            g = TTF_RenderGlyph_Solid(font, ti, fgc);

            if (g == NULL)
                throw std::runtime_error("Could not render glyph");

            if (font_w == 0 || font_h == 0) {
                font_w = g->w;
                font_h = g->h;
            }
        }

        SDL_Rect to;
        to.x = x * tw;
        to.y = y * th;
        to.w = tw; //tw;
        to.h = font_h; //th;

        SDL_Color bgfg[2] = { { br, bg, bb, 0xFF }, { fr, fg, fb, 0xFF } };
        SDL_SetPaletteColors(g->format->palette, &bgfg[0], 0, 2);

        SDL_Surface* conv = SDL_ConvertSurface(g, screen->format, 0);

        if (conv == NULL)
            throw std::runtime_error("Failed to convert surface");

        if (SDL_BlitScaled(conv, NULL, screen, &to) < 0)
            throw std::runtime_error("Failed to blit");

        SDL_FreeSurface(conv);
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

                resizer(*this);
            }
            break;

        case SDL_KEYDOWN:
            keypress(*this, e.key.keysym);
            break;

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

        std::cout << "Recv." << std::endl;
        
        int i = SDLNet_TCP_Recv(socket, (char*)out.data(), out.size());

        std::cout << "Recv ok." << std::endl;

        if (i < 0)
            throw std::runtime_error("Error receiving data");

        out.resize(i);
    }

    void send(const std::string& in) {

        std::cout << "Send." << std::endl;

        int i = SDLNet_TCP_Send(socket, (char*)in.data(), in.size());

        std::cout << "Send ok." << std::endl;

        if (i != (int)in.size())
            throw std::runtime_error("Error sending data");
    }

    void send(const char* data, size_t len) {

        std::cout << "Send2." << std::endl;

        int i = SDLNet_TCP_Send(socket, data, len);

        std::cout << "Send2 ok." << std::endl;

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

    std::cout << "Socket sender!" << std::endl;

    socket->send(u8, len);
}

int tsm_drawer_cb(struct tsm_screen* screen, uint32_t id, const uint32_t* ch, size_t len, unsigned int cwidth, 
                  unsigned int posx, unsigned int posy, const struct tsm_screen_attr* attr, tsm_age_t age, 
                  void* data) {

    Screen* draw = (Screen*)data;

    for (unsigned int i = 0; i < len; ++i) {
        uint32_t c = ch[i];

        for (unsigned int j = 0; j < cwidth; ++j) {
            draw->tile(posx+j, posy, c, 
                       attr->fr, attr->fg, attr->fb,
                       attr->br, attr->bg, attr->bb);
        }
    }

    if (len == 0) {
        for (unsigned int j = 0; j < cwidth; ++j) {
            draw->tile(posx+j, posy, (uint32_t)' ', 
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

        bm _r("redraw");
        std::cout << "screen " << draw.sw << " " << draw.sw << std::endl;

        tsm_age_t age = tsm_screen_draw(screen, tsm_drawer_cb, &draw);
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

void keypressor(Screen& screen, const SDL_Keysym& k, VTE& vte) {

    std::cout << "| " << (int)k.sym << " " << (int)k.mod << std::endl;

    std::cout << "Handling keyboard." << std::endl;

    tsm_vte_handle_keyboard(vte.vte, k.sym, k.sym, 0, k.sym);

    std::cout << "Handling keyboard ok." << std::endl;
}

void multiplexor(Screen& screen, Socket& socket, VTE& vte) {

    static std::string buff;
    static std::string rewritten;

    if (!socket.poll(50))
        return;

    std::cout << "MUX" << std::endl;

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

                std::cout << "SB : " << (int)c << std::endl;
                if (c == '\xFF') {
                    telnetstate = SB_IAC;

                } else if (c == '\x18') {
                    std::cout << "Sending terminal type." << std::endl;
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
                std::cout << "DONT : " << (int)c << std::endl;
                telnetstate = STREAM;
                break;

            case WONT:
                std::cout << "WONT : " << (int)c << std::endl;
                telnetstate = STREAM;
                break;

            case WILL:
                std::cout << "WILL : " << (int)c << std::endl;
                telnetstate = STREAM;
                break;

            case DO:

                std::cout << "DO : " << (int)c << std::endl;
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

    vte.feed(rewritten);
    vte.redraw();

    buff.clear();
    rewritten.clear();
}



int main(int argc, char** argv) {

    try {

        Screen screen(12, 12, 
                      "terminus.ttf",
                      70, 50);

        Socket sock(argv[1], ::atoi(argv[2]));

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
