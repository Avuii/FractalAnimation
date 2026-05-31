#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "fractalparams.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class FractalWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Start_clicked();
    void on_Stop_clicked();
    void on_Reset_clicked();
    void on_DopasujDoOkna_clicked();

    void on_horizontalSliderSzybkoscAnimacji_valueChanged(int value);
    void on_horizontalSliderIloscIteracji_valueChanged(int value);
    void on_horizontalSliderRozdzielczoscRenderowania_valueChanged(int value);
    void on_horizontalSliderZoom_valueChanged(int value);
    void on_horizontalSliderPrzesyX_valueChanged(int value);
    void on_horizontalSliderPrzesY_valueChanged(int value);
    void on_checkBoxPetla_toggled(bool checked);
    void on_TypFraktalaCombo_currentIndexChanged(int index);
    void on_PaletaKolorowCombo_currentIndexChanged(int index);


    void onFrameRendered(double fps, double renderMs, double zoom,
                         double posX, double posY, int iterations);

private:
    void setupCombos();
    void setupSliders();
    void syncParamsToWidget();
    void applyCenterFromSliders();

    double baseCenterX() const;
    double baseCenterY() const;
    double centerOffsetScale() const;
    void setupResponsiveLayout();

    Ui::MainWindow *ui;
    FractalWidget *m_fractalWidget = nullptr;
    FractalParams m_params;

    double m_lastZoom = 1.0;
};

#endif