
#include <QtGui>

#include <unordered_map>
#include <vector>
#include <string>

#include "bdf.h"
#include "config.h"

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
        m_backingStore->resize(tw*sw, th*sh);
    }

    virtual void render(QPainter* painter) {
        //painter->drawText(QRectF(0, 0, width(), height()), Qt::AlignCenter, QStringLiteral("QWindow"));

        QColor bgc(0, 0, 0);
        QColor fgc(0, 0xFF, 0xFF);

        painter->fillRect(50, 50, 8, 16, bgc);

        const auto& glyph = font.glyphs['&'];

        QBitmap bm = QBitmap::fromData(QSize(glyph.w, font.h), &(glyph.bm[0]), QImage::Format_Mono);

        painter->drawPixmap(58, 50, bm);
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

        m_backingStore->resize(event->size());
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
