# Camera Controls

The software rasterizer now includes a controllable camera system with two modes:

## FPS Mode (First-Person Shooter)
- **WASD** - Move forward/backward/left/right
- **Q/E** - Move up/down
- **Arrow Keys** - Look around (when mouse look is disabled)
- **Tab** - Toggle mouse look (allows looking around with mouse)
- **Mouse Movement** - Look around (when mouse look is enabled)

## Orbit Mode
- **WASD** - Zoom in/out
- **Arrow Keys** - Orbit around target
- **Left Mouse Drag** - Orbit around target with mouse
- **Mouse Wheel** - Zoom in/out

## General Controls
- **C** - Switch between FPS and Orbit camera modes
- **R** - Toggle cube rotation
- **ESC** - Exit application (when mouse look is enabled)

## Tips
- Start in FPS mode by default
- Use Tab to enable mouse look for smoother camera control
- In Orbit mode, the camera orbits around the origin (0,0,0)
- Camera speed and sensitivity can be adjusted in the code