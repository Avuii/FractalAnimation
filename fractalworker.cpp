#include "fractalworker.h"

#include <QElapsedTimer>
#include <QMutexLocker>

#include <cmath>

FractalWorker::FractalWorker(QObject *parent)
    : QThread(parent)
{
}

FractalWorker::~FractalWorker()
{
    stop();
}

void FractalWorker::requestRender(const FractalParams &params, int width, int height)
{
    QMutexLocker locker(&m_mutex);

    m_params = params;
    m_width = width;
    m_height = height;
    m_dirty = true;

    m_cond.wakeOne();
}

void FractalWorker::stop()
{
    {
        QMutexLocker locker(&m_mutex);
        m_quit = true;
        m_cond.wakeOne();
    }

    wait();
}

void FractalWorker::run()
{
    while (true) {
        FractalParams params;
        int w = 0;
        int h = 0;

        {
            QMutexLocker locker(&m_mutex);

            while (!m_dirty && !m_quit) {
                m_cond.wait(&m_mutex);
            }

            if (m_quit) {
                break;
            }

            params = m_params;
            w = m_width;
            h = m_height;
            m_dirty = false;
        }

        if (w <= 0 || h <= 0) {
            continue;
        }

        QElapsedTimer timer;
        timer.start();

        QImage image = renderFrame(params, w, h);
        double renderMs = static_cast<double>(timer.elapsed());

        emit frameReady(image, renderMs);
    }
}

QImage FractalWorker::renderFrame(const FractalParams &params, int w, int h)
{
    if (params.type == FractalType::SierpinskiCarpet) {
        return renderSierpinski(params, w, h);
    }

    return renderEscapeFractal(params, w, h);
}

QImage FractalWorker::renderEscapeFractal(const FractalParams &params, int w, int h)
{
    double scale = qBound(0.1, params.renderResolution / 100.0, 1.0);

    int rw = qMax(1, static_cast<int>(w * scale));
    int rh = qMax(1, static_cast<int>(h * scale));

    QImage image(rw, rh, QImage::Format_RGB32);

    double aspect = static_cast<double>(rw) / static_cast<double>(rh);
    double range = (params.type == FractalType::Mandelbrot ? 3.5 : 3.0) / params.zoom;
    double rangeX = range * aspect;

    for (int py = 0; py < rh; ++py) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(py));
        double cy = params.centerY + range * (0.5 - static_cast<double>(py) / rh);

        for (int px = 0; px < rw; ++px) {
            double cx = params.centerX + rangeX * (static_cast<double>(px) / rw - 0.5);

            int iter = 0;

            if (params.type == FractalType::Mandelbrot) {
                iter = iterateMandelbrot(cx, cy, params.maxIterations);
                line[px] = colorize(iter, params.maxIterations, params.palette).rgb();
            } else {
                iter = iterateJulia(cx, cy, params.juliaCx, params.juliaCy, params.maxIterations);
                line[px] = colorizeJulia(iter, params.maxIterations, params.palette).rgb();
            }
        }
    }

    if (rw != w || rh != h) {
        return image.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    return image;
}

QImage FractalWorker::renderSierpinski(const FractalParams &params, int w, int h)
{
    double scale = qBound(0.1, params.renderResolution / 100.0, 1.0);

    int rw = qMax(1, static_cast<int>(w * scale));
    int rh = qMax(1, static_cast<int>(h * scale));

    QImage image(rw, rh, QImage::Format_RGB32);

    double range = 1.0 / params.zoom;
    int levels = qBound(1, params.sierpinskiLevels, 28);

    for (int py = 0; py < rh; ++py) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(py));
        double y = params.centerY + range * (0.5 - static_cast<double>(py) / rh);

        for (int px = 0; px < rw; ++px) {
            double x = params.centerX + range * (static_cast<double>(px) / rw - 0.5);

            if (isSierpinskiFilled(x, y, levels)) {
                line[px] = colorizeCarpet(x, y, params.palette).rgb();
            } else {
                line[px] = qRgb(3, 7, 18);
            }
        }
    }

    if (rw != w || rh != h) {
        return image.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    return image;
}

int FractalWorker::iterateMandelbrot(double cx, double cy, int maxIter) const
{
    double zx = 0.0;
    double zy = 0.0;

    for (int i = 0; i < maxIter; ++i) {
        double zx2 = zx * zx;
        double zy2 = zy * zy;

        if (zx2 + zy2 > 4.0) {
            return i;
        }

        zy = 2.0 * zx * zy + cy;
        zx = zx2 - zy2 + cx;
    }

    return maxIter;
}

int FractalWorker::iterateJulia(double zx, double zy, double cx, double cy, int maxIter) const
{
    for (int i = 0; i < maxIter; ++i) {
        double zx2 = zx * zx;
        double zy2 = zy * zy;

        if (zx2 + zy2 > 4.0) {
            return i;
        }

        double nextX = zx2 - zy2 + cx;
        double nextY = 2.0 * zx * zy + cy;

        zx = nextX;
        zy = nextY;
    }

    return maxIter;
}

bool FractalWorker::isSierpinskiFilled(double x, double y, int levels) const
{
    if (x < 0.0 || x > 1.0 || y < 0.0 || y > 1.0) {
        return false;
    }

    for (int i = 0; i < levels; ++i) {
        int xi = static_cast<int>(std::floor(x * 3.0));
        int yi = static_cast<int>(std::floor(y * 3.0));

        if (xi > 2) {
            xi = 2;
        }

        if (yi > 2) {
            yi = 2;
        }

        if (xi == 1 && yi == 1) {
            return false;
        }

        x = x * 3.0 - xi;
        y = y * 3.0 - yi;
    }

    return true;
}

