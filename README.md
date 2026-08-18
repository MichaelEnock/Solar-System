# Solar System

Welcome to the Solar System Simulation! This project is a 3D simulation of the solar system using C++ and OpenGL.

![Solar system simulation](https://camo.githubusercontent.com/8fcb81f0179bc5c11642e23984bdb11211c4d252713ee34794c76ae33afe5d52/68747470733a2f2f692e696d6775722e636f6d2f50356f373238702e676966)

## Features

- Free-fly camera with WASD movement and mouse-look, for exploring the solar system from any angle.
- Real-time orbiting and spinning planets, each with independent distance, radius, rotation speed, and orbit period.
- The Moon renders relative to Earth and orbits it directly.
- Adjustable Sun spin speed and Moon orbit speed via keyboard input.
- Directional and point lighting applied to planet surfaces.
- Skybox background for an immersive starfield.

## Controls

| Key            | Action                     |
|----------------|-----------------------------|
| `W` `A` `S` `D`| Move the camera             |
| Mouse          | Look around                 |
| Scroll Wheel   | Zoom in / out                |
| `↑` / `↓`      | Increase / decrease Sun spin speed |
| `←` / `→`      | Increase / decrease Moon orbit speed |
| `Esc`          | Close the application        |

## The Simulation

The simulated solar system currently renders the Sun, Venus, Earth (with an orbiting Moon), Mars, and Neptune, each mapped with its own texture and lit against a starfield skybox.

## Shaders and Lighting

Separate shader programs are used for different objects in the scene:

- **Planets** use a shader combining a directional light (simulating sunlight) and a point light, giving each planet realistic surface shading as it rotates and orbits.
- **The Sun** uses a simpler, unlit shader so it renders as a constant light source rather than being shaded itself.
- **The skybox** is rendered with its own shader as a static background sphere/quad, unaffected by lighting.

## Dependencies

- C++17
- OpenGL 3.3 (or later)
- [GLFW](https://github.com/glfw/glfw)
- [GLAD](https://github.com/Dav1dde/glad)
- [GLM](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb)

All dependencies are included under `dependencies/` and are statically/dynamically linked as configured in the build task.

## Building and Running

This project is built with **MinGW-w64 g++**. If you're using VS Code, the included `.vscode/tasks.json` build task is already configured to compile it.

To build manually from the project root:

```bash
g++ -g src/main.cpp src/planet.cpp src/Sphere.cpp src/Timer.cpp -o solarSystem.exe -Idependencies/include -Iinclude -Ldependencies/lib -lglfw3dll -lglad -lopengl32
```

Then run the produced executable. Make sure `glfw3.dll` is in the same directory as the executable, and that the program is run from a location where the relative `../shaders/` and `../Textures/` paths resolve correctly (i.e. run it from inside the project folder).

## Credits

Planet textures were retrieved from [solarsystemscope.com/textures](https://www.solarsystemscope.com/textures/)

An OpenGL resource that greatly helped: [learnopengl.com](https://learnopengl.com/)
