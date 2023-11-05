# Basic Application

Let's prepare a base application to handle all the glue around the renderer.

An application is just a shell of functions which do things to run the things we want.
In my world I like to have an `Application` base class which looks like this

### Application.hpp
```cpp
#pragma once

struct GLFWwindow;

class Application
{
public:
    void Run();

protected:
    bool Initialize();
    bool Load();
    void Unload();

    void Update();
    void Render();

    virtual void OnFramebufferResized();
    virtual void OnKeyDown(
        int32_t key,
        int32_t modifiers,
        int32_t scancode);
    virtual void OnKeyUp(
        int32_t key,
        int32_t modifiers,
        int32_t scancode);

private:
    GLFWwindow* _windowHandle;
    void ToggleFullscreen();
};
```

Only one public method, which will be the entry point for the `main` function.

Since we will be using `GLFW` and we only need to support one window, `Application` will hold the native glfw handle
to the window.

And the rest of the skellington.

### Application.cpp
```cpp
#include "Application.hpp"

void Application::Run()
{
    if (!Initialize())
    {
        return;
    }

    if (!Load())
    {
        return;
    }

    while (true)
    {
        Update();

        Render();
    }

    Unload();
}

bool Application::Initialize()
{
    return true;
}

bool Application::Load()
{
    return true;
}

void Application::Unload()
{

}

void Application::Update()
{

}

void Application::Render()
{

}
```

I like to have `Initialize` and `Load` two separate things. `Initialize` contains things which are usually only necessary to call during startup of the application.
Things like creating a window, initializing a render context, fetching device capabilities etc. and `Load` will contain things which can only be loaded
when there is an active render context going on already. Things like loading and compiling shaders, textures and other resources.

As you can see all methods but `Run` are protected. That means we won't be able to call them directly.
And instantiating the `Application` class like that won't make much sense either.

Let's keep these 2 files in `src/Shared` since we want to reuse them for all the projects we make.

Now is the time to create an actual project.

Let's call it HelloWindow and lets put it in `src/01-HelloWindow`.

We need

### HelloWindowApplication.hpp
```cpp
#pragma once

#include "../Shared/Application.hpp"

class HelloWindowApplication : public Application
{
protected:
    void Update();
};
```

and

### HelloWindowApplication.cpp

```cpp
#include "HelloWindowApplication.hpp"

void HelloWindowApplication::Update()
{
    Application::Update();
}
```

and also

### Main.cpp
```cpp
#include "HelloWindowApplication.hpp"

int32_t main(
    [[maybe_unused]] int32_t argc,
    [[maybe_unused]] char* argv[])
{
    HelloWindowApplication application;
    application.Run();
    return 0;
}
```

All that should compile, but doesn't do anything yet, obviously.

As mentioned earlier being sick of `printf` or `std::cout` we will use `spdlog` everywhere, where things should be output somehow.
Errors, warnings or the occasional status.

With that in mind, lets fill out the rest of the skellington that is the `Application` class.

### Application::Run
```cpp
void Application::Run()
{
    if (!Initialize())
    {
        return;
    }

    spdlog::info("App: Initialized");

    if (!Load())
    {
        return;
    }

    spdlog::info("App: Loaded");

    while (!glfwWindowShouldClose(_windowHandle))
    {
        glfwPollEvents();

        Update();

        Render();

        glfwSwapBuffers(_windowHandle);
    }

    spdlog::info("App: Unloading");

    Unload();

    spdlog::info("App: Unloaded");
}
```

### Application::Initialize
```cpp
bool Application::Initialize()
{
    if (glfwInit() == GLFW_FALSE)
    {
        spdlog::error("Glfw: Unable to initialize");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    auto primaryMonitor = glfwGetPrimaryMonitor();
    auto videoMode = glfwGetVideoMode(primaryMonitor);

    auto screenWidth = videoMode->width;
    auto screenHeight = videoMode->height;

    auto windowWidth = static_cast<int32_t>(static_cast<float>(screenWidth) * 0.8f);
    auto windowHeight = static_cast<int32_t>(static_cast<float>(screenHeight) * 0.8f);

    _windowHandle = glfwCreateWindow(windowWidth, windowHeight, "OpenGL - Getting Started", nullptr, nullptr);
    if (_windowHandle == nullptr)
    {
        const char* errorDescription = nullptr;
        auto errorCode = glfwGetError(&errorDescription);
        if (errorCode != GLFW_NO_ERROR)
        {
            spdlog::error("GLFW: Unable to create window Details_{}", errorDescription);
        }
        return false;
    }

    glfwSetWindowUserPointer(_windowHandle, this);

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);
    glfwSetWindowPos(_windowHandle, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);

    glfwSetFramebufferSizeCallback(_windowHandle, ApplicationAccess::FramebufferResizeCallback);
    glfwSetKeyCallback(_windowHandle, ApplicationAccess::KeyCallback);

    glfwMakeContextCurrent(_windowHandle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_FRAMEBUFFER_SRGB);

    glClearColor(0.35f, 0.76f, 0.16f, 1.0f);
    glClearDepthf(1.0f);

    return true;
}
```

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

`OnFramebufferResized` is invoked whenever we resize the window. That leads to its framebuffer (the non client recangle area) being resized too and thats
where we can hook into, to resize framebuffers or other resources which are depending on the framebuffer size.

`OnKeyDown` and `OnKeyUp` are our callbacks when keys were pressed and released. Should be sufficient for this kind of guide as also mentioned before.

We wanted `F11` to toggle windowed-window and fullscreen. Well there you have it.

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

Give it a try, just add a `glEnable(0xFFFF);` at the end of `Application::Initialize`.
You should get some console output which looks like this:

```
[2023-11-05 23:53:53.768] [error] GL_INVALID_ENUM error generated. <cap> enum is invalid; expected GL_ALPHA_TEST, GL_BLEND, GL_COLOR_MATERIAL, GL_CULL_FACE, GL_DEPTH_TEST, GL_DITHER, GL_FOG, etc. (136 others).
Source: API
Type: Error
Severity: high
```