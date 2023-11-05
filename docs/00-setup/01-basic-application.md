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

    glClearColor(0.35f, 0.76f, 0.16f, 1.0f);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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