Find a location of your choice somewhere on your file system.

Perhaps something like

(Linux) `~/Projects/OpenGLGettingStarted`
(Windows) `C:\Projects\OpenGLGettingStarted`

And we shall refer to it as the project directory.

Now try to reproduce the following structore of the project directory on your system

```
OpenGLGettingStarted
├── lib
│   └── CMakeLists.txt             # here we describe all third party dependencies
│                                  # like glfw, glad, spdlog, imgui, ...
├── src
│   ├── 01-01-BasicWindow
│   │   ├── Main.cpp               # guess what is in here
│   │   └── CMakeLists.txt         # or here
└── CMakeLists.txt                 # Thats our solution file
```

Next step is to fill the `CMakeLists` files, so that `CMake` knows what to do.

Lets start with `lib/CMakeLists.txt`

When we create our first window we have some dependencies, namely `GLFW`, `GLAD` and `spdlog`.
`CMake` has a mechanism called [FetchContent](https://cmake.org/cmake/help/v3.28/module/FetchContent.html) which we will be using to pull those dependencies.

```cmake title="lib/CMakeLists.txt"
--8<-- "lib/CMakeLists.txt:3:5"
```

Those two line mean, make `FetchContent` available for us to use, and we would also like to be sure that our platform can handle `OpenGL` otherwise `CMake` would fail
to prepare the project and let us know. But its unlikely that your system will not support OpenGL. We will also not be supporting anything but a somewhat modern
Linux Distribution, like Arch or Fedora or the likes, or Windows. Operating systems from Apple will not be supported, as they lack support for the OpenGL version we are
going to target.

Next few lines will be pulling the aforementioned dependencies

`GLFW`

: We want the `GLFW` sources basically, but we dont need its tests, its example or docs built, we also dont want it to install stuff to somewhere,
  just build so that we can link it together with the rest of the application later.
  ```cmake title="lib/CMakeLists.txt"
  --8<-- "lib/CMakeLists.txt:7:22"
  ```

`GLAD`

: GLAD is a functions loader for `OpenGL`. It takes the `OpenGL` specification xml for the targetted version and generates function bindings for us, which we need to load
  when we have a render context available. What that is I will explain later.
  ```cmake title="lib/CMakeLists.txt"
  --8<-- "lib/CMakeLists.txt:24:42"
  ```

`spdlog`

: a logging framework which provides structured logging facilities. No more weird `printf` or `std::cout`.
  ```cmake title="lib/CMakeLists.txt"
  --8<-- "lib/CMakeLists.txt:44:55"
  ```

Next one is `CMakeLists.txt` in our project directory.

That is our main project file.

```cmake title="CMakeLists.txt"
--8<-- "CMakeLists.txt:1:7"
```

It declares the project name, which c++ standard we want to target and which targets we want to add.
Those are a) our dependencies which are declared in `lib/CMakeLists.txt` hence the `add_directory(lib)` line
as well as b) our actual projects we are going to write in this guide.

Let's put in our first project in too, right below `add_subdirectory(lib)`

```cmake
add_subdirectory(src/00-BasicWindow)
```

And then we move on to BasicWindow's CMakeLists.txt and Main.cpp

```cmake title="src/01-01-BasicWindow/CMakeLists.txt"
--8<-- "src/01-01-BasicWindow/CMakeLists.txt"
```

Again declare a target, in this case `01-01-BasicWindow` (I try to keep the docs pages in sync with the target names to aid my brain while writing this).
Specify which cpp file/unit to build.

Then there is a small block which detects whether you are using Microsoft's C++ Compiler or not, and
configure the compiler to always treat warnings as errors.

Then we make `spdlog`'s include directory available to the target and link the program with our dependencies.

Finally we can start working on the first c++ parts. We will keep everything in one file, to create a window and react to keyboard input `ESC` and `F11`.

Let's start.

### Main.cpp

I like to use `int32_t`, `uint16_t` over types/aliases like `int`, or `ushort`. For that we include `cstdint`.

We also include glad and glfw header as well as spdlog's. When it comes to glad and glfw we need to be careful here to include glad before glfw.
C++ is weird and sometimes the order of includes is important, because certain compiler definitions/switches are declared somewhere else which are picked up by something else.

That's one of the cases here.

Plus the usual entry point for your bog standard c/c++ program.

It all should look like this:

```cpp
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

int main(int32_t argc, char* argv[])
{
    return 0;   
}
```

You might want to reconfigure the project here, to be sure all deps are loaded and `CMake` knows how to build all this stuff.
in `VSCode` press `Ctrl+Shift+p` and look for `CMake: Delete Cache and Reconfigure`. It is probably called very similar in `Clion` or `Visual Studio`.

`VSCode` will probably say something like this afterwards:

```bash
[cmake] -- Build files have been written to: /home/deccer/Projects/OpenGL-Getting-Started/build
```

If not you most likely goofed something up, please check if you have setup the project structure as I described or if there is a typo somewhere.

If you  try to build it as is. You should get an error saying something like that:

