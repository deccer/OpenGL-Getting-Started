# Setup

To render a triangle or the next star citizen on screen we need some application with a window
where we can render our world into.

Let's try to slowly build up an application which gets improved more and more over each chapter

To start we create a small application which handles most common tasks like

- creating a window
- react to input
- resizing should work in general, we dont want it to be perfect (that's a long story)
- when it starts up, let it be centered and use 90% of the screen resolution
- `Escape` will close it
- `F11` will toggle fullscreen

We should also decide what tools and languages, extensions, editors and or IDEs we pick.

We will be using the following languages, programs, extensions and libraries

- [C++](https://isocpp.org/get-started) as our main programming language
- [GLSL](https://www.opengl.org/registry/doc/GLSLangSpec.4.60.pdf) our shading programming language, since we need to program some things for the `GPU`
- [CMake](https://www.cmake.org) will be our project generator, to generate `C++` projects, which we can build with
- [Visual Studio Code]() our IDE of choice, into which we will be installing the 
- [CMake Tools](https://github.com/microsoft/vscode-cmake-tools) a Visual Studio extension which will be interfacing with cmake, no need to run cmake via cli then, which is quite nice

Library wise we will be using

- [GLFW](https://www.glfw.org) our window and input glue
- [GLAD](https://glad.dav1d.de/) our opengl functions loader
- [GLM](https://github.com/g-truc/glm) will be our math library
- [STB](https://github.com/nothings/stb.git) we will be using this to load images to create textures
- [fastgltf](https://github.com/spnda/fastgltf) will be used later down the road to load glTF models from disk.

We will be adding some more fancy bits and bops in throughout the tour. Amongst those things are/will be

- [spdlog](https://github.com/gabime/spdlog) to handle logging - i can't stand `printf` and or `std::cout` anymore. I prefer structured logging. You can read more here if you want to know more what that is
- [debugbreak](https://github.com/scottt/debugbreak) a portable debug break implementation which we will be using for our error opengl problem handling
- [Dear Imgui](https://github.com/ocornut/imgui) to show off some debug values in the window itself, rather than printeffing some nonsense into the console window, also allows us to build an actual useful UI, could potentially turn into an editor of sorts or just debug controls
- [Tracy](https://github.com/wolfpld/tracy.git) a frame profiler which can and will show us some profiling data later on to see how fast things run


I wont be explaining how to install `Visual Studio Code`, `Visual Studio`, `CLion`. I'm sure you can follow the links mentioned above and install the programs yourself according to their instructions.

All the other things, from `GLFW` to `fastgltf` CMake will take care of it and download the necessary files.

For the project itself I had the following project structure in mind, so that you can also see where goes what

```cpp
ProjectRoot
├── lib
│   └── CMakeLists.txt                    # here we describe all third party dependencies
│                                         # like glfw, glad, spdlog, imgui, ...
├── src
│   ├── Shared
│   │   ├── Application.cpp               # guess what is in here
│   │   ├── Application.hpp               # or here
│   │   └── CMakeLists.txt                # project file for the shared library
│   │   01-HelloWindow
│   │   ├── Data
│   │   │   └── Shaders
│   │   │       ├── Simple.vs.glsl        # vertex shader
│   │   │       └── Simple.fs.glsl        # fragment shader
│   │   ├── HelloWindowApplication.cpp    # our application bits for this project
│   │   ├── HelloWindowApplication.hpp
│   │   └── CMakeLists.txt                # project file for HelloWindow
│   │   02-HelloTriangle
│   │   ├── HelloTriangleApplication.cpp
... ... ...
│   │   └── CMakeLists.txt                # project file for HelloTriangle
│   ...and so forth...
└── CMakeLists.txt                        # Thats our solution file
```
