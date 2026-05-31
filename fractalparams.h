#ifndef FRACTALPARAMS_H
#define FRACTALPARAMS_H

enum class FractalType {
    Mandelbrot,
    Julia,
    SierpinskiCarpet
};

enum class ColorPalette {
    Fire,
    Ocean,
    Psychedelic,
    Rainbow,
    Grayscale
};

struct FractalParams {
    FractalType type = FractalType::Mandelbrot;
    ColorPalette palette = ColorPalette::Fire;

    double centerX = -0.743643887037151;
    double centerY = 0.131825904205330;
    double zoom = 1.0;

    double juliaCx = -0.7;
    double juliaCy = 0.27015;

    int maxIterations = 450;
    int renderResolution = 80;
    int sierpinskiLevels = 6;

    double animationSpeed = 1.0;
    bool loop = false;

    double zoomStart = 1.0;
};

#endif