```
error: unused parameter ‘argc’ [-Werror=unused-parameter]
```

And another one for `argv` because we dont really use them anywhere. We still want to treat warnings as errors, but we cant really fix anything here,
instead we tell the compiler that we are aware of it, and annotate the 2 parameters with an attribute called `[[maybe_unused]]`.

The whole thing should look like

```cpp
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

int main(
    [[maybe_unused]] int32_t argc,
    [[maybe_unused]] char* argv[])
{
    return 0;   
}
```

If you build now, it should succeed.

Next step, create a window. In order to create a window we have to do a few things first, because we want a window which provides us with a render context. To tell `GLFW` what parameters we want we have to give it a few hints, literally.

Well, first we initialize `GLFW` itself by calling `glfwInit` and check its result value. It could fail because for some reason we didnt link glfw with our project or so.

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:12:16"
```

This is also the first time we use `spdlog`, instead of `printf` or `std::cout`.

Now the hints. We want to use `OpenGL`, we would like to target OpenGL 4.6 and its core profile.
On top of that, we want he window be resizable, and come with window decoration (border, top window bar and system menu/buttons)

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:18:25"
```

When we create the window we can specify its width and height. Earlier I mentioned that I like my windows centered, and in the case of the OpenGLGettingStarted thing also be 80% of whatever resolution you use at the moment. Therefore we ask the system about the current resolution to derive the window dimensions from.

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:27:34"
```

Then we can actually create the window, by passing the window dimensions and a title. We don't want to use exclusive fullscreen. 

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:36:46"
```

Window creation can fail for various reasons, that's why we are going to ask it for an actual error message, if its not able to create a window.

By the way you can always check `GLFW`'s docs for more information.

Now we have a window, but I like it centered, as mentioned before, `GLFW` doesn't automatically center the window for us unlike `SDL2` (another library providing windowing and input glue, amongst other things)

So that's what we will be doing here

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:48:51"
```

We can also interact with the window now, that means send keyboard input, or mouse events, resize the window, minify or maxify it to mention a few. We will focus on 2 for now.

Resizing and keyboard input.

To be able to get a feedback when a window has been resized or key pressed/released we need to hook up 2 callbacks using the following calls.

Resize first. Windows consist of roughly 2 major things. A non client area and the client area. The former is everything like border which can be 1-n pixels thick, and the title bar which has a caption, the usual buttons to minimize, maximize and close a window and the system menu. The latter is usually the part wrapped by the non client area. The client area is the space we have available to draw our stuff to/on. Now when a window gets resized, the client area is resized as well, automatically.

Perhaps a ugly picture helps.

![](https://www.equestionanswers.com/vcpp/images/windows-client-non-client-area.gif)

Since we only want to focus on that area in particular we will setup a callback which will listen to that change so that we dont have to worry about calculating weird offsets/margins when we have to take border sizes and window title bars into account when it comes to sizes.

`GLFW` also calls that non client area a framebuffer, and its resize callback FramebufferSizeCallback.

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:53:59"
```

Resizing the framebuffer will come in handy later, when we have to adjust your projection matrices and framebuffer attachments (we will talk about what it is later)

We also need some form of handling keyboard input. `GLFW` has a callback for that as well

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:61:72"
```

The window should close when we press `ESC`, thats what we do here, tell the window that next time its events are updated, it will see that we want it to close and it closes. We will see that exact evalution few lines below from here in a second.

The next two lines are important.

OpenGL is a state machine. An OpenGL context holds that state. The state contains information such as which textures are bound to which texture units, which attachments the current framebuffer object has and things like that.

When you set the current context, you are switching all the state from the old context to the new context.

We dont have an old context nor another one than just this one, but thats what you have to call at least once in your application. 

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:74:75"
```

The call after `glfwMakeContextCurrent` is as important, as it loads all necessary OpenGL function pointers, so that we can actually use them.

Then we will be setting some of that mentioned OpenGL state explicitly to their default values.

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:77:83"
```

!!! danger "TODO"

    Explain the states, explain srgb here?

### Main Loop

Next block, we are also almost finished, is the so called game loop, a simple loop which runs as long as the window is upen. It refreshes all state `GLFW` knows about like which keys have been pressed, where is the window being moved to or resized upto what dimensions or whether a joystick has been plugged in amongst other things.

`GLFW` created a double buffered window for us, and the `glfwSwapBuffers` swaps those buffers whenever they are ready to be swapped.

```cmake title="src/01-01-BasicWindow/Main.cpp"
--8<-- "src/01-01-BasicWindow/Main.cpp:77:83"
```

That game loop is the place where most of the magic happens. You update your game objects, read key from the keyboard and mouse, queue sounds to be played, receive network packets perhaps, and most importantly render your virtual world.

!!! danger "TODO"

    Explain swapping buffers a bit more here

What happens after we exit that game loop?

Exactly, we clean up. Destroy the main window and terminate `GLFW` so that it can clean up its internal state.

And then we return to the `OS``.

In the next chapter we will add all the necessary things to render a triangle.
