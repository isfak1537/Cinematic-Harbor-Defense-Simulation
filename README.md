# Cinematic Harbor Defense Simulation

An interactive 2D graphics simulation built with **C++** and **OpenGL/GLUT**. This project demonstrates core computer graphics algorithms, real-time animation, and collision physics in a harbor defense scenario.

---

## 🎮 Features

* **Algorithmic Primitives:** Manual implementation of **DDA Line Drawing** and **Midpoint Circle** algorithms.
* **Real-time Animation:** Driven by a 16ms GLUT timer loop achieving a smooth **60 FPS**.
* **Interactive Controls:** Player-controlled cannon rotation and projectile firing.
* **Particle System:** Dynamic radial explosion effects upon ship destruction.
* **Physics & Logic:** AABB collision detection, sinusoidal wave oscillation, and projectile trajectories.
* **Visual HUD:** Live score tracking and a rotating radar sweep system.

---

## 🛠️ Technical Stack

* **Language:** C++
* **Graphics Library:** OpenGL (Legacy Fixed-Function Pipeline)
* **Windowing API:** GLUT / FreeGLUT
* **Data Structures:** STL Vectors for dynamic object management (Bullets, Ships, Particles).

---

## 🚀 How to Run

Follow these steps to compile and run the project on your local machine.

### Prerequisites

* **C++ Compiler:** MinGW (Windows), GCC (Linux), or Clang (macOS).
* **OpenGL & GLUT:** Ensure you have the FreeGLUT development libraries installed.

### Setup & Execution

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/NabilRahber/Cinematic-Harbor-Defense-Simulation.git
    cd Cinematic-Harbor-Defense-Simulation
    ```

2.  **Compilation (using g++):**
    * **Windows:**
        ```bash
        g++ main.cpp -o defense.exe -lfreeglut -lopengl32 -lglu32
        ```
    * **Linux:**
        ```bash
        g++ main.cpp -o defense -lglut -lGL -lGLU
        ```

3.  **Run the Simulation:**
    * **Windows:** `.\defense.exe`
    * **Linux:** `./defense`

---

## 🕹️ Controls

| Key | Action |
| :--- | :--- |
| **A / D** | Rotate Cannon Left/Right |
| **Space** | Fire Bullets |
| **Esc** | Exit Simulation |

---

## 📂 Project Structure

* `main.cpp`: Contains the core game loop, algorithm implementations (DDA/Circle), and rendering logic.
* `README.md`: Project documentation.

---

**Developed by Nabil Rahber** *A Computer Graphics Engineering Project focused on real-time rendering and algorithmic logic.*
