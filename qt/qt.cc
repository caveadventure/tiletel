
#include <QtGui>

#include <unordered_map>
#include <vector>
#include <string>

#include <unistd.h>

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


class RasterWindow : public QWindow {

//    Q_OBJECT

public:

    bool done;

    QBackingStore backingStore;

    config::Config& cfg;

    unsigned int tw;
    unsigned int th;
    unsigned int sw;
    unsigned int sh;

    QImage screen;

    bdf::Font font;
    
    explicit RasterWindow(config::Config& _cfg, QWindow* parent = 0) : 
        QWindow(parent), 
        done(false),
        backingStore(this),
        cfg(_cfg), 
        tw(cfg.tile_width), th(cfg.tile_height), 
        sw(cfg.screen_width), sh(cfg.screen_height) {

        bdf::parse_bdf(cfg.fonts.front(), font);

        create();

        resize(tw*sw, th*sh);
        backingStore.resize(QSize(tw*sw, th*sh));
        screen = QImage(tw*sw, th*sh, QImage::Format_RGB32);
    }

    typedef uint32_t pixel_t;

    inline pixel_t map_color(uint8_t r, uint8_t g, uint8_t b) {
        uint32_t _r = 0xFF000000 | (r << 16) | (g << 8) | b;
        return (pixel_t)_r;
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


    virtual void render() {

        bm _r("render");

        int pitch = screen.bytesPerLine();
        unsigned char* pixels = screen.bits();

        pixel_t bgc = map_color(0, 0, 0);
        pixel_t fgc = map_color(0xFF, 0x0F, 0x0F);

        for (unsigned int y = 0; y < sh; ++y) {
            for (unsigned int x = 0; x < sw; ++x) {

                fill_rect(pixels, pitch, x*tw, y*th, tw, th, bgc);

                const auto& glyph = font.glyphs['&'];

                blit_bitmap(pixels, pitch, x*tw, y*th, fgc, glyph);
            }
        }
    }

public:


    void renderNow() {

        if (!isExposed())
            return;

        render();

        QRect rect(0, 0, width(), height());
        backingStore.beginPaint(rect);

        QPaintDevice* device = backingStore.paintDevice();
        QPainter painter(device);

        painter.drawImage(0, 0, screen);

        backingStore.endPaint();
        backingStore.flush(rect);
    }

protected:

    bool event(QEvent* event) {

        if (event->type() == QEvent::UpdateRequest) {
            renderNow();
            return true;

        } else if (event->type() == QEvent::KeyPress) {
            keyEvent((QKeyEvent*)event);
            return true;

        } else if (event->type() == QEvent::Close) {
            done = true;
            return true;
        }

        return QWindow::event(event);
    }

    void resizeEvent(QResizeEvent* event) {

        const auto& s = event->size();

        sw = s.width() / tw;
        sh = s.height() / th;

        backingStore.resize(s);
        screen = QImage(s, QImage::Format_RGB32);

        if (isExposed())
            renderNow();
    }

    void exposeEvent(QExposeEvent* event) {

        if (isExposed()) {
            renderNow();
        }
    }

    void keyEvent(QKeyEvent* event) {

        std::cout << "Key: " << event->key() << " [" << event->text().toUtf8().constData() << "]" << std::endl;
    }

};


int main(int argc, char** argv) {

    config::Config cfg;
    config::parse_config("default.cfg", cfg);

    QGuiApplication app(argc, argv);

    RasterWindow window(cfg);
    window.show();

    while (!window.done) {
        usleep(0.1);
        app.processEvents();
    }

    app.exit();

    return 0;
    //return app.exec();
}
