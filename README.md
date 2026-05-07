# Maritime World Journey

Maritime World Journey is a C++ OpenGL/GLUT computer graphics project built for Windows and Code::Blocks. It presents four animated scenes with day/night switching, moving vehicles, weather effects, and a shared ship animation that connects the scenes.

## Features

- Four separate animated scenes rendered with OpenGL and GLUT.
- Day and night modes across the project.
- Scene-specific animations such as cars, a train, clouds, a turbine, fighter jets, birds, and boats.
- Interactive mouse and keyboard controls.
- Automatic scene progression through the ship animation.

## Scenes

### Scene 1

- Coastal landscape with mountains, clouds, land, and moving cars.
- A shared ship begins here and can move into the next scene.

### Scene 2

- Road and rail themed scene with cars, clouds, a train, and optional rain.
- The ship can be started here as well.

### Scene 3

- City/transport scene with a turbine, clouds, a bus, a blue car, and fighter jets.
- Includes a reset option for the scene.

### Scene 4

- Beach and ocean scene with sun/moon, clouds, birds, and boats.
- The ship changes direction here and can travel back and forth.

## Controls

- `1` - Switch to Scene 1
- `2` - Switch to Scene 2
- `3` - Switch to Scene 3
- `4` - Switch to Scene 4
- `d` - Switch to day mode
- `n` - Switch to night mode
- `c` - Toggle rain in Scene 2
- Left mouse button - Start the ship animation
- Left mouse button in Scene 2 - Toggle the train animation
- Left mouse button in Scene 3 - Start the blue car animation
- Left mouse button in Scene 4 - Move the ship and alternate its direction on repeated clicks
- `r` - Reset Scene 3
- `Esc` - Exit the program

## Build Requirements

- Windows
- Code::Blocks with MinGW
- FreeGLUT/OpenGL development libraries

The project file is already configured for Code::Blocks in `final_project.cbp`.

## Build And Run

1. Open `final_project.cbp` in Code::Blocks.
2. Make sure the compiler and linker paths match your local MinGW and FreeGLUT installation.
3. Build the Debug or Release target.
4. Run the generated executable from Code::Blocks or from the `bin/` folder.

If you want to build manually, link against:

- `freeglut`
- `opengl32`
- `glu32`
- `winmm`
- `gdi32`

## Project Structure

- `main.cpp` - Main rendering, animation, and input logic
- `final_project.cbp` - Code::Blocks project file
- `final_project.depend` - Dependency information used by Code::Blocks
- `final_project.layout` - Code::Blocks layout file
- `computer_graphics_report.pdf` - Project report
- `bin/` - Built executables and output files
- `obj/` - Build objects

## Notes

- The project uses GLUT timers to keep animations running only in the active scene.
- The ship animation acts as the main transition element between scenes.
- Scene 4 uses normalized coordinates, while Scenes 1 to 3 use a 1500 x 1000 viewport.
