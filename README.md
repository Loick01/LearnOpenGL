# LearnOpenGL (WIP)

This repository contains my implementation of the chapters from the [LearnOpenGL](https://learnopengl.com/) tutorial. Each chapter has its own directory in [src](src). Source files in [includes](includes) and [externals](externals) are shared across all chapters.

<table align="center">
  <tr>
    <td align="center">
      <img src="./screenshot/camera.png" height="300"/>
      <br>
      <strong>Camera (10)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/lighting_map.png" height="300"/>
      <br>
      <strong>Ligthing maps (15)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/light_caster.png" height="300"/>
      <br>
      <strong>Light casters (16)</strong>
    </td>
  </tr>

  <tr>
    <td align="center">
      <img src="./screenshot/mesh_loading.png" height="300"/>
      <br>
      <strong>Mesh loading (20)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/blending.png" height="300"/>
      <br>
      <strong>Blending (24)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/cubemaps.png" height="300"/>
      <br>
      <strong>Cubemap (27)</strong>
    </td>
  </tr>

  <tr>
    <td align="center">
      <img src="./screenshot/geometry_shader.png" height="300"/>
      <br>
      <strong>Geometry shader (30)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/advanced_lighting.png" height="300"/>
      <br>
      <strong>Advanced lighting (33)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/shadow_map.png" height="300"/>
      <br>
      <strong>Shadow mapping (35)</strong>
    </td>
  </tr>

  <tr>
    <td align="center">
      <img src="./screenshot/normal_map.png" height="300"/>
      <br>
      <strong>Normal mapping (37)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/parallax_mapping.png" height="300"/>
      <br>
      <strong>Parallax mapping (38)</strong>
    </td>
    <td align="center">
      <img src="./screenshot/deferred_shading.png" height="300"/>
      <br>
      <strong>Deferred shading (41)</strong>
    </td>
  </tr>
</table>

### How to compile

```bash
# Run these commands from the directory of a chapter you want to execute
mkdir build
cd build
cmake ..
make
./main
```

### Requirements

+ C++17
+ CMake >= 3.10
+ GLFW 3
+ GLAD, GLM and Assimp are already included in [externals](externals)

### Controls

+ **W/A/S/D :** Move the camera forward/backward/left/right
+ **Mouse wheel :** Zoom in/out
+ **Spacebar/Left control :** Move the camera up/down
+ **Mouse :** Look around
+ **Escape :** Close the window