QColor FractalWorker::colorize(int iter, int maxIter, ColorPalette palette) const
{
    if (iter >= maxIter) {
        return Qt::black;
    }

    double t = static_cast<double>(iter) / static_cast<double>(maxIter);

    if (palette == ColorPalette::Fire) {
        int r = qBound(0, static_cast<int>(t * 3.0 * 255.0), 255);
        int g = qBound(0, static_cast<int>((t * 3.0 - 1.0) * 255.0), 255);
        int b = qBound(0, static_cast<int>((t * 3.0 - 2.0) * 255.0), 255);
        return QColor(r, g, b);
    }

    if (palette == ColorPalette::Ocean) {
        int r = qBound(0, static_cast<int>(t * 50.0), 255);
        int g = qBound(0, static_cast<int>(t * 150.0), 255);
        int b = qBound(0, static_cast<int>(t * 255.0), 255);
        return QColor(r, g, b);
    }

    if (palette == ColorPalette::Psychedelic) {
        double angle = t * 6.283185307179586;
        int r = qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle)), 255);
        int g = qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle + 2.094395102393195)), 255);
        int b = qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle + 4.188790204786390)), 255);
        return QColor(r, g, b);
    }

    if (palette == ColorPalette::Rainbow) {
        int hue = static_cast<int>(360.0 * t) % 360;
        return QColor::fromHsv(hue, 255, 255);
    }

    int v = qBound(0, static_cast<int>(t * 255.0), 255);
    return QColor(v, v, v);
}

QColor FractalWorker::colorizeJulia(int iter, int maxIter, ColorPalette palette) const
{
    if (iter >= maxIter) {
        if (palette == ColorPalette::Fire) {
            return QColor(20, 3, 0);
        }

        if (palette == ColorPalette::Ocean) {
            return QColor(0, 10, 28);
        }

        if (palette == ColorPalette::Psychedelic) {
            return QColor(18, 0, 28);
        }

        if (palette == ColorPalette::Rainbow) {
            return QColor(4, 6, 18);
        }

        return Qt::black;
    }

    double t = static_cast<double>(iter) / static_cast<double>(maxIter);
    double softened = std::sqrt(t);
    double band = std::fmod(iter * 0.035, 1.0);

    if (palette == ColorPalette::Fire) {
        int r = qBound(0, static_cast<int>(90.0 + 165.0 * softened), 255);
        int g = qBound(0, static_cast<int>(30.0 + 170.0 * band), 255);
        int b = qBound(0, static_cast<int>(10.0 + 70.0 * t), 255);
        return QColor(r, g, b);
    }

    if (palette == ColorPalette::Ocean) {
        int r = qBound(0, static_cast<int>(15.0 + 60.0 * t), 255);
        int g = qBound(0, static_cast<int>(80.0 + 150.0 * softened), 255);
        int b = qBound(0, static_cast<int>(150.0 + 105.0 * band), 255);
        return QColor(r, g, b);
    }

    if (palette == ColorPalette::Psychedelic) {
        double angle = band * 6.283185307179586;
        int r = qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle)), 255);
        int g = qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle + 2.094395102393195)), 255);
        int b = qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle + 4.188790204786390)), 255);
        return QColor(r, g, b);
    }

    if (palette == ColorPalette::Rainbow) {
        int hue = static_cast<int>(std::fmod(iter * 9.0, 360.0));
        return QColor::fromHsv(hue, 235, 255);
    }

    int v = qBound(0, static_cast<int>(60.0 + 195.0 * softened), 255);
    return QColor(v, v, v);
}

QColor FractalWorker::colorizeCarpet(double x, double y, ColorPalette palette) const
{
    double tx = x - std::floor(x);
    double ty = y - std::floor(y);
    double t = qBound(0.0, 0.55 * tx + 0.45 * ty, 1.0);

    if (palette == ColorPalette::Fire) {
        return QColor(
            qBound(0, static_cast<int>(180.0 + 75.0 * t), 255),
            qBound(0, static_cast<int>(70.0 + 140.0 * t), 255),
            qBound(0, static_cast<int>(20.0 + 60.0 * t), 255)
            );
    }

    if (palette == ColorPalette::Ocean) {
        return QColor(
            qBound(0, static_cast<int>(30.0 + 40.0 * t), 255),
            qBound(0, static_cast<int>(100.0 + 120.0 * t), 255),
            qBound(0, static_cast<int>(170.0 + 80.0 * t), 255)
            );
    }

    if (palette == ColorPalette::Psychedelic) {
        double angle = t * 6.283185307179586;
        return QColor(
            qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle)), 255),
            qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle + 2.094395102393195)), 255),
            qBound(0, static_cast<int>(128.0 + 127.0 * std::sin(angle + 4.188790204786390)), 255)
            );
    }

    if (palette == ColorPalette::Rainbow) {
        int hue = static_cast<int>(360.0 * t) % 360;
        return QColor::fromHsv(hue, 230, 255);
    }

    int v = qBound(0, static_cast<int>(80.0 + 175.0 * t), 255);
    return QColor(v, v, v);
}