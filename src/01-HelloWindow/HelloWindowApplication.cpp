#include "HelloWindowApplication.hpp"

#include <spdlog/spdlog.h>

void HelloWindowApplication::Update()
{
    Application::Update();
    spdlog::info("Hello from HelloWindowApplication");
}