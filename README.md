# 🌀 Qt Fractal Animation

<p align="center">
  <strong>Desktop application for generating and animating fractals built with C++ and Qt Widgets.</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-Core-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" />
  <img src="https://img.shields.io/badge/Qt%206-Widgets-41CD52?style=for-the-badge&logo=qt&logoColor=white" />
  <img src="https://img.shields.io/badge/CMake-Build-064F8C?style=for-the-badge&logo=cmake&logoColor=white" />
  <img src="https://img.shields.io/badge/Desktop-App-111827?style=for-the-badge&logo=windows&logoColor=white" />
  <img src="https://img.shields.io/badge/Fractals-Animation-7C3AED?style=for-the-badge" />
</p>

<p align="center">
  <img src="screenshots/overview.png" alt="Qt Fractal Animation overview" width="720" />
</p>

---

## 📚 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Fractals](#fractals)
- [Screenshots](#screenshots)
- [Tech Stack](#tech-stack)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Future Improvements](#future-improvements)
- [Author](#author)

---

<a id="overview"></a>
## 📌 Overview

**Qt Fractal Animation** is a desktop application built with **C++**, **Qt Widgets** and **CMake**.

The project allows the user to generate and animate selected fractals in real time.  
It includes continuous zoom animation, smooth parameter morphing, self-similarity visualization and multiple color palettes.

The main goal of the project was to create an interactive Qt application that combines mathematical visualization with a clean desktop interface.

---

<a id="features"></a>
## ✨ Features

### 🌀 Fractal Rendering
- Mandelbrot set rendering
- Julia set rendering
- Sierpiński carpet rendering
- pixel-based fractal generation
- adjustable number of iterations
- adjustable render resolution

### 🎞️ Animation
- continuous Mandelbrot zoom
- smooth Julia parameter morphing
- Sierpiński carpet zoom for self-similarity visualization
- adjustable animation speed
- real-time FPS and render time display

### 🎨 Color Palettes
- fire palette
- ocean palette
- psychedelic palette
- rainbow palette
- grayscale palette

### 🖥️ User Interface
- desktop GUI built with Qt Widgets
- custom dark graphite premium theme
- rounded UI panels
- control panel for fractal settings
- live information section with FPS, render time, zoom and position

---

<a id="fractals"></a>
## 🔬 Fractals

### Mandelbrot Set

The Mandelbrot set is animated using continuous zoom into a selected point.  
This mode is focused on showing the complexity of the fractal boundary and the effect of increasing magnification.

### Julia Set

The Julia set is animated by smoothly changing the complex parameter `c`.  
This creates a morphing effect where the shape of the fractal changes over time.

### Sierpiński Carpet

The Sierpiński carpet demonstrates recursive self-similarity.  
The animation uses zoom to show that smaller parts of the structure repeat the shape of the whole fractal.

---

<a id="screenshots"></a>
## 📸 Screenshots

| Mandelbrot | Julia |
|---|---|
| <img src="screenshots/mandelbrot.png" alt="Mandelbrot set" width="360" /> | <img src="screenshots/julia.png" alt="Julia set" width="360" /> |

| Sierpiński Carpet | Interface |
|---|---|
| <img src="screenshots/sierpinski.png" alt="Sierpiński carpet" width="360" /> | <img src="screenshots/overview.png" alt="Application interface" width="360" /> |

---

<a id="tech-stack"></a>
## 🛠️ Tech Stack

| Area | Technology |
|---|---|
| Language | C++ |
| GUI Framework | Qt 6 / Qt Widgets |
| UI Design | Qt Designer `.ui` file |
| Build System | CMake |
| Application Type | Desktop GUI application |
| Platform | Windows / Desktop |

---

<a id="requirements"></a>
## ⚙️ Requirements

To build and run the project locally, you need:

- Qt 6.5 or newer
- CMake 3.16 or newer
- C++17 compatible compiler
- Qt Creator or another C++ IDE
- Windows, Linux or macOS desktop environment

---

<a id="getting-started"></a>
## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/YOUR_USERNAME/qt-fractal-animation.git
cd qt-fractal-animation
```

### 2. Configure the project

```bash
cmake -S . -B build
```

### 3. Build the project

```bash
cmake --build build
```

### 4. Run the application

After building, run the generated executable from the build directory.

You can also open the project in **Qt Creator** and build it directly from the IDE.

---

<a id="project-structure"></a>
## 📁 Project Structure

```text
qt-fractal-animation/
├── screenshots/
│   ├── overview.png
│   ├── mandelbrot.png
│   ├── julia.png
│   └── sierpinski.png
├── .gitignore
├── CMakeLists.txt
├── README.md
├── fractalparams.h
├── fractalwidget.cpp
├── fractalwidget.h
├── fractalworker.cpp
├── fractalworker.h
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
└── mainwindow.ui
```

---

<a id="future-improvements"></a>
## 🚧 Future Improvements

- add more fractal types
- add mouse-based zoom and panning
- add screenshot export
- add animation recording
- improve responsive layout behavior
- add more color palettes
- optimize rendering performance
- add application icon and Windows installer

---

<a id="author"></a>

## 👩‍💻 Author

Created by Katarzyna Stańczyk.
