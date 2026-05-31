#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include <QElapsedTimer>
#include <QImage>
#include <QTimer>
#include <QWidget>

#include "fractalparams.h"
#include "fractalworker.h"

class FractalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FractalWidget(QWidget *parent = nullptr);
    ~FractalWidget() override;

    void startAnimation();
    void stopAnimation();
    void resetAnimation();

    void setParams(const FractalParams &params);
    FractalParams params() const;

signals:
    void frameRendered(double fps, double renderMs, double zoom,
                       double posX, double posY, int iterations);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationTick();
    void onFrameReady(QImage image, double renderMs);

private:
    void scheduleRender();
    void advanceAnimation();

    FractalParams m_params;
    FractalWorker *m_worker = nullptr;
    QTimer *m_animTimer = nullptr;

    QImage m_frame;

    bool m_rendering = false;
    bool m_pendingRender = false;

    enum class State {
        Stopped,
        Running
    };

    State m_state = State::Stopped;

    double m_currentZoom = 1.0;
    double m_animationTime = 0.0;
    double m_displayCenterX = 0.0;
    double m_displayCenterY = 0.0;

    int m_currentIterations = 450;

    QElapsedTimer m_fpsTimer;
    double m_lastRenderMs = 0.0;
    int m_frameCount = 0;
    double m_fps = 0.0;
};

#endif