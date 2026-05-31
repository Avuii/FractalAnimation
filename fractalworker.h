#ifndef FRACTALWORKER_H
#define FRACTALWORKER_H

#include <QColor>
#include <QImage>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "fractalparams.h"

class FractalWorker : public QThread
{
    Q_OBJECT

public:
    explicit FractalWorker(QObject *parent = nullptr);
    ~FractalWorker() override;

    void requestRender(const FractalParams &params, int width, int height);
    void stop();

signals:
    void frameReady(QImage image, double renderMs);

protected:
    void run() override;

private:
    QImage renderFrame(const FractalParams &params, int w, int h);
    QImage renderEscapeFractal(const FractalParams &params, int w, int h);
    QImage renderSierpinski(const FractalParams &params, int w, int h);

    int iterateMandelbrot(double cx, double cy, int maxIter) const;
    int iterateJulia(double zx, double zy, double cx, double cy, int maxIter) const;

    bool isSierpinskiFilled(double x, double y, int levels) const;

    QColor colorize(int iter, int maxIter, ColorPalette palette) const;
    QColor colorizeJulia(int iter, int maxIter, ColorPalette palette) const;
    QColor colorizeCarpet(double x, double y, ColorPalette palette) const;

    QMutex m_mutex;
    QWaitCondition m_cond;

    FractalParams m_params;
    int m_width = 0;
    int m_height = 0;
    bool m_dirty = false;
    bool m_quit = false;
};

#endif