#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fractalwidget.h"

#include <QLineEdit>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Fraktale");
    setLayoutDirection(Qt::LeftToRight);
    ui->centralwidget->setLayoutDirection(Qt::LeftToRight);

    setupResponsiveLayout();

    m_fractalWidget = new FractalWidget(ui->widget);

    auto *fractalLayout = new QVBoxLayout(ui->widget);
    fractalLayout->setContentsMargins(0, 0, 0, 0);
    fractalLayout->setSpacing(0);
    fractalLayout->addWidget(m_fractalWidget);

    connect(m_fractalWidget, &FractalWidget::frameRendered,
            this, &MainWindow::onFrameRendered);

    setupCombos();
    setupSliders();

    applyCenterFromSliders();
    syncParamsToWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupResponsiveLayout()
{
    auto *mainLayout = new QHBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(18);

    auto *leftPanel = new QWidget(ui->centralwidget);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(14);

    auto *rightPanel = new QWidget(ui->centralwidget);
    rightPanel->setObjectName("rightPanel");

    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(12);

    ui->widget->setMinimumSize(520, 360);
    ui->widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->Informacje->setFixedHeight(165);
    ui->Informacje->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->Ustawienia->setFixedHeight(115);
    ui->Animacja->setFixedHeight(195);
    ui->Jakosc->setFixedHeight(135);
    ui->Widok->setFixedHeight(225);

    ui->Ustawienia->setMinimumWidth(410);
    ui->Animacja->setMinimumWidth(410);
    ui->Jakosc->setMinimumWidth(410);
    ui->Widok->setMinimumWidth(410);

    ui->Ustawienia->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->Animacja->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->Jakosc->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->Widok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    rightPanel->setMinimumWidth(430);
    rightPanel->setMaximumWidth(540);
    rightPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    leftLayout->addWidget(ui->widget, 1);
    leftLayout->addWidget(ui->Informacje, 0);

    rightLayout->addWidget(ui->Ustawienia);
    rightLayout->addWidget(ui->Animacja);
    rightLayout->addWidget(ui->Jakosc);
    rightLayout->addWidget(ui->Widok);
    rightLayout->addStretch(1);

    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 0);

    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 0);
}

void MainWindow::setupCombos()
{
    ui->TypFraktalaCombo->clear();
    ui->TypFraktalaCombo->addItems({
        tr("Mandelbrot"),
        tr("Julia"),
        tr("Dywan Sierpińskiego")
    });

    ui->PaletaKolorowCombo->clear();
    ui->PaletaKolorowCombo->addItems({
        tr("Ogień"),
        tr("Ocean"),
        tr("Psychodeliczny"),
        tr("Tęcza"),
        tr("Skala szarości")
    });
}

void MainWindow::setupSliders()
{
    ui->horizontalSliderSzybkoscAnimacji->setRange(1, 20);
    ui->horizontalSliderSzybkoscAnimacji->setValue(10);

    ui->horizontalSliderIloscIteracji->setRange(100, 2500);
    ui->horizontalSliderIloscIteracji->setValue(450);

    ui->horizontalSliderRozdzielczoscRenderowania->setRange(10, 100);
    ui->horizontalSliderRozdzielczoscRenderowania->setValue(80);

    ui->horizontalSliderZoom->setRange(0, 1000);
    ui->horizontalSliderZoom->setValue(0);

    ui->horizontalSliderPrzesyX->setRange(-500, 500);
    ui->horizontalSliderPrzesyX->setValue(0);

    ui->horizontalSliderPrzesY->setRange(-500, 500);
    ui->horizontalSliderPrzesY->setValue(0);

    ui->lineEditSzybkoscAnimacji->setReadOnly(true);
    ui->lineEditIloscIteracji->setReadOnly(true);
    ui->lineEditRozdzielczoscRenderowania->setReadOnly(true);
    ui->lineEditZoom->setReadOnly(true);
    ui->lineEditPrzesX->setReadOnly(true);
    ui->lineEditPrzesY->setReadOnly(true);

    ui->lineEditSzybkoscAnimacji->setAlignment(Qt::AlignCenter);
    ui->lineEditIloscIteracji->setAlignment(Qt::AlignCenter);
    ui->lineEditRozdzielczoscRenderowania->setAlignment(Qt::AlignCenter);
    ui->lineEditZoom->setAlignment(Qt::AlignCenter);
    ui->lineEditPrzesX->setAlignment(Qt::AlignCenter);
    ui->lineEditPrzesY->setAlignment(Qt::AlignCenter);

    ui->lineEditSzybkoscAnimacji->setText("1.0");
    ui->lineEditIloscIteracji->setText("450");
    ui->lineEditRozdzielczoscRenderowania->setText("80%");
    ui->lineEditZoom->setText("1.00e+00");
    ui->lineEditPrzesX->setText("0.000000");
    ui->lineEditPrzesY->setText("0.000000");

    ui->checkBoxPetla->hide();

    m_params.animationSpeed = 1.0;
    m_params.maxIterations = 450;
    m_params.renderResolution = 80;
    m_params.zoom = 1.0;
    m_params.sierpinskiLevels = 6;
}

void MainWindow::syncParamsToWidget()
{
    m_fractalWidget->setParams(m_params);
}

double MainWindow::baseCenterX() const
{
    if (m_params.type == FractalType::Mandelbrot) {
        return -0.743643887037151;
    }

    if (m_params.type == FractalType::SierpinskiCarpet) {
        return 0.5;
    }

    return 0.0;
}

double MainWindow::baseCenterY() const
{
    if (m_params.type == FractalType::Mandelbrot) {
        return 0.131825904205330;
    }

    if (m_params.type == FractalType::SierpinskiCarpet) {
        return 0.5;
    }

    return 0.0;
}

double MainWindow::centerOffsetScale() const
{
    double zoom = std::max(1.0, m_lastZoom);

    if (m_params.type == FractalType::Mandelbrot) {
        return 2.0 / (zoom * 500.0);
    }

    if (m_params.type == FractalType::SierpinskiCarpet) {
        return 0.8 / (zoom * 500.0);
    }

    return 3.0 / (std::max(1.0, m_params.zoom) * 500.0);
}

void MainWindow::applyCenterFromSliders()
{
    double scale = centerOffsetScale();

    m_params.centerX = baseCenterX() + ui->horizontalSliderPrzesyX->value() * scale;
    m_params.centerY = baseCenterY() + ui->horizontalSliderPrzesY->value() * scale;

    ui->lineEditPrzesX->setText(QString::number(m_params.centerX, 'f', 6));
    ui->lineEditPrzesY->setText(QString::number(m_params.centerY, 'f', 6));
}

void MainWindow::on_Start_clicked()
{
    applyCenterFromSliders();
    syncParamsToWidget();

    m_fractalWidget->startAnimation();

    ui->AnimacjaInfo->setText("Animacja");
}

void MainWindow::on_Stop_clicked()
{
    m_fractalWidget->stopAnimation();

    ui->AnimacjaInfo->setText("Stop");
}

void MainWindow::on_Reset_clicked()
{
    m_fractalWidget->stopAnimation();

    ui->horizontalSliderZoom->setValue(0);
    ui->horizontalSliderPrzesyX->setValue(0);
    ui->horizontalSliderPrzesY->setValue(0);

    m_lastZoom = 1.0;
    m_params.zoom = 1.0;

    applyCenterFromSliders();
    syncParamsToWidget();

    m_fractalWidget->resetAnimation();

    ui->AnimacjaInfo->setText("Reset");
}

void MainWindow::on_DopasujDoOkna_clicked()
{
    ui->horizontalSliderZoom->setValue(0);
    ui->horizontalSliderPrzesyX->setValue(0);
    ui->horizontalSliderPrzesY->setValue(0);

    m_lastZoom = 1.0;
    m_params.zoom = 1.0;

    applyCenterFromSliders();
    syncParamsToWidget();
}

void MainWindow::on_horizontalSliderSzybkoscAnimacji_valueChanged(int value)
{
    double speed = value / 10.0;

    m_params.animationSpeed = speed;
    ui->lineEditSzybkoscAnimacji->setText(QString::number(speed, 'f', 1));

    syncParamsToWidget();
}

void MainWindow::on_horizontalSliderIloscIteracji_valueChanged(int value)
{
    m_params.maxIterations = value;
    m_params.sierpinskiLevels = qBound(1, 2 + value / 120, 12);

    ui->lineEditIloscIteracji->setText(QString::number(value));

    syncParamsToWidget();
}

void MainWindow::on_horizontalSliderRozdzielczoscRenderowania_valueChanged(int value)
{
    m_params.renderResolution = value;
    ui->lineEditRozdzielczoscRenderowania->setText(QString::number(value) + "%");

    syncParamsToWidget();
}

void MainWindow::on_horizontalSliderZoom_valueChanged(int value)
{
    double zoom = std::pow(10.0, value / 200.0);

    m_params.zoom = zoom;
    m_lastZoom = zoom;

    ui->lineEditZoom->setText(QString::number(zoom, 'e', 2));

    applyCenterFromSliders();
    syncParamsToWidget();
}

void MainWindow::on_horizontalSliderPrzesyX_valueChanged(int)
{
    applyCenterFromSliders();
    syncParamsToWidget();
}

void MainWindow::on_horizontalSliderPrzesY_valueChanged(int)
{
    applyCenterFromSliders();
    syncParamsToWidget();
}

void MainWindow::on_checkBoxPetla_toggled(bool checked)
{
    m_params.loop = checked;
    syncParamsToWidget();
}

void MainWindow::on_TypFraktalaCombo_currentIndexChanged(int index)
{
    m_fractalWidget->stopAnimation();

    m_params.type = static_cast<FractalType>(index);

    ui->horizontalSliderZoom->setValue(0);
    ui->horizontalSliderPrzesyX->setValue(0);
    ui->horizontalSliderPrzesY->setValue(0);

    m_lastZoom = 1.0;
    m_params.zoom = 1.0;

    applyCenterFromSliders();
    syncParamsToWidget();

    m_fractalWidget->resetAnimation();

    ui->AnimacjaInfo->setText("Stop");
}

void MainWindow::on_PaletaKolorowCombo_currentIndexChanged(int index)
{
    m_params.palette = static_cast<ColorPalette>(index);
    syncParamsToWidget();
}

void MainWindow::onFrameRendered(double fps, double renderMs, double zoom,
                                 double posX, double posY, int iterations)
{
    m_lastZoom = std::max(1.0, zoom);

    ui->KlatkaInfo->setText(QString::number(fps, 'f', 1));
    ui->CzasRenderowaniaInfo->setText(QString::number(renderMs, 'f', 1) + "ms");
    ui->LiczbaIteracjiInfo->setText(QString::number(iterations));
    ui->ZoomInfo->setText(QString::number(zoom, 'e', 2));

    ui->PozycjaInfo->setText(
        QString("( %1 , %2 )")
            .arg(posX, 0, 'f', 6)
            .arg(posY, 0, 'f', 6)
        );
}