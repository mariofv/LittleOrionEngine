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
  <a href="">
    <img alt="GitHub repo size in bytes" src="https://img.shields.io/github/repo-size/mariofv/OrionEngine.svg">
  </a>
  <a href="https://github.com/mariofv/OrionEngine/blob/master/LICENSE">
    <img alt="GitHub license" src="https://img.shields.io/github/license/mariofv/OrionEngine.svg">
  </a>
</p>

![](https://github.com/mariofv/OrionEngine/blob/master/Media/engine_usage_v0_1_5-alpha.gif)

## Installation

- Download the last release (or whatever release you want) from the [Releases section](https://github.com/mariofv/OrionEngine/releases).
- Extract `Build.zip`.
- Execute `OrionEngine.exe`.

## Usage


## Development setup

In order to do that assure that you have the following:
- A computer.
- A graphic card with OpenGL support.
- [VisualStudio 2017 or above](https://visualstudio.microsoft.com/es/).

## Contributing

Because this is a academic project is not possible to contribute directly to this repo. Said that, feel free to fork it (<https://github.com/mariofv/OrionEngine/fork>) and to expand it in your own way!

## Contributors
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
