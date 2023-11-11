# Basic Application

### Initial CMake setup

If you want to follow from scratch, let me explain how I did it.

You pick a folder somewhere on your drives. Let's call the folder `~/Projects/OpenGLGettingStarted` (on Linux) or `C:\Users\YourUserName\Documents\Projects\OpenGLGettingStarted` (on Windows).

Create a folder/file structure like this. Leave Application.{hpp/cpp} empty for now, I will explain in a minute what we do there.
```
OpenGLGettingStarted
├── lib
│   └── CMakeLists.txt
├── src
│   ├── Shared
│   │   ├── Application.cpp
│   │   ├── Application.hpp
│   │   └── CMakeLists.txt
└── CMakeLists.txt
```

So, `OpenGLGettingStarted/CMakeLists.txt` is our main project file - you could compare it to `Visual Studio`s solution file, if you worked with `VS` before. This one should look like this

```cmake title="OpenGLGettingStarted/CMakeLists.txt"
--8<-- "CMakeLists.txt:1:8"
```

Then we will need `OpenGLGettingStarted/lib/CMakeLists.txt` which has a bit more content since we need to load a bunch of dependencies, as mentioned above, `GLFW`, `GLAD` and so forth

```cmake title="OpenGLGettingStarted/lib/CMakeLists.txt"
--8<-- "lib/CMakeLists.txt"
```

FetchContent will clone the repos according to what was specified and some options are set for one or another library, like Tracy to enable tracing, or for glad to provide bindings for OpenGL 4.6

As mentioned earlier I would like to use a base class `Application` which handles input and windowing things, as well as provides a ready to use opengl context for us to use.

That means we create a `src` folder and within a `Shared` folder where we will create those two files `Application.hpp` and `Application.cpp`

`Shared` is turned into a target which we can link with all future applications to provide Application-functionality to all of them, for that we need yet another `CMakeLists.txt`.
Which will look like that

```cmake title="OpenGLGettingStarted/src/Shared/CMakeLists.txt"
--8<-- "src/Shared/CMakeLists.txt"
```

As you can see it creates a library, and needs `Application.cpp` and it depends on glfw, glad, spdlog and debugbreak.
If we have, for some reason, more things we would like to share across all other applications/examples later on, perhaps some Input class of some sort, then it would go in there too
as `Input.cpp` to be compiled into the shared library.

That applies to the other `CMakeLists.txt` too by the way. When we add new .cpp files to the project they should be added to the target. `Visual Studio` and `Clion` (since they support CMake out of the box) might add .cpp files automatically for you.

Ok then two things are missing. Bear with me its a little bit of code incoming :)

```cpp title="OpenGLGettingStarted/src/Shared/Application.hpp"
--8<-- "src/Shared/Application.hpp"
```

We said we wanted to make an Application base class which will handle windowing and input for us, and perhaps the odd other thing too.

`Run`
  
: Thats our only gate to the outside, thats what `main` will call in our actual examples later on

`ReadTextFromFile`

: Used to load text, wait for it, from a file, this will come in handy later when we load shader scripts from disk

`Initialize`

: thats the method where we initialize application relevant things like, creating the main window, adjusting it to be in center, hooking up callbacks, since we use glfw and asking for an OpenGL context

`Load`
 
: Thats empty in here for now, but this one is supposed to be called after `Initialize` and you would typically do things like loading programs, loading textures or levels before rendering anything

`Update`

: Is supposed to be for your game/business logic, where you update physics, ask the network for new state or prepare state for the gpu to render, like preparing a matrix buffer which contains all transforms of your world objects

`Render`

: Here goes all the render code, mostly draw calls

`OnFramebufferResized`

: We eventually need to handle when you resize the window, or when we go from fullscreen to windowed mode etc, that goes here, thats the spot where you want to recreate your render targets or viewports

`OnKeyDown/OnKeyUp`
: should be self explanatory

`OnOpenGLDebugMessage`

: OpenGL has this neat mechanism which is called `debug message callback`, where it will report a message when some state doesnt make sense for OpenGL and we can react to that. In our case, we will use that to halt the debugger (hence debugbreak is used) so that we are able to follow the call stack to see who the offending opengl call is causing trouble - which in most cases isnt OpenGL's fault but yours :) because you passed a wrong value to a glXXX call. We will see that later on

`ToggleFullsceen`

: As the name suggests, it goggles the window to/from fullscreen/windowed

There is also this `ApplicationAccess` thing. That only exists in `Application.cpp` which we will see next and I will explain there what is is.

```cpp title="OpenGLGettingStarted/src/Shared/Application.cpp"
--8<-- "src/Shared/Application.cpp"
```

!!! danger "TODO"

    Elaborate a bit more on all the methods in Application.cpp


!!! danger "TODO"

    Explain OpenGL context + glad


!!! danger "TODO"

    Explain the first glXXX calls


!!! danger "TODO"

    Explain glDebugMessageCallback a bit more + the big thing i use there as the callback


