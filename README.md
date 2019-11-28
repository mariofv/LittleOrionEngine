<h1 align="center" style="border-bottom: none;">Orion Engine</h1>
<h3 align="center">C++ 3D Engine developed during the Master's Degree in AAA Videogames Development</h3>
<p align="center">
  <a href="https://github.com/mariofv/OrionEngine/releases/">
    <img alt="GitHub version" src="https://img.shields.io/github/v/release/mariofv/OrionEngine?include_prereleases">
  </a>
  <a href="https://github.com/mariofv/OrionEngine/stargazers/">
    <img alt="GitHub stars" src="https://img.shields.io/github/stars/mariofv/OrionEngine.svg">
  </a>
  <a href="https://github.com/mariofv/OrionEngine/network/">
    <img alt="GitHub forks" src="https://img.shields.io/github/forks/mariofv/OrionEngine.svg">
  </a>
  <a href="https://github.com/mariofv/OrionEngine/">
    <img alt="GitHub repo size in bytes" src="https://img.shields.io/github/repo-size/mariofv/OrionEngine.svg">
  </a>
  <a href="https://github.com/mariofv/OrionEngine/blob/master/LICENSE">
    <img alt="GitHub license" src="https://img.shields.io/github/license/mariofv/OrionEngine.svg">
  </a>
</p>

![](https://github.com/mariofv/OrionEngine/blob/master/Media/engine_usage_v0_1_5-alpha.gif)

## Highlights
- Load and visualize 3D models.
- Change model texture with one of your choice.
- Explore the scene using a Unity-like camera.
- Configure different engine properties: renderer, time management, ...

## Installation

- Download the last release (or whatever release you want) from the [Releases section](https://github.com/mariofv/OrionEngine/releases).
- Extract `Build.zip`.
- Execute `OrionEngine.exe`.

## Usage
### Model and texture loading
- In order to load a model drag and drop a `fbx` file into the editor's window to load it. The engine will automatically try to import model materials from the specified path in the `fbx` file.
- To change the texture of the current model drag and drop a `png` or `dds` file into the editor's window. The engine will overwrite the existing texture with the new one.

### Camera controls
- To enable camera movement hold `RClick` inside the _Scene_ window.
- Move mouse to look around.
- Press `W` `A` `S` `D` to move camera around.
- Press `Q`/`E` to increase/decrease camera height.
- Use the mouse wheel to zoom.
- Hold `LShift`to duplicate camera movement speed.
- Press `F` to focus in the loaded model.
- With movement disabled, press `LAlt` + `LClick` to orbit the current object.

### Engine configuration
In _Configuration_ window you can check and configure Engine properties.
- **Hardware** shows some current computer hardware and graphical hardware info, like VRAM usage.
- **Window** configures engine's window properties.
- **Renderer** configures different `OpenGL` parameters.
- **Camera** shows camera frustum parameters like up, front and position vectors. Also other frustum parameters like near and far plane can be configured.
- **Timers** configures engine's time management. Here you can cap FPS, increase/decrease time scale, ...
- **Input** shows current input info.

## Development setup

In order to do that assure that you have the following:
- A computer.
- A graphic card with OpenGL support.
- [VisualStudio 2017 or above](https://visualstudio.microsoft.com/es/).

## Contributing

Because this is a academic project is not possible to contribute directly to this repo. Said that, feel free to fork it (<https://github.com/mariofv/OrionEngine/fork>) and to expand it in your own way!

## Authors
| [![Mario Fernández Villalba](https://github.com/mariofv.png?size=100)](https://github.com/mariofv) |
| ---- |
| [Mario Fernández Villalba](https://github.com/mariofv/) |

## Built With

* [SDL 2.0](https://www.libsdl.org/) - Used to manage window creation and input.
* [glew 2.1.0](http://glew.sourceforge.net/) - Used to manage OpenGL extensions.
* [MathGeoLib 3.0.0](https://github.com/juj/MathGeoLib/) - Used to do the maths.
* [Dear ImGui 1.73](https://github.com/ocornut/imgui/) - Used to build the GUI.
* [DevIL 1.8](http://openil.sourceforge.net/) - Used to load images.
* [assimp 5.0](https://github.com/assimp/assimp/) - Used to load 3D models.
* [Font Awesome 5](https://github.com/FortAwesome/Font-Awesome) - Used to display fancy icons.
* [Icon Font Cpp Headers](https://github.com/juliettef/IconFontCppHeaders/) - Used to import Font Awesome icons in C++.

## License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/mariofv/OrionEngine/blob/master/LICENSE) file for details.

## Release History

* [0.2.0 And there were GameObjects](https://github.com/mariofv/OrionEngine/releases/tag/v0.2.0-alpha)
    * GameObjects come in. Each GameObject represents an entity that can be modified using components.
    * Components are objects that contain different entities parts, like meshes, materials, etc.  
    * New hierarchy window that represents the scene GameObject hierarchy.
    * Revamped properties window. Here component parameters can be modified.
* [0.1.5 Final alpha version (this is the one teachers)!](https://github.com/mariofv/OrionEngine/releases/tag/v0.1.5-alpha)
    * Fixed various bugs.
    * Several refactors in order to follow good C++ practises.
    * Super cool README.
* [0.1.4 New LOG, preparations for final 0.1 alpha version!](https://github.com/mariofv/OrionEngine/releases/tag/v0.1.4-alpha)
    * Revamped log. It displays messages in a clearer and more readable way.
    * Debug messages are captured from Assimp and OpenGL.
    * Filter messages by soruce!
    * Model texture can be overwritten dragging and dropping a png file into the engine window.
* [0.1.3 wow UI, so much configuration options](https://github.com/mariofv/OrionEngine/releases/tag/v0.1.3-alpha)
    * New gorgeous GUI.
    * Lots of new configuration options for different modules (camera, render, ...).
    * Scene is rendered in a separate window.
    * Model properties are shown in a new window.
    * Camera speed and focus is calculated regarding the loaded model size.
* [0.1.2 Time starts to flow, impending new GUI shows in the horizon...](https://github.com/mariofv/OrionEngine/releases/tag/v0.1.2-alpha)
    * Added time module, that controls time flow. It implements two clocks, the real time clock and the game clock.
    * Time can be paused and stepped foward one frame. Also time scale can be changed.
    * FPS can be limited.
    * Started new implementation of GUI. Added some new info to camera config.
* [0.1.1 This is about model loading and a camera](https://github.com/mariofv/OrionEngine/releases/tag/v0.1.1-alpha)
    * Improved model loading. Is possible to load models dragging and dropping their .fbx files into the engine's window. Camera will be scaled accordingly to new model dimensions.
    * Improved camera controls. Pressing F will focus the camera on the loaded model. Also, pressing Alt will make the camera orbit around the current model.
    * Added bounding box. Pressing B will enable/disable the bounding box of the current model.
* [0.1.0 Here comes the base version!](https://github.com/mariofv/OrionEngine/releases/tag/v0.1.0-alpha)
    * It can load models from fbx files using a diffuse texture.
    * Camera can be controlled in Unity-like style.
    * Engine has basic UI with several config and debug options.
