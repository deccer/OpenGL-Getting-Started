# Setup

To render a triangle or the next star citizen on screen we need some application with a window
to do it for us.

Let's create a small application which handles most common tasks like

- creating a window
- react to input
- resizing should work
- when it starts up, let it be centered and use 90% of the screen resolution
- `Escape` will close it and `F11` will toggle fullscreen

We will be using [CMake](https://www.cmake.org) to be our project generator, `C++` our programming language of choice,
[GLFW](https://www.glfw.org) our window and input glue, [GLAD](https://glad.dav1d.de/) our opengl loader
and we will use all those things to come up with an `Application` class.

On top of all that, we might be using [spdlog](https://github.com/gabime/spdlog) to handle logging.
I can't stand `printf` or `std::cout` anymore. Logging is a solved problem. People will complain about its "slow" compile times,
but this doesnt really matter here.

To display some debug data we wont be using the console window like everyone else.
We will integrate [Dear Imgui](https://github.com/ocornut/imgui) instead, right away, because why not.
You might want to use it in the future anyway, for showing values for debugging purposes
or want to click a thing to change a thing while the program is running, so, why not integrate it right away, I will also explain how.

As mentioned, we will be using `CMake` to handle/build our project. I will not explain what `CMake` is or how exactly it works, but
in the following paragraph i will comment what is what a bit here and there. Should be somewhat self explanatory.

Before we start, let me try to explain the intended project structure.


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

Shared will contain all the things which we might be sharing across the projects. This repo will contain the final thing.
There won't be dedicated git branches per chapter or something like that. However I will explain everything in this guide
and we will start from a project skeleton and progressively add things to it and modify things as we go.