### HelloWindowApplication

Now is the time to create an actual project.

Let's call the target HelloWindow and lets put it in `src/01-HelloWindow`.

We need

```cpp title="src/01-HelloWindow/HelloWindowApplication.hpp"
--8<-- "src/01-HelloWindow/HelloWindowApplication.hpp"
```

and

```cpp title="src/01-HelloWindow/HelloWindowApplication.cpp"
--8<-- "src/01-HelloWindow/HelloWindowApplication.cpp"
```

and also

```cpp title="src/01-HelloWindow/Main.cpp"
--8<-- "src/01-HelloWindow/Main.cpp"
```

All that should compile, but doesn't do anything yet, obviously.

!!! failure "Attention. Attention"

    From here onwards is subject to change and under construction. Ignore please :)


The important bits here are, initialize `GLFW`, grab the screen resolution of the current primary monitor so that we can center the window
on that primary screen and also resize it to be 80% of the resolution. I really hate windows spawning at random locations everytime you start the application.

We also need an active render context, and to load all the relevant opengl functions for that active context we use `GLAD`.

To see we hooked up everything correct so far, we should see a window and its content is a nice frog green.

You might wonder wtf that `ApplicationAccess` class is. It's just a tool to be able to hook into glfw's callbacks and at the same time
keep the visibility of the handling functions still within `Application`. Otherwise those callbacks would need to be public static within `Application`
and thats another thing I dont want, expose things to the public which is not supposed to be public, anyway, fight me if you want.

```cpp
class ApplicationAccess final
{
public:
    static void FramebufferResizeCallback(
        GLFWwindow* window,
        int32_t framebufferWidth,
        int32_t framebufferHeight)
    {
        auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (application == nullptr)
        {
            spdlog::error("App: You forgot to call glfwSetWindowUserPointer in Application::Initialize");
            return;
        }

        if (framebufferWidth > 0 && framebufferHeight > 0)
        {
            application->framebufferWidth = framebufferWidth;
            application->framebufferHeight = framebufferHeight;
            application->OnFramebufferResized();
            application->Render();
        }
    }

    static void KeyCallback(
        GLFWwindow* window,
        int32_t key,
        int32_t scancode,
        int32_t action,
        int32_t modifiers
    )
    {
        auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (application == nullptr)
        {
            spdlog::error("App: You forgot to call glfwSetWindowUserPointer in Application::Initialize");
            return;
        }

        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            application->OnKeyDown(key, modifiers, scancode);
        }
        else
        {
            application->OnKeyUp(key, modifiers, scancode);
        }
    }
};
```

That's `ApplicationAccess`' implementation. It should be defined above `Application` class within `Application.cpp`.

You could probably redirect the key information to some other construct, some `InputHandler` of some kind,
rather than having 2 methods in the `Application` class, but perhaps its an exercise for you to exercise if you want.

I'm saying this because many people prefer not to have to access the `Application` class when they want to check for
keypresses in their `CameraController` :)

Ok, moving on, the rest of `Application`.

```cpp
bool Application::Load()
{
    return true;
}

void Application::Unload()
{
    if (_windowHandle != nullptr)
    {
        glfwDestroyWindow(_windowHandle);
    }

    glfwTerminate();
}

void Application::Update()
{
}

void Application::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::OnFramebufferResized()
{
    spdlog::info("Framebuffer resized to {}_{}", framebufferWidth, framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void Application::OnKeyDown(
    int32_t key,
    int32_t modifiers,
    int32_t scancode)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(_windowHandle, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F11)
    {
        ToggleFullscreen();
    }
}

void Application::OnKeyUp(
    int32_t key,
    int32_t modifiers,
    int32_t scancode)
{
}

void Application::ToggleFullscreen()
{
    _isFullscreen = !_isFullscreen;

    glfwSetWindowAttrib(_windowHandle, GLFW_DECORATED, _isFullscreen ? GLFW_FALSE : GLFW_TRUE);
    glfwSetWindowAttrib(_windowHandle, GLFW_RESIZABLE, _isFullscreen ? GLFW_FALSE : GLFW_TRUE);

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    auto primaryMonitor = glfwGetPrimaryMonitor();
    auto videoMode = glfwGetVideoMode(primaryMonitor);

    auto screenWidth = videoMode->width;
    auto screenHeight = videoMode->height;

    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);

    if (_isFullscreen)
    {
        glfwSetWindowPos(_windowHandle, monitorLeft, monitorTop);
        glfwSetWindowSize(_windowHandle, screenWidth, screenHeight);
    }
    else
    {
        auto windowWidth = static_cast<int32_t>(static_cast<float>(screenWidth) * 0.8f);
        auto windowHeight = static_cast<int32_t>(static_cast<float>(screenHeight) * 0.8f);

        glfwSetWindowPos(_windowHandle, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);
        glfwSetWindowSize(_windowHandle, windowWidth, windowHeight);
    }
}
```

Few words about what is happening here.

As mentioned before I like to keep `Initialize` and `Load` separate. `Unload` as the name suggests unloads any resources to clean up
rather then letting the OS take care of that when you ALT+F4 the app.

The methods `Update` and `Render` are the ones we will be handling our demo/example/guide code in to render stuff and to update things as they are changing.

`OnFramebufferResized` is invoked whenever we resize the window. That leads to its framebuffer (the non client recangle area) being resized too and that's
where we can hook into; to resize framebuffers or other resources which are depending on the framebuffer size.

`OnKeyDown` and `OnKeyUp` are our callbacks when keys were pressed and released. Should be sufficient for this kind of guide as also mentioned before.

We wanted `F11` to toggle windowed-window and fullscreen, now we can.

Basic application is not done yet. Let's also hookup the debug message callback, which will save our butts in the future. Alot of tutorials or blogs keep using
so called `glCall` or `glCheck` macros which run `glGetError` - even that they mess up quite a bit, because that ancient function also needs to be called correctly.

Anywho.

lets extend `ApplicationAccess` with another method like this

```cpp
static void GLAPIENTRY DebugMessageCallback(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    [[maybe_unused]] GLsizei length,
                                    const GLchar* message,
                                    [[maybe_unused]] const void* userParam)
{
    // Ignore certain verbose info messages (particularly ones on Nvidia).
    if (id == 131169 || 
        id == 131185 || // NV: Buffer will use video memory
        id == 131218 || 
        id == 131204 || // Texture cannot be used for texture mapping
        id == 131222 ||
        id == 131154 || // NV: pixel transfer is synchronized with 3D rendering
        id == 0         // gl{Push, Pop}DebugGroup
    )
    return;

    std::stringstream debugMessageStream;
    debugMessageStream << message << '\n';

    switch (source)
    {
        case GL_DEBUG_SOURCE_API: debugMessageStream << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: debugMessageStream << "Source: Window Manager"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: debugMessageStream << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: debugMessageStream << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: debugMessageStream << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER: debugMessageStream << "Source: Other"; break;
    }

    debugMessageStream << '\n';

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: debugMessageStream << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: debugMessageStream << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: debugMessageStream << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: debugMessageStream << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: debugMessageStream << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER: debugMessageStream << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: debugMessageStream << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: debugMessageStream << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: debugMessageStream << "Type: Other"; break;
    }

    debugMessageStream << '\n';

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: debugMessageStream << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: debugMessageStream << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW: debugMessageStream << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: debugMessageStream << "Severity: notification"; break;
    }

    if (userParam != nullptr)
    {
        auto windowHandle = (GLFWwindow*)userParam;
        auto application = static_cast<Application*>(glfwGetWindowUserPointer(windowHandle));
        if (application == nullptr)
        {
            spdlog::error("App: You forgot to call glfwSetWindowUserPointer in Application::Initialize");
            return;
        }

        application->OnOpenGLDebugMessage(type, debugMessageStream.str());
    }
}
```

That also means we have to extend `Application` itself with `OnOpenGLDebugMessage` like so

```cpp
protected:
...
virtual void OnOpenGLDebugMessage(uint32_t messageType, std::string_view debugMessage);
...
```

and

```cpp
void Application::OnOpenGLDebugMessage(uint32_t messageType, std::string_view debugMessage)
{
    spdlog::error(debugMessage);
    if (messageType == GL_DEBUG_TYPE_ERROR)
    {
        debug_break();
    }
}
```

The `Initialize` method also needs to be adjusted slightly to hookup the callback and enable the thing.

```cpp
glDebugMessageCallback(ApplicationAccess::DebugMessageCallback, _windowHandle);
glEnable(GL_DEBUG_OUTPUT);
glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
```

So OpenGL has this mechanism which was added as an extension in OpenGL 3.3 or so and is core since 4.3 I believe.
A much superior error revealing thing than `glGetError` ever was. It helps to actually point at the offending function
without cluttering your code with those `glCall/glCheck` macros. and provides a bit more information rather than just "INVALID HANDLE" or something like that

In this context we are actually only interested in actual errors, thats when we ask the debugger to stop here via `debug_break`. Then you can take a look
at the stacktrace and locate the offending function call that way. With that information you can then go into the [glspec](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf)
And look for the function to figure our how to call it properly.

Dont forget to include its header as well

```cpp
#include <debugbreak.h>
```

Give it a try, just add a `glEnable(0xFFFF);` at the end of `Application::Initialize`.
You should get some console output which looks like this:

```
[2023-11-05 23:53:53.768] [error] GL_INVALID_ENUM error generated. <cap> enum is invalid; expected GL_ALPHA_TEST, GL_BLEND, GL_COLOR_MATERIAL, GL_CULL_FACE, GL_DEPTH_TEST, GL_DITHER, GL_FOG, etc. (136 others).
Source: API
Type: Error
Severity: high
```