
#include <QtGui>

#include <unordered_map>
#include <vector>
#include <string>

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

    config::Config& cfg;

    unsigned int tw;
    unsigned int th;
    unsigned int sw;
    unsigned int sh;

    bdf::Font font;
    
    explicit RasterWindow(config::Config& _cfg, QWindow* parent = 0) : 
        QWindow(parent), 
        cfg(_cfg), 
        tw(cfg.tile_width), th(cfg.tile_height), 
        sw(cfg.screen_width), sh(cfg.screen_height), 
        m_update_pending(false) {

        bdf::parse_bdf(cfg.fonts.front(), font);

        m_backingStore = new QBackingStore(this);
        create();

        resize(tw*sw, th*sh);
        m_backingStore->resize(QSize(tw*sw, th*sh));
    }

    virtual void render(QPainter* painter) {

        bm _r("render");

        const auto& glyph = font.glyphs['&'];

        QBitmap bm = QBitmap::fromData(QSize(glyph.w, font.h), &(glyph.bm[0]), QImage::Format_Mono);

        QColor bgc(0, 0, 0);
        QColor fgc(0xFF, 0x0F, 0x0F);

        for (unsigned int y = 0; y < sh; ++y) {
            for (unsigned int x = 0; x < sw; ++x) {

                painter->fillRect(x*tw, y*th, tw, th, bgc);

                painter->setPen(fgc);

                painter->drawPixmap(x*tw, y*th, bm);
            }
        }
    }

public slots:

    void renderLater() {

        if (!m_update_pending) {
            m_update_pending = true;
            QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
        }
    }

    void renderNow() {

        if (!isExposed())
            return;

        QRect rect(0, 0, width(), height());
        m_backingStore->beginPaint(rect);

        QPaintDevice *device = m_backingStore->paintDevice();
        QPainter painter(device);

        painter.fillRect(0, 0, width(), height(), Qt::white);
        render(&painter);

        m_backingStore->endPaint();
        m_backingStore->flush(rect);
    }

protected:

    bool event(QEvent* event) {

        if (event->type() == QEvent::UpdateRequest) {
            m_update_pending = false;
            renderNow();
            return true;
        }
        return QWindow::event(event);
    }

    void resizeEvent(QResizeEvent* event) {

        const auto& s = event->size();

        sw = s.width() / tw;
        sh = s.height() / th;

        m_backingStore->resize(s);

        if (isExposed())
            renderNow();
    }

    void exposeEvent(QExposeEvent* event) {

        if (isExposed()) {
            renderNow();
        }
    }

private:

    QBackingStore* m_backingStore;
    bool m_update_pending;
};


int main(int argc, char** argv) {

    config::Config cfg;
    config::parse_config("default.cfg", cfg);

    QGuiApplication app(argc, argv);

    RasterWindow window(cfg);
    window.show();

    return app.exec();
}
