#pragma once

#include <cstdint>

struct GLFWwindow;

class Application
{
public:
    virtual ~Application();

    void Run();

protected:
    virtual bool Initialize();
    virtual bool Load();
    virtual void Unload();

    virtual void Update();
    virtual void Render();

    virtual void OnFramebufferResized();
    virtual void OnKeyDown(
        int32_t key,
        int32_t modifiers,
        int32_t scancode);
    virtual void OnKeyUp(
        int32_t key,
        int32_t modifiers,
        int32_t scancode);

    int32_t framebufferWidth = 0;
    int32_t framebufferHeight = 0;    

private:
    friend class ApplicationAccess;

    GLFWwindow* _windowHandle = nullptr;
    bool _isFullscreen = false;

    void ToggleFullscreen();
};