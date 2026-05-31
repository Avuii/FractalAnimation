#include "fractalwidget.h"

#include <QPainter>
#include <QResizeEvent>

#include <algorithm>
#include <cmath>

FractalWidget::FractalWidget(QWidget *parent)
    : QWidget(parent)
{
    m_worker = new FractalWorker(this);

    connect(m_worker, &FractalWorker::frameReady,
            this, &FractalWidget::onFrameReady,
            Qt::QueuedConnection);

    m_worker->start();

    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(16);

    connect(m_animTimer, &QTimer::timeout,
            this, &FractalWidget::onAnimationTick);

    m_fpsTimer.start();

    setMinimumSize(100, 100);
}

FractalWidget::~FractalWidget()
{
    m_animTimer->stop();
    m_worker->stop();
}

FractalParams FractalWidget::params() const
{
    return m_params;
}

void FractalWidget::setParams(const FractalParams &params)
{
    FractalType oldType = m_params.type;

    m_params = params;

    if (m_state == State::Stopped || oldType != params.type) {
        m_currentZoom = params.zoom;
        m_animationTime = 0.0;
    }

    scheduleRender();
}

void FractalWidget::startAnimation()
{
    m_state = State::Running;
    m_animTimer->start();
}

void FractalWidget::stopAnimation()
{
    m_state = State::Stopped;
    m_animTimer->stop();
    scheduleRender();
}

void FractalWidget::resetAnimation()
{
    m_state = State::Stopped;
    m_animTimer->stop();

    m_currentZoom = m_params.zoomStart;
    m_animationTime = 0.0;

    scheduleRender();
}

void FractalWidget::onAnimationTick()
{
    advanceAnimation();
    scheduleRender();
}

void FractalWidget::advanceAnimation()
{
    double speed = qBound(0.1, m_params.animationSpeed, 3.0);
    m_animationTime += 0.016 * speed;

    if (m_params.type == FractalType::Mandelbrot) {
        double factor = std::pow(1.006, speed);
        m_currentZoom *= factor;

        if (!std::isfinite(m_currentZoom)) {
            m_currentZoom = 1.0e12;
        }
    } else if (m_params.type == FractalType::Julia) {
        m_currentZoom = m_params.zoom;
    } else {
        double factor = std::pow(1.006, speed);
        m_currentZoom *= factor;

        if (!std::isfinite(m_currentZoom)) {
            m_currentZoom = 1.0e12;
        }
    }
}

void FractalWidget::onFrameReady(QImage image, double renderMs)
{
    m_frame = image;
    m_lastRenderMs = renderMs;
    m_rendering = false;

    ++m_frameCount;

    qint64 elapsed = m_fpsTimer.elapsed();

    if (elapsed >= 500) {
        m_fps = m_frameCount * 1000.0 / static_cast<double>(elapsed);
        m_frameCount = 0;
        m_fpsTimer.restart();
    }

    update();

    emit frameRendered(m_fps, m_lastRenderMs, m_currentZoom,
                       m_displayCenterX, m_displayCenterY,
                       m_currentIterations);

    if (m_pendingRender) {
        m_pendingRender = false;
        scheduleRender();
    }
}

void FractalWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (m_frame.isNull()) {
        painter.fillRect(rect(), Qt::black);
    } else {
        painter.drawImage(rect(), m_frame);
    }
}

void FractalWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    scheduleRender();
}

void FractalWidget::scheduleRender()
{
    if (width() <= 0 || height() <= 0) {
        return;
    }

    if (m_rendering) {
        m_pendingRender = true;
        return;
    }

    FractalParams params = m_params;

    if (params.type == FractalType::Mandelbrot) {
        params.zoom = m_currentZoom;

        double logZoom = std::log10(std::max(1.0, m_currentZoom));
        int autoIterations = m_params.maxIterations + static_cast<int>(logZoom * 350.0);

        params.maxIterations = qBound(m_params.maxIterations, autoIterations, 8000);
        m_currentIterations = params.maxIterations;
    } else if (params.type == FractalType::Julia) {
        params.zoom = m_params.zoom;

        double t = m_animationTime * 0.85;
        params.juliaCx = -0.45 + 0.22 * std::cos(t);
        params.juliaCy = 0.12 + 0.22 * std::sin(t);

        m_currentZoom = params.zoom;
        m_currentIterations = params.maxIterations;
    } else {
        params.zoom = m_currentZoom;

        double fixedX = 0.0;
        double fixedY = 1.0;

        params.centerX = fixedX + (m_params.centerX - fixedX) / m_currentZoom;
        params.centerY = fixedY + (m_params.centerY - fixedY) / m_currentZoom;

        double logZoom = std::log(std::max(1.0, m_currentZoom)) / std::log(3.0);
        int autoLevels = m_params.sierpinskiLevels + 8 + static_cast<int>(logZoom);

        params.sierpinskiLevels = qBound(1, autoLevels, 28);

        m_currentIterations = params.sierpinskiLevels;
    }

    m_displayCenterX = params.centerX;
    m_displayCenterY = params.centerY;

    m_rendering = true;
    m_worker->requestRender(params, width(), height());
}