# DirectX 12 Renderer

## Overview

Deferred renderer using DirectX 12.

The renderer is structured into four passes:

- **GPass (Geometry Pass)** – Renders to four buffers:
  - Color
  - Normal and roughness
  - Specular color
  - Position in view space (potentially optimized out later)
  - Position in world space
- **LightPerspectivePass** – Renders the scene from the light’s perspective to generate a Z-buffer for shadow mapping
- **LightPass** – Creates the light map using data from GPass and LightPerspectivePass
- **FinalPass** – Combines the color buffer with the light map

The renderer allows real-time material property adjustments via a GUI. It supports three types of light (directional, point, spot) and utilizes techniques such as Phong lighting, normal mapping, specular mapping, and a reverse Z-buffer. There is still significant room for optimization and improvements.

Demo video:

[![Alt text](https://img.youtube.com/vi/J-55XKMLEYs/0.jpg)](https://youtu.be/J-55XKMLEYs).

## Technology used

- **DirectX 12**
- **DirectXTex** - for loading textures from image files
- **Assimp library** - for loading meshes and materials
- **Dear ImGui** - for gui
