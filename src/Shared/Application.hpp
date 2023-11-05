#pragma once

#include <cstdint>

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

private:
    GLFWwindow* _windowHandle